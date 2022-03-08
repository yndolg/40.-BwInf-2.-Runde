//
// Created by niels on 06.03.22.
//

#ifndef INC_3_HEXMAX_SOLVER_H
#define INC_3_HEXMAX_SOLVER_H


#include <string>
#include <array>
#include <vector>
#include <bitset>
#include <unordered_set>
#include "State.h"


class Solver {
private:
    static std::unordered_set<State, StateHash> getNextStates(const State& s);

public:
    std::string solve(std::string basicString, int i);
};


#endif //INC_3_HEXMAX_SOLVER_H
