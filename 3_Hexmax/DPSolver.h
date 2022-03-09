//
// Created by niels on 08.03.22.
//

#ifndef INC_3_HEXMAX_DPSOLVER_H
#define INC_3_HEXMAX_DPSOLVER_H

#include <string>
#include <bitset>
#include <unordered_map>

#include <unordered_set>

#include "State.h"

class DPSolver {
    struct memo_key{
        int pos;
        int d;

        bool operator==(const memo_key &rhs) const;

        bool operator!=(const memo_key &rhs) const;
    };


private:
    int calc(int pos, int surplus);
    int calcMemo(int pos, int surplus);

    std::vector<int> memo;


    static std::pair<int, int> difference(std::bitset<7> c1, std::bitset<7> c2);
    int cacheHit = 0;
    int cacheMiss = 0;

    std::string word;
    int max_moves;

    std::string reconstruct(int max);
    int memoKey(memo_key m) const;

public:
    std::string solve(const std::string& string, int i);

    std::string getMoves(std::string old, std::string next);

};


#endif //INC_3_HEXMAX_DPSOLVER_H
