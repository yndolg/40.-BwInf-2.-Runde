//
// Created by niels on 29.01.22.
//


#include <iostream>

#include "ResubstitutionSolver.h"
#include "Utils.h"
#include "ISDSolver.h"

using namespace std;


int main() {
    Utils::Instance instance = Utils::readInstanceFromFile("/media/data/schule/BWINF/40_2/4_Stapel/Eingabe/stapel5.txt");

    instance.H = Utils::transpose(instance.H);

    /*ResubstitutionSolver<256> s(i);
    s.prepare();
    auto solutions = s.solve();*/
    vector<vector<int>> solutions;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    int attempts = 0;

    for(int i = 0; i < 1000; i++){
        ISDSolver s;
        solutions = s.solve(instance);
        attempts += s.attempts;

    }

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();


    for (const auto &solution: solutions) {
        cout << "Solution:";
        for (const auto &v: solution) {
            cout << " " << v;
        }
        cout << "\n";
    }
    std::cout << "Execution took on average " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()/1000 << " µs" << std::endl;
    std::cout << "Average attemts: " << attempts / 1000.0 << endl;
}
