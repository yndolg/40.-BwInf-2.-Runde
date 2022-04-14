//
// Created by niels on 29.01.22.
//


#include <iostream>
#include <fstream>
#include <iomanip>

#include "BruteforceSolver.h"
#include "Utils.h"
#include "ISDSolver.h"

using namespace std;

string usage = "Benutzung: ./stapel [Algorithmus] [Dateipfad] \r\n"
               "  Algorithmen:\r\n"
               "   'isd': information-set-decoding\r\n"
               "   'bruteforce': Bruteforce-Algorithmus\r\n";

int main(int argc, char **argv) {

    if (argc != 3) {
        cout << usage;
        return 1;
    }


    ifstream ifs(argv[2]);
    if(!ifs.good()){
        cout << "Die Problemdefinition konnte nicht eingelesen werden.\r\n" << usage;
        ifs = ifstream("/media/data/schule/BWINF/40_2/4_Stapel/Eingabe/stapel4.txt");
    }

    Utils::Instance orig_instance = Utils::readInstanceFromFile(ifs);
    Utils::Instance instance = orig_instance;
    instance.H = Utils::transpose(instance.H);



    vector<vector<int>> solutions;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    if (string("isd") == argv[1]) {
        cout << "Lösungsverfahren: Information-Set-Decoding\n";
        for(int i = 0; i < 1000; i++){
            ISDSolver s;
            solutions = s.solve(instance);
        }
    } else if (string("bruteforce") == argv[1]) {
        cout << "Lösungsverfahren: Brute-Force\n";
        // Auswählen, welche größte die Bitsets bei der Berechnung haben, da die Größe zur Compile-Zeit
        // festgelegt sein muss.
        if(instance.H[0].size() < 256){
            BruteforceSolver<256> s(instance);
            s.prepare();
            solutions = s.solve();
        } else if(instance.H[0].size() < 1024){
            BruteforceSolver<1024> s(instance);
            s.prepare();
            solutions = s.solve();
        } else if(instance.H[0].size() < 8192){
            BruteforceSolver<8192> s(instance);
            s.prepare();
            solutions = s.solve();
        } else{
            cout << "Der Brute-Force-Ansatz kann nur Probleme mit < 8192 Variablen lösen.\n";
            return 1;
        }

    } else {
        cout << usage;
        return 1;
    }



    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    if(!solutions.empty()){
        cout << "Die richtigen Karten wurden gefunden:\r\n";
        for (const auto &solution: solutions) {
            for (const auto &v: solution) {
                cout << " Karte " << std::setw(3) << v << " (";
                for(int bit: orig_instance.H[v]){
                    cout << bit;
                }
                cout << ")\n";
            }
            cout << "\n";
        }
    }else{
        cout << "Es gibt keine Lösung.\n";
    }
    std::cout << "Das Lösen hat "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << " ms gedauert." << std::endl;
}
