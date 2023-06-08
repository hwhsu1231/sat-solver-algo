#include "solver.h"

// Return result
vector<int> solver::result()
{
    if (!sat) {
        if(runTime>timeout)
            return vector<int>(1, -1);
        else
            return vector<int>(1, 0);
    }

    vector<int> ret(maxVarIndex+1, 1);
    for(int i=1; i<=maxVarIndex; ++i)
        ret[i] = var.getVal(i) ? i : -i;
    return ret;
}

void solver::printCNF()
{
    for(auto &cls : clauses) {
        for(int i=0; i<cls.size(); ++i)
            printf("%d ", cls.getLit(i));
        printf("0\n");
    }
}

int solver::size()
{
    return maxVarIndex;
}

int solver::getVarCount()
{
    return (int)maxVarIndex;
}

int solver::getClauseCount()
{
    return (int)clauses.size();
}


// Solver helper function
static bool satisfyAlready(const vector<int> &cls)
{
    unordered_set<int> checker;
    for (auto &v : cls)
    {
        if (checker.find(-v) != checker.end())
        {
            return true;
        }
        checker.insert(v);
    }
    return false;
}


/// @brief 將指定元素新增到列表的尾部。
/// @param pool 存放 solver::WatcherInfo 物件的 vector，表示元素的集合。
/// @param head 目標列表的頭部。
/// @param eleid 指定要新增到列表的元素索引。
static void appendListWatcher(vector<solver::WatcherInfo> &pool, int &head, int eleid)
{
    // 如果列表是空的，將頭部設置為指定元素，並使指定元素的 prev 和 next 指向自身。
    if( head == -1 ) {
        head = eleid;
        pool[eleid].prev = pool[eleid].next = eleid;
        return;
    }
    int prev = pool[head].prev;
    // 設定指定元素的 next 為列表的頭部，prev 為原先最後一個元素的索引
    pool[eleid].next = head;
    pool[eleid].prev = prev;
    // 將原先最後一個元素的 next 指向指定元素，頭部的 prev 指向指定元素
    pool[prev].next = eleid;
    pool[head].prev = eleid;
}


/// @brief 交換 vector 中指定元素的位置，將指定元素從一個列表移動到另一個列表。
/// @param pool 存放 solver::WatcherInfo 物件的 vector，表示元素的集合。
/// @param from 指定元素所在的原始列表的頭部。
/// @param to 指定元素要移動到的目標列表的頭部。
/// @param eleid 指定要移動的元素的索引。
static void swapListWatcher(vector<solver::WatcherInfo> &pool, int &from, int &to, int eleid)
{
    // 檢查原始列表的頭部是否是要移動的元素
    if( eleid == from )
    {
        if( pool[from].next == from )
            from = -1;
        else
            from = pool[eleid].next;
    }
    // 從原始列表中移除指定元素
    pool[pool[eleid].prev].next = pool[eleid].next;
    pool[pool[eleid].next].prev = pool[eleid].prev;
    // 將指定元素新增到目標列表的尾部
    appendListWatcher(pool, to, eleid);
}


// static inline int __idx(int lit) {
//     return (abs(lit)<<1) + (lit>0);
// }


/// @brief 通過 CNF 檔案進行初始化
/// @param filename CNF 檔案名稱
/// @param mode 執行模式
void solver::init(const char *filename, int mode)
{
    // 初始化空的求解器
    *this = solver();
    runMode = mode;

    // 從 CNF 檔案中獲取原始子句
    vector< vector<int> > raw;
    parse_DIMACS_CNF(raw, maxVarIndex, filename);

    // 初始化變數堆疊
    var = opStack(maxVarIndex+4);

    // 初始化用於學習子句的容器
    nowLearnt.resize(maxVarIndex << 1 + 4);

    // 初始化臨時表
    litMarker.init(maxVarIndex+4);
    delMarker.init(maxVarIndex+4);

    // 初始化啟發式資訊
    varPriQueue.init(maxVarIndex);

    for (auto &cls : raw)
    {
        if (cls.empty())
        {
            unsatAfterInit = 1;
        }
        else if (cls.size() == 1)
        {
            unit.emplace_back(cls[0]);
        }
        else if (!satisfyAlready(cls))
        {
            clauses.push_back(Clause());
            clauses.back().watcher[0] = 0;
            clauses.back().watcher[1] = ((int)cls.size() >> 1);
            clauses.back().simpleCnt[0] = 0;
            clauses.back().simpleCnt[1] = 0;
            clauses.back().lit = move(cls);
        }
    }

    // 初始化兩個觀察者列表和正負文字索引列表
    initAllWatcherList();

    // 為所有單子句進行指派並執行單子句傳播（BCP）
    nowLevel = 0;
    if( unit.size() ) {
        for(auto lit : unit)
            unsatAfterInit |= !set(abs(lit), lit>0);
        if(!unsatAfterInit)
            simplifyClause();
    }
}


