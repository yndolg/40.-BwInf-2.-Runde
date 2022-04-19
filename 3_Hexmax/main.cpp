#include <iostream>
#include <fstream>
#include "DPSolver.h"
#include "TSPSolver.h"
#include "State.h"

using namespace std;

string verwendung = "Verwendung: ./hexmax [-v <Algorithmus>] <Dateipfad> \r\n"
               " -v: Ausgabe der Zwischenstände nach allen Umlegungen\r\n"
               "     Algorithmen: \r\n"
               "       greedy: Greedy-Algorithmus, findet Umlegungen in O(n) \r\n"
               "       kurz  : Genetischer Algorithmus, findet Umlegungen, bei\r\n"
               "               denen Severin sich möglichst wenig bewegen muss.\r\n";

int main(int argc, char **argv) {

    // Einlesen der Argumente
    vector<string> args(argv + 1, argv + argc);
    if (args.empty() || args.size() > 4) {
        cout << verwendung;
        return 1;
    }
    int argpos = 0;
    string print_steps;
    if (args[argpos] == "-v") {
        argpos += 1;
        print_steps = args[argpos];
        if (print_steps != "greedy" && print_steps != "kurz") {
            cout << verwendung;
            return 1;
        }
        argpos += 1;
    }
    if (args.size() == argpos) {
        cout << verwendung;
        return 1;
    }
    ifstream ifs(args[argpos]);
    if (!ifs.good()) {
        cout << "Die Problemdefinition konnte nicht eingelesen werden.\r\n" << verwendung;
        return 1;
    }

    // Einlesen des Problems
    std::string s;
    ifs >> s;
    int c;
    ifs >> c;

    // Finden der maximalen Hex-Zahl
    DPSolver solver;
    auto r = solver.solve(s, c);

    // Ausgabe
    cout << "Die größtmögliche Hex-Zahl ist \n  " << r << ".\n";
    if (print_steps == "kurz") {
        TSPSolver tsp_solver;
        auto moves = tsp_solver.solve(s, r);
        cout << "Umlegungen, um zu dieser Hex-Zahl zu gelangen: \n";
        cout << State::visualisiereUmlegungen(moves, s);
        auto moves_greedy = DPSolver::getMoves(s, r);
        cout << "So muss sich Severin beim Umlegen " << TSPSolver::length(moves) << " Einheiten bewegen.\n";
        cout << "Mit dem Greedy-Algorithmus müsste er sich " << TSPSolver::length(moves_greedy) << " Einheiten bewegen.\n";
    } else if (print_steps == "greedy") {
        cout << "Umlegungen, um zu dieser Hex-Zahl zu gelangen: \n";
        auto moves = DPSolver::getMoves(s, r);
        cout << State::visualisiereUmlegungen(moves, s);
    }

    return 0;
}
