//
// Created by niels on 29.01.22.
//

#include <fstream>
#include <vector>
#include <iostream>
#include <chrono>
#include <omp.h>
#include <bitset>

#include "ResubstitutionSolver.h"
#include "Utils.h"

using namespace std;


int main() {
    Utils::Instance i = Utils::readInstanceFromFile("/media/data/schule/BWINF/40_2/4_Stapel/Eingabe/stapel2.txt");

    auto matrix = Utils::transpose(i.H);

    ResubstitutionSolver<256> s;
    auto solutions = s.solve(matrix, i.k);

    for (const auto &solution: solutions) {
        cout << "Solution:";
        for (const auto &v: solution) {
            cout << " " << v;
        }
        cout << "\n";
    }
}