/// @brief 賦值 id=val@nowLevel 並遞迴地執行 BCP
/// @param id 要賦值的變數的 ID。
/// @param val 要賦值的值。
/// @param src 指示該賦值的來源。
/// @return 如果 BCP 成功完成且沒有衝突，則回傳 true；否則回傳 false。
bool solver::set(int id, bool val, int src) {

    if( solveDone ) return false;

    // 若變數已設定 ID，則檢查一致性
    if( var.getVal(id) != 2 )
    {
        conflictingClsID = -1;
        return var.getVal(id) == (int)val;
    }

    if( nowLevel == 0 ) src = -1;

    // 設定 id=val@nowLevel
    var.set(id, val, nowLevel, src);
    
    // 更新兩文字觀察
    bool ret = true;
    int &head = (val ? neg[id] : pos[id]);
    int idx = head;
    while( idx != -1 )
    {
        WatcherInfo &now = watchers[idx];
        int next = (now.next == head ? -1 : now.next);

        // 更新觀察者
        updateClauseWatcher(now);
        
        if (getVal(now) == 2 || eval(now))
        {
            // 情況1：觀察者達到一個待處理/滿足的文字
            // 將該觀察者推入對應的檢查列表
            if (getSign(now))
                swapListWatcher(watchers, head, pos[getVar(now)], idx);
            else
                swapListWatcher(watchers, head, neg[getVar(now)], idx);
        }
        else
        {
            // 觀察者遍歷所有子句返回原始子句
            // 無法找到下一個要觀察的文字

            // b 是該子句中的另一個觀察者
            WatcherInfo b(now.clsid, now.wid^1);

            // 情況2：觀察者的替代觀察者在該子句中
            if (getVal(b) == 2) 
            {
                //cout<<"c2"<<endl;
                if( !set(getVar(b), getSign(b), now.clsid) ) {

                    ret = false;
                    break;
                }
            }
            // 情況4：觀察者的替代觀察者不滿足該子句
            else if (!eval(b))
            {
                conflictingClsID = now.clsid;
                ret = false;
                break;
            }
            // 情況3：觀察者的替代觀察者已經滿足該子句，不需要進一步處理
        }
        idx = next;
    }
    // BCP 成功完成且沒有衝突
    return ret;

}


/// @brief 回溯函式，用於回退到指定層級。
/// @param lv 指定的回溯層級
void solver::backtrack(int lv)
{
    // 從變數堆疊中回溯到指定層級之上的變數
    for (int i=var._top; i >= 0 && var.stk[i].lv > lv; --i)
    {
        varPriQueue.restore(var.stk[i].var);
    }
    // 回退變數到指定層級
    var.backToLevel(lv);
}


// Conflicting.
// It will backtrack to the decision level
// where causing conflict clause become unit
// and set vid, sign, src indicating unit variable information.

/// @brief 從衝突中學習
/// @param vid 參考變數的 ID
/// @param sign 參考變數的正負號
/// @param src 衝突子句的 ID
/// @return 學習類型
int solver::learnFromConflict(int &vid, int &sign, int &src)
{
    vector<int> learnt = firstUIP();
    if( learnt.empty() )
        return LEARN_UNSAT;

    // 確定非時間順序回溯
    int backlv = 0;
    int towatch = -1;
    for (int i = (int)learnt.size()-2; i>=0; --i)
    {
        if( var.getLv(abs(learnt[i])) > backlv ) {
            backlv = var.getLv(abs(learnt[i]));
            towatch = i;
        }
    }

    // 學習一個賦值
    if( learnt.size() == 1 || backlv == 0 ) {
        backtrack(0);
        nowLevel = 0;
        int uip = learnt.back();
        if( !set(abs(uip), uip>0) )
            return LEARN_UNSAT;
        unit.emplace_back(uip);
        return LEARN_ASSIGNMENT;
    }

    // 新增衝突子句
    clauses.push_back(Clause());
    clauses.back().watcher[0] = towatch;           // Latest
    clauses.back().watcher[1] = (int)learnt.size() - 1; // Learnt
    clauses.back().lit = move(learnt);
    watchers.resize(watchers.size()+2);
    initWatcherList((int)clauses.size() - 1);

    backtrack(backlv);
    nowLevel = backlv;
    vid = clauses.back().getWatchVar(1);
    sign = clauses.back().getWatchSign(1);
    src = (int)clauses.size() - 1;

    return LEARN_CLAUSE;
}


