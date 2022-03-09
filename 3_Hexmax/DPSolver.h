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

    struct memo_key_hash{
        std::size_t operator()(const memo_key& k) const{
            size_t hash = 5381;
            hash = ((hash << 5) + hash) + k.pos;
            hash = ((hash << 5) + hash) + k.d;
            return hash;
        }
    };

private:

    std::string solution;
    int calc(int pos, int surplus);
    int calcMemo(int pos, int surplus);

    std::unordered_map<memo_key, int,memo_key_hash> memo;


    static std::pair<int, int> difference(std::bitset<7> c1, std::bitset<7> c2);
    int cacheHit = 0;
    int cacheMiss = 0;

    std::string word;


public:
    std::string solve(const std::string& string, int i);

    std::string getMoves(std::string old, std::string next);

    std::string reconstruct(int max);
};


#endif //INC_3_HEXMAX_DPSOLVER_H
