//
// Created by niels on 11.04.22.
//

#ifndef STAPEL_ISDSOLVER_H
#define STAPEL_ISDSOLVER_H

#include <vector>
#include <random>
#include "Utils.h"

class ISDSolver {
public:
    ISDSolver();
    int attempts = 0;
    std::vector<std::vector<int>> solve(Utils::Instance i);

private:

    std::vector<int> getRandomPermutation(int n);
    std::mt19937 rng;
};


#endif //STAPEL_ISDSOLVER_H