/// @brief 初始化所有的觀察者列表。
void solver::initAllWatcherList()
{
    // 初始化觀察者列表和正負文字索引列表
    watchers = vector<WatcherInfo>(clauses.size()<<1);
    pos = vector<int>(maxVarIndex+4, -1);
    neg = vector<int>(maxVarIndex+4, -1);

    // 遍歷所有子句
    for(int cid=0; cid<clauses.size(); ++cid) {
        clauses[cid].watcher[0] = 0;
        clauses[cid].watcher[1] = (clauses[cid].size() >> 1);
        // 初始化該子句的觀察者列表
        initWatcherList(cid);
    }
}


/// @brief 初始化觀察者列表。
/// @param cid 要初始化觀察者列表的子句的索引。
void solver::initWatcherList(int cid)
{
    Clause &cls = clauses[cid];
    int id = cls.getWatchVar(0);
    int wid1 = (cid<<1);
    int wid2 = (cid<<1) + 1;

    // 初始化第一個觀察者
    watchers[wid1] = WatcherInfo(cid, 0);
    if( cls.getWatchSign(0) )
        appendListWatcher(watchers, pos[id], wid1);
    else
        appendListWatcher(watchers, neg[id], wid1);

    // 初始化第二個觀察者
    id = cls.getWatchVar(1);
    watchers[wid2] = WatcherInfo(cid, 1);
    if( cls.getWatchSign(1) )
        appendListWatcher(watchers, pos[id], wid2);
    else
        appendListWatcher(watchers, neg[id], wid2);
}


bool solver::solve()
{
    time(&startTime);
    if( unsatAfterInit ) return sat = false;
    // Preprocessing for given problem
    // Init database with all clause which has 2 or more literal in raw database
    // Eliminate all unit clause and check whether there is empty clause
    if(!preprocess())
        return sat = false;
    // Init heuristic
    initHeuristic();
    sat = _solve();
    return sat;
}


bool solver::_solve()
{
    while (true)
    {
        ++nowLevel;
        pii decision = pickUnassignedVar();
        if( decision.first == -1 )
            return true;
        time_t now;
        time(&now);
        runTime = (int)difftime(now, startTime);
        if(runTime>timeout) return false;
        int vid = decision.first;
        int sign = decision.second;
        int src = -1;
        
        while (!set(vid, sign, src))
        {
            //if( solveDone ) return false;
            //cout<<conflictingClsID<<endl;
            if( conflictingClsID == -1 )
                return false;

            int learnResult = learnFromConflict(vid, sign, src);
            //cout<<learnResult<<endl;
            if((learnResult == LEARN_UNSAT))
                return false;
            else if( learnResult == LEARN_ASSIGNMENT )
                break;
        }
    }
    return false;
}


/******************************************************
    Preprocessing
******************************************************/

/// @brief 預處理函式
/// @return 預處理結果
bool solver::preprocess() 
{
    if (!simplifyClause())
    {
        return false;
    }
    return true;
}


/// @brief 簡化子句。
/// @return 如果成功簡化子句，返回 true；否則返回 false。
bool solver::simplifyClause()
{
    int cid=0;
    while( cid < clauses.size() ) {
        bool satisfied = false;
        int lid = 0;
        while( lid < clauses[cid].size() ) {
            int vid = clauses[cid].getVar(lid);
            int sign = clauses[cid].getSign(lid);
            int now = var.getVal(vid);
            if( now==2 ) ++lid;
            else if( now==sign ) {
                satisfied = true;
                break;
            }
            else {
                swap(clauses[cid].lit[lid], clauses[cid].lit.back());
                clauses[cid].lit.pop_back();
            }
        }

        if( clauses.empty() ) return false;
        if( satisfied ) {
            clauses[cid] = clauses.back();
            clauses.pop_back();
        }
        else {
            clauses[cid].watcher[0] = 0;
            clauses[cid].watcher[1] = (clauses[cid].size() >> 1);
            ++cid;
        }
    }
    oriClsNum = (int)clauses.size();
    initAllWatcherList();
    return true;
}


/******************************************************
    Implementing Conflict Analysis and Learning Heuristic
******************************************************/

