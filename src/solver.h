#ifndef __SOLVER_H
#define __SOLVER_H

#include "parser.h"
#include "clause.h"
#include "opstack.h"
#include "heap.h"
#include <cmath>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <iostream>
#include <ctime>
#include <chrono>
using namespace std;


extern int solveDone;

/// @brief 暫存表
struct Temptable
{
public:
    /// @brief // 暫存表
    vector<int> table;

    /// @brief 初始化暫存表
    /// @param sz 暫存表大小
    inline void init(int sz)
    {
        table = vector<int>(sz+4);
        timestamp = vector<int>(sz+4, -1);
        nowT = 0;
    }

    /// @brief 清空暫存表
    inline void clear()
    {
        ++nowT;
    }

    /// @brief 獲取變數的值
    /// @param id 變數的ID
    /// @return 變數的值，如果尚未設置則返回-1
    inline int get(int id)
    {
        // Return -1 if unset yet
        int is_val = (timestamp[id] == nowT);
        return table[id] * is_val - !is_val;
    }

    /// @brief 設置變數的值
    /// @param id 變數的ID
    /// @param val 變數的值
    inline void set(int id, unsigned int val)
    {
        table[id] = val;
        timestamp[id] = nowT;
    }

protected:
    vector<int> timestamp;  // 記錄變數的時間戳記
    int nowT = 0;           // 目前的時間戳記
};

/// @brief 求解器
class solver
{
public:
    /// @brief 觀察者的相關資訊
    struct WatcherInfo
    {
        int clsid;        // 子句的 ID
        int wid;          // 觀察者的 ID
        int prev = -1;    // 前一個觀察者的索引
        int next = -1;    // 下一個觀察者的索引
        WatcherInfo() {};
        WatcherInfo(int clsidt, int widt)
        {
            clsid = clsidt;
            wid = widt;
        }
    };

    typedef pair<int,int> pii;
    enum {
        INFINITE = 1023456789,
        LEARN_UNSAT = 1,
        LEARN_ASSIGNMENT = 2,
        LEARN_CLAUSE = 3,
        VSIDS = 4,
        MOM = 8,
        JW = 16
        // JW = 16,
        // DFS = 32
    };

    void init(const char *filename, int mode);
    bool solve();
    vector<int> result();
    void printCNF();
    int size();

    int getVarCount();       // 獲取變數數量
    int getClauseCount();    // 獲取子句數量

protected:
    int unsatAfterInit = 0;       // 初始化後不可滿足的子句數量
    int sat = 1;                  // SAT 結果（0：UNSAT，1：SAT）
    int maxVarIndex;              // 最大變數索引
    int oriClsNum;                // 初始子句數量
    vector<Clause> clauses;       // 子句集合
    vector<int> unit;             // 單位子句
    opStack var;                  // 變數堆疊
    int nowLevel = 0;             // 目前的層級

    // bool dfs(int var, bool sign);

    ////////////////////////////////////////////////////////////
    // Helper function for DPLL
    ////////////////////////////////////////////////////////////

    bool set(int var, bool val, int src=-1);
    void backtrack(int lv);
    int learnFromConflict(int &vid, int &sign, int &src);
    void initAllWatcherList();
    void initWatcherList(int cid);


    bool _solve();
    inline int _resolve(int clsid, int x);

    ////////////////////////////////////////////////////////////
    // Preprocess
    ////////////////////////////////////////////////////////////

    bool preprocess();
    bool simplifyClause();

    ////////////////////////////////////////////////////////////
    // Clause helper function
    ////////////////////////////////////////////////////////////

    inline bool evalClauesLit(int clsid, int id) const;
    inline bool evalClauesLit(const Clause &cls, int id) const;
    inline bool evalClauesWatchedLit(const WatcherInfo &info) const;
    inline bool evalClauesWatchedLit(int clsid, int wid) const;
    inline bool evalClauesWatchedLit(const Clause &cls, int wid) const;
    inline int updateClauseWatcher(const WatcherInfo &info);
    inline int updateClauseWatcher(int clsid, int wid);
    inline int updateClauseWatcher(Clause &cls, int wid);

    ////////////////////////////////////////////////////////////
    // 2 Literal Watching helper function
    ////////////////////////////////////////////////////////////

    inline int getLit(const WatcherInfo &info) const;
    inline int getVar(const WatcherInfo &info) const;
    inline int getSign(const WatcherInfo &info) const;
    inline int getVal(const WatcherInfo &info) const;
    inline bool eval(const WatcherInfo &info) const;

    // Conflict Clause Learning Heuristic
    vector<int> nowLearnt;

    vector<int> firstUIP();
    void minimizeLearntCls();
    bool isFromUIP(int vid, int sign);



    void initHeuristic();
    pii pickUnassignedVar();
    // int pickBalancedPhase(int vid);

public:
    static const int  clauseSzThreshold = 10;
    int               runMode;      // 執行模式
    int               timeout;      // 超時時間
    time_t            startTime;    // 開始時間
    int               runTime;      // 執行時間

protected:
    int conflictingClsID = -1;
    Temptable litMarker;
    Temptable delMarker;

    // 2 Literal Watching
    vector<WatcherInfo> watchers;   // 觀察者集合
    vector<int> pos;
    vector<int> neg;

    // Branching Heuristic
    VarHeap varPriQueue;
    vector<int> phaseRecord;
};





/// @brief 解析衝突子句以獲取解析數量 (Resolve helper)
/// @param clsid 衝突子句的 ID
/// @param x 排除的變數 ID
/// @return 解析的數量
inline int solver::_resolve(int clsid, int x)
{
    int ret = 0;
    for (int i=0; i<clauses[clsid].size(); ++i)
    {
        int lit = clauses[clsid].getLit(i);
        int vid = clauses[clsid].getVar(i);
        int sign = clauses[clsid].getSign(i);
        if( vid == x || litMarker.get(vid) == sign ) continue;
        if( litMarker.get(vid) != -1 ) return -1;
        litMarker.set(vid, sign);
        varPriQueue.increasePri(vid, 1.0-VarHeap::decayFactor, sign);
        if (var.getLv(vid) == nowLevel)
        {
            ++ret;
        }
        else
        {
            nowLearnt.push_back(lit);
        }
    }
    return ret;
}


#endif
