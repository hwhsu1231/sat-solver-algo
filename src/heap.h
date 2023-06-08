#ifndef __HEAP_H
#define __HEAP_H

#include <vector>
#include <algorithm>
#include <cstdlib>
#include <cmath>
using namespace std;


// Heap maintaining priority order of all variables
// Priority must be positive floating point
class VarHeap
{
public:
    void init(int maxVarIndex);
    inline void increaseInitPri(int var, double pri, int sign);
    inline void heapify();
    inline int size();
    inline int top();
    inline void pop();
    inline int litBalance(int var);
    inline void restore(int var);
    inline void increasePri(int var, double pri, int sign);
    inline void decayAll();
    constexpr static const double decayFactor = 0.9;

protected:
    struct HeapEntry
    {
        double pri = 0;       ///< 變數的優先級
        int var = 0;          ///< 變數的索引
        int lastEvalT = 0;    ///< 上次評估時間
        HeapEntry() : pri(0), var(0), lastEvalT(0) {};
        HeapEntry(double pri, int var) : pri(pri), var(var), lastEvalT(0) {}
    };
    int nowT = 0;                   ///< 當前時間

    // Array as heap
    vector<HeapEntry> arr;          ///< 儲存變數的陣列
    vector<int> signCnt;            ///< 儲存正負號計數的陣列
    int sz;                         ///< 堆積的大小
    int fullSz;                     ///< 堆積的最大容量

    // Mapping var to index in arr
    vector<int> mapping;
    inline void swapEntry(int a, int b);

    // Function maintaining heap property
    void upward(int id);
    void downward(int id);

    // Helper function
    inline double getPri(int id);
    inline void addPri(int id, double pri);

};

/// @brief 增加初始優先級的變數
/// @param var 要增加優先級的變數
/// @param pri 增加的優先級值
/// @param sign 符號（正負號），預設為 -1
inline void VarHeap::increaseInitPri(int var, double pri, int sign=-1)
{
    arr[mapping[var]].pri += pri;
    if( sign!=-1 )
        signCnt[var] += (sign ? 1 : -1);
}

/// @brief 獲取堆積的大小
/// @return 堆積的大小
inline int VarHeap::size()
{
    return sz;
}

/// @brief 獲取堆積頂部的變數
/// @return 堆積頂部的變數
inline int VarHeap::top()
{
    return arr[1].var;
}

/// @brief 對所有變數進行衰減
inline void VarHeap::decayAll()
{
    ++nowT;
}

/// @brief 交換兩個堆元素的位置
/// @param aid 第一個元素的索引
/// @param bid 第二個元素的索引
inline void VarHeap::swapEntry(int aid, int bid)
{
    mapping[arr[aid].var] = bid;
    mapping[arr[bid].var] = aid;
    swap(arr[aid], arr[bid]);
}

/// @brief 
inline void VarHeap::heapify()
{
    while (sz < fullSz)
    {
        upward(++sz);
    }
}

/// @brief 
inline void VarHeap::pop()
{
    swapEntry(1, sz--);
    downward(1);
}

/// @brief 獲取變數的符號平衡值
/// @param var 要獲取符號平衡值的變數
/// @return 變數的符號平衡值
inline int VarHeap::litBalance(int var)
{
    return signCnt[var];
}

/// @brief 重新調整變數的位置，使其符合堆的性質
/// @param var 要重新調整位置的變數
inline void VarHeap::restore(int var)
{
    int id = mapping[var];
    if( id <= sz )
        return;
    ++sz;
    if( var != arr[sz].var )
        swapEntry(id, sz);
    upward(sz);
}


/// @brief 增加變數的優先級
/// @param var 要增加優先級的變數
/// @param pri 增加的優先級值
/// @param sign 符號（正負號），預設為 -1
inline void VarHeap::increasePri(int var, double pri, int sign=-1)
{
    int id = mapping[var];
    addPri(id, pri);
    if( sign!=-1 )
        signCnt[id] += (sign ? 1 : -1);
    if( id <= sz )
        upward(id);
}

/// @brief 獲取堆積元素的優先級
/// @param id 堆積元素的索引
/// @return 堆積元素的優先級
inline double VarHeap::getPri(int id)
{
    if (arr[id].lastEvalT != nowT)
    {
        arr[id].pri *= pow(decayFactor, nowT - arr[id].lastEvalT);
        arr[id].lastEvalT = nowT;
    }
    return arr[id].pri;
}

/// @brief 增加堆積元素的優先級
/// @param id 堆積元素的索引
/// @param pri 增加的優先級值
inline void VarHeap::addPri(int id, double pri)
{
    arr[id].pri = getPri(id) + pri;
}


#endif
