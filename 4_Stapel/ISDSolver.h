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

    int versuche = 0;

    std::vector<std::vector<int>> solve(Utils::Instance i);

private:

    std::vector<int> zufaelligePermutation(int n);

    static void efficientGauss(std::vector<boost::dynamic_bitset<>> &bit_mat);

    std::mt19937 rng;
};


#endif //STAPEL_ISDSOLVER_H