/// @brief 找到第一個單元子句 (1UIP)。
/// @return 包含第一個單元子句的變數的向量。如果找不到單元子句，則返回空向量。
vector<int> solver::firstUIP() 
{
    // VSIDS
    if((runMode & VSIDS))
    {
        varPriQueue.decayAll();
    }

    // 初始化
    litMarker.clear();
    nowLearnt.clear();
    int todoNum = _resolve(conflictingClsID, -1);
    if (todoNum == -1)
        return vector<int>();

    // 解析和找到 1UIP
    int top = var._top;
    while( todoNum > 1 )
    {
        while( litMarker.get(var.stk[top].var) == -1)
        {
            --top;
        }
        int nowNum = _resolve(var.stk[top].src, var.stk[top].var);
        if( nowNum == -1 )
        {
            return vector<int>();
        }
        todoNum += nowNum - 1;
        --top;
    }

    // 將 1UIP 放在向量的末尾
    while (litMarker.get(var.stk[top].var) == -1)
    {
        --top;
    }
    int uip = (var.stk[top].val > 0 ? -var.stk[top].var : var.stk[top].var);
    nowLearnt.push_back(uip);

    // 最小化
    minimizeLearntCls();

    return nowLearnt;
}


/// @brief 最小化學習子句。
void solver::minimizeLearntCls()
{
    // 最小化子句
    litMarker.clear();
    delMarker.clear();

    // 標記所有學習子句中的文字
    for(unsigned int i=0; i<nowLearnt.size(); ++i)
    {
        litMarker.set(abs(nowLearnt[i]), nowLearnt[i]>0);
    }

    int eliminateNum = 0;

    // 檢查除了 1UIP 以外的所有文字
    for (int i = (int)nowLearnt.size()-2; i>=0; --i)
    {
        int src = var.getSrc(abs(nowLearnt[i]));
        if( src == -1 )
            continue;
        bool selfSubsumed = true;

        // 檢查該子句中的每個文字
        for (int j = 0; j < clauses[src].size(); ++j)
        {
            int vid = clauses[src].getVar(j);
            int sign = clauses[src].getSign(j)>0;

            // 如果不是 1UIP 的文字且不是從 1UIP 推導出來的，則該文字無法被子句所取代
            if( abs(nowLearnt[i])!=vid && !isFromUIP(vid, sign) )
            {
                selfSubsumed = false;
                break;
            }
        }
        // 如果該文字被子句所取代，則將其標記為刪除
        if( selfSubsumed ) {
            ++eliminateNum;
            delMarker.set(i, 1);
        }
    }

    // 如果有文字被刪除，則重新組織學習子句
    if (eliminateNum)
    {
        int j = -1;
        for (unsigned int i=0; i<nowLearnt.size(); ++i)
        {
            if( delMarker.get(i) == -1 )
                nowLearnt[++j] = nowLearnt[i];
        }
        nowLearnt.resize(j+1);
    }
}


/// @brief 判斷文字是否來自於 1UIP。
/// @param vid 要判斷的變數的 ID。
/// @param sign 要判斷的文字的正負號。
/// @return 如果文字來自於 1UIP，則返回 true；否則返回 false。
bool solver::isFromUIP(int vid, int sign)
{
    // 如果文字已經被標記，則返回標記結果
    if( litMarker.get(vid) != -1 )
    {
        return litMarker.get(vid) == sign;
    }

    int src = var.getSrc(vid);

    // 如果找不到文字的來源，則標記為不是來自於1UIP
    if( src == -1 ) {
        litMarker.set(vid, 2);
        return false;
    }

    // 遞迴檢查該文字所在的子句中的每個文字
    for(int i=0; i<clauses[src].size(); ++i) {
        int nv = clauses[src].getVar(i);
        int ns = clauses[src].getSign(i);
        if( nv!=vid && isFromUIP(nv, ns) == false ) {
            litMarker.set(vid, 2);
            return false;
        }
    }

    // 標記文字為來自於1UIP
    litMarker.set(vid, sign);
    return true;
}


////////////////////////////////////////////////////////////
// Implementing Branching Heuristic
////////////////////////////////////////////////////////////

/// @brief 初始化啟發式資訊。
void solver::initHeuristic()
{
    // MOM 啟發式搜尋
    if ((runMode & MOM))
    {   // 遍歷所有子句
        for(auto &cls : clauses)
        {   // 若該子句大小 <= 門檻值，則將子句中的每個文字的初始優先級加 1
            if (cls.size()<=clauseSzThreshold)
            {
                for (int i=0; i<cls.size(); ++i)
                {
                    varPriQueue.increaseInitPri(cls.getVar(i), 1.0, cls.getSign(i));
                }
            }
        }
    }
    // JW Score 啟發式搜尋
    else if((runMode & JW))
    {   // 遍歷所有子句
        for(auto &cls : clauses)
        {   // 對於子句中的每個文字
            for(int i = 0; i < cls.size(); ++i)
            {   // 使用 Jeroslow-Wang Score 公式計算該變數的初始優先級
                varPriQueue.increaseInitPri(cls.getVar(i), pow(0.5, cls.size()), cls.getSign(i));
            }
        }
    }

    // 隨機選擇
    else
    {
        srand((unsigned int)time(NULL));
        double randPri = (double) rand() / (RAND_MAX + 1.0);
        // 將所有子句中的每個文字的初始優先級設定為隨機值
        for (int i=1;i<maxVarIndex+1;i++)
        {
            varPriQueue.increaseInitPri(i, randPri, 1);
        }
    }
    // 建立變數優先級堆積
    varPriQueue.heapify();
}


