#include <cstdio>
#include <ctime>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <thread>
#include <iostream>
#include <chrono>
#include <mutex>
#include <filesystem>
#include "solver.h"
using namespace std;
namespace fs = std::filesystem;


int solveDone = 0;
vector<int> result;

vector<string> conditionNameList;
vector<int> conditionEnumList;     // 測試條件
vector<string> benchmarkNameList;
vector<string> classNameList;
vector<vector<float>> classCondMeanList;


int main(int argc, const char *argv[])
{
    srand((unsigned int)time(NULL));


    /* 
        VSIDS         = 4
        MOM           = 8
        JW            = 16

        BCP                     = 0
        BCP + VSIDS             = 0+4     = 4
        BCP + VSIDS + MOM       = 0+4+8   = 12
        BCP + VSIDS + JW        = 0+4+16  = 20
     */

    conditionNameList.push_back("BCP              ");
    conditionNameList.push_back("BCP + VSIDS      ");
    conditionNameList.push_back("BCP + VSIDS + MOM");
    conditionNameList.push_back("BCP + VSIDS + JW ");

    conditionEnumList.push_back(0);         // BCP
    conditionEnumList.push_back(4);         // BCP + VSIDS
    conditionEnumList.push_back(12);        // BCP + VSIDS + MOM
    conditionEnumList.push_back(29);        // BCP + VSIDS + JW


    fs::path maindir("benchmark");
    // fs::path maindir("benchmark2");

    // 遍歷 benchmark 目錄中的子測資類別
    for (auto dirit1 : fs::directory_iterator(maindir)) 
    {   
        string className = dirit1.path().filename().u8string();
        std::cout << "Class: " << className << std::endl;
        
        vector<float> condResultSum(4, 0);
        vector<int> condResultCount(4, 0);
        vector<float> condMeanList(4, 0);

        // 遍歷類別中的測資
        for (const auto &dirit2 : fs::directory_iterator(dirit1.path()))
        {   
            std::cout << "---- [" << dirit2.path().filename().u8string() << "] ";
            // std::cout << "---- [" << dirit2.path().filename().u8string() << "]" << std::endl;

            solver yasat_tmp;
            yasat_tmp.init(dirit2.path().u8string().c_str(), conditionEnumList[0]);
            std::cout << "Variables: " << yasat_tmp.getVarCount() << ", Clauses: " << yasat_tmp.getClauseCount() << std::endl;

            // 遍歷所有情況
            for (size_t it = 0; it < conditionEnumList.size(); it++)
            {
                solver yasat;
                auto startTime = std::chrono::high_resolution_clock::now();           // mark start time
                yasat.init(dirit2.path().u8string().c_str(), conditionEnumList[it]);  // 初始化 cnf 檔
                yasat.timeout = 10;
                yasat.solve();
                result = yasat.result();
                auto endTime = std::chrono::high_resolution_clock::now();             // mark end time
                float totalTime = 
                    std::chrono::duration<float, std::milli> (endTime - startTime).count();

                std::cout << "-------- " << conditionNameList[it] << " : ";
                if (result[0] == -1) {
                    cout << "TIMEOUT" << endl;
                }
                else if (result[0]) {
                    cout << "SATISFIABLE,   Time = " << totalTime << "ms" << endl;
                    condResultSum[it] += totalTime;
                    condResultCount[it]++;
                }
                else {
                    cout << "UNSATISFIABLE, Time = " << totalTime << "ms" << endl;
                    condResultSum[it] += totalTime;
                    condResultCount[it]++;
                }
            }
        }

        classNameList.push_back(className);

        for (size_t it = 0; it < conditionEnumList.size(); it++) {
            if (condResultCount[it] == 0) {
                condMeanList[it] = -1;
            } else {
                condMeanList[it] = condResultSum[it] / (float)condResultCount[it];
            }
        }
        classCondMeanList.push_back(condMeanList);
    }

    return 0;
}
