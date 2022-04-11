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
private:
    int getDP(int pos, int surplus);

    std::vector<int> memo;
    std::vector<int> parent;

    static std::pair<int, int> difference(std::bitset<7> c1, std::bitset<7> c2);

    std::string word;
    std::vector<std::bitset<7>> word_digits;

    int max_moves;

    std::string reconstruct(int max);

public:
    std::string solve(const std::string &string, int i);

    static std::string getMoves(std::string old, std::string next);

    void calcBottomUp();

    [[nodiscard]] int memoKey(int pos, int d) const;
};


#endif //INC_3_HEXMAX_DPSOLVER_H