/// @brief 選擇未分配的變數。
/// @return 一個包含變數ID和正負號的 pair。如果無法找到未分配的變數，則返回 {-1, 0}。
pair<int,int> solver::pickUnassignedVar()
{
    // 尋找下一個決策變數
    while (true)
    {
        if (varPriQueue.size() == 0)
        {
            return {-1, 0};
        }
        int vid = varPriQueue.top();
        varPriQueue.pop();
        if (var.getVal(vid) == 2)
        {
            int sign = (varPriQueue.litBalance(vid)>0);
            return {vid, sign};
        }
    }
}


////////////////////////////////////////////////////////////
// Clause helper function
////////////////////////////////////////////////////////////

/// @brief 評估第 clsid 個子句的第 id 個文字。
/// @param clsid 子句識別號
/// @param id 文字識別號
/// @return 
inline bool solver::evalClauesLit(int clsid, int id) const
{
    return evalClauesLit(clauses[clsid], id);
}

/// @brief 評估子句 cls 中的第 id 個文字
/// @param cls 子句
/// @param id 文字識別號
/// @return 
inline bool solver::evalClauesLit(const Clause &cls, int id) const
{
    return var.getVal(cls.getVar(id)) == cls.getSign(id);
}

/// @brief 
/// @param info 觀察者資訊
/// @return 
inline bool solver::evalClauesWatchedLit(const WatcherInfo &info) const
{
    return evalClauesWatchedLit(info.clsid, info.wid);
}

inline bool solver::evalClauesWatchedLit(int clsid, int wid) const
{
    return evalClauesWatchedLit(clauses[clsid], wid);
}

inline bool solver::evalClauesWatchedLit(const Clause &cls, int wid) const
{
    return evalClauesLit(cls, cls.watcher[wid]);
}

inline int solver::updateClauseWatcher(const WatcherInfo &info)
{
    return updateClauseWatcher(info.clsid, info.wid);
}

inline int solver::updateClauseWatcher(int clsid, int wid)
{
    return updateClauseWatcher(clauses[clsid], wid);
}

/// @brief 更新子句 cls 的觀察者識別號
/// @param cls 子句物件
/// @param wid 觀察者識別號
/// @return 
inline int solver::updateClauseWatcher(Clause &cls, int wid)
{
    for (int counter = cls.size(); counter; --counter)
    {
        cls.watchNext(wid);
        if (!cls.watchSame() &&
            (var.getVal(cls.getWatchVar(wid)) == 2 || evalClauesWatchedLit(cls, wid)))
        {
            return cls.getWatchLit(wid);
        }

    }
    return cls.getWatchLit(wid);
}

////////////////////////////////////////////////////////////
// 2 Literal Watching helper function
////////////////////////////////////////////////////////////

/// @brief 獲得觀察者的文字值。
/// @param info 觀察者資訊
/// @return 回傳觀察者的文字值
inline int solver::getLit(const WatcherInfo &info) const
{
    return clauses[info.clsid].getWatchLit(info.wid);
}

/// @brief 獲得觀察者的變數 ID
/// @param info 觀察者資訊
/// @return 回傳觀察者的變數 ID
inline int solver::getVar(const WatcherInfo &info) const
{
    return clauses[info.clsid].getWatchVar(info.wid);
}

/// @brief 獲得觀察者的正負號
/// @param info 觀察者資訊
/// @return 回傳觀察者的正負號
inline int solver::getSign(const WatcherInfo &info) const
{
    return clauses[info.clsid].getWatchSign(info.wid);
}

/// @brief 獲得觀察者的變數值
/// @param info 觀察者資訊
/// @return 回傳觀察者的變數值
inline int solver::getVal(const WatcherInfo &info) const
{
    return var.getVal(getVar(info));
}

/// @brief 
/// @param info 
/// @return 
inline bool solver::eval(const WatcherInfo &info) const
{
    return evalClauesWatchedLit(clauses[info.clsid], info.wid);
}