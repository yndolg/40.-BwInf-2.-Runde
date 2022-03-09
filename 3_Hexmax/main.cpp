#include <iostream>
#include <fstream>
#include <random>
#include "Solver.h"
#include "DPSolver.h"

using namespace std;
int main() {
    std::ifstream ifs("../Eingabe/hexmax5.txt");
    std::string s;
    ifs >> s;
    int c;
    ifs >> c;
    DPSolver solver;

    auto r = solver.solve(s, c);

    cout << "Solution: " << r << "\n";
    cout << "Moves: \n";
    cout << solver.getMoves(s, r);
    return 0;
}
