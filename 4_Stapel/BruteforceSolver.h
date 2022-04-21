//
// Created by niels on 28.02.22.
//

#ifndef STAPEL_BRUTEFORCESOLVER_H
#define STAPEL_BRUTEFORCESOLVER_H

#include <chrono>
#include <vector>
#include <bitset>
#include <unordered_set>
#include <iostream>
#include <omp.h>
#include <algorithm>
#include <cmath>
#include "Utils.h"

using namespace std;

template<int size>
class BruteforceSolver {

    vector<int> row_of_var;
    vector<vector<int>> matrix;

    vector<bitset<size>> row_bitsets;
    vector<bitset<size>> cols_bitsets;

    chrono::time_point<chrono::steady_clock> fortschritt_zeit = chrono::steady_clock::now();

    vector<vector<int>> ones_in_row;
    vector<vector<int>> ones_of_col;

    vector<bitset<size>> results;
    int n_cols;

    int K;

    /*
     * Alle Variablen < col werden rekursiv resubstituiert
     */
    void resub(int col, bitset<size> codeword, int c, bitset<size> syndrome) {

        // Variablen setzten, die resubstituiert werden können
        // row_of_var[col] >= 0: Diese Variable hat ein Pivotelement im Gauss-Verfahren
        //                       gehabt und kann resubstituiert werden
        while (col >= 0 && row_of_var[col] >= 0 && c >= 0 && c <= col + 1) {

            // Der Wert dieser Spalte wird aus den bereits substituierten Spalten berechnet, die Summe
            // der bisher substituierten Variablen, skalar multipliziert mit der Zeile, in der das
            // Pivotelement dieser Variable ist.
            bool v = (row_bitsets[row_of_var[col]] & codeword).count() % 2;

            // Wenn die Variable auf 1 gesetzt wurde, muss die Spalte zum aktuellen Syndrom hinzugefügt werden
            if (v) {
                codeword[col] = 1;
                syndrome ^= cols_bitsets[col];
                --c;
            }
            col--;
        }

        // Abbrechen, wenn es nicht mehr genug Spalten gibt, um das Zielgewicht zu erreichen
        if (c > col + 1) {
            printProgress(codeword);
            return;
        }

        // Abbrechen, wenn das Zielgewicht bereits überschritten wurde
        if (c < 0) {
            printProgress(codeword);
            return;
        }

        // Die Substitution hat die letzte Variable erreicht
        if (col < 0) {
            results.push_back(codeword);
            return;
        }

        // Abbrechen, wenn bereits eine Lösung gefunden wurde
        if (!results.empty()) {
            return;
        }

        // Die Variable 'col' ist frei -> rekursiv versuchen, 0 oder 1 zu substituieren
        if (row_of_var[col] < 0) {
            codeword[col] = 0;
            resub(col - 1, codeword, c, syndrome);

            codeword[col] = 1;
            resub(col - 1, codeword, c - 1, syndrome ^ cols_bitsets[col]);
            return;
        }

        cout << "Ein Fehler ist aufgetreten.\n";
    }

    void printProgress(bitset<size> codeword) {
        // maximal einmal pro Sekunde ausgeben
        if (chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - fortschritt_zeit).count() >=
            3000) {
            fortschritt_zeit = chrono::steady_clock::now();

            // Der Fortschritt wird aus dem aktuellen Codewort berechnet.
            // Das ist möglich, da die Rekursion immer zuerst 0 und dann 1
            // ausprobiert.
            double progress = 0;
            int count = 0;
            for (int i = n_cols - 1; i >= 0; i--) {
                if (row_of_var[i] < 0) {
                    count += 1;
                    if (codeword[i] == 1) {
                        progress += pow < double > (2.0, -count);
                    }
                }
            }
            cout << "Fortschritt: " << (progress * 100.0) << "% \n";
        }
    }

public:
    vector<vector<int>> solve() {

        auto starting_time = chrono::steady_clock::now();
        bitset<size> solution(0);

        resub(n_cols - 1, solution, K + 1, bitset<size>());

        vector<vector<int>> r;
        for (const auto &s: results) {
            r.push_back(Utils::get_true_positions(s));
        }
        return r;
    }

    explicit BruteforceSolver(Utils::Instance instance) {
        matrix = std::move(instance.H);
        K = instance.k;
        n_cols = matrix[0].size();
    }

    void vorberechnen() {

        // die Matrix in reduzierte Zeilenstufenform bringen
        Utils::gauss(matrix);

        // vorberechnen, welche Zeile das Pivotelement einer Spalte hat
        row_of_var = vector<int>(n_cols, -1);
        for (int i = 0; i < matrix.size(); i++) {
            int j = 0;
            while (matrix[i][j] != 1 && j < matrix[0].size()) {
                j++;
            }
            if (matrix[i][j] == 1) {
                row_of_var[j] = i;
            }
        }

        // Positionen der Einsen einer Zeile vorberechnen, ohne das Pivotelement
        for (auto &row: matrix) {
            vector<int> ones;
            bool first = true;
            for (int i = 0; i < row.size(); i++) {
                if (row[i] == 1) {
                    if (first) {
                        first = false;
                    } else {
                        ones.push_back(i);
                    }
                }
            }
            ones_in_row.push_back(ones);
        }

        // Positionen der Einsen in einer Spalte vorberechnen
        ones_of_col = vector<vector<int>>(n_cols, vector<int>(0));
        for (int col = 0; col < n_cols; col++) {
            if (row_of_var[col] >= 0) {
                ones_of_col[col] = ones_in_row[row_of_var[col]];
            }
        }

        // Spalten in Bitsets umwandeln, um effizient das Syndrom berechnen zu können
        for (int i = 0; i < n_cols; i++) {
            bitset<size> word;
            word.reset();
            for (int j = 0; j < matrix.size(); j++) {
                if (matrix[j][i])
                    word.set(j, matrix[j][i]);
            }
            cols_bitsets.push_back(word);
        }

        // Zeilen in Bitsets umwandeln, um effizienter rechnen zu können
        for (const auto &row: matrix) {
            bitset<size> bitset;
            for (int i = 0; i < row.size(); i++)
                bitset[i] = row[i];
            row_bitsets.push_back(bitset);
        }
    }
};

#endif //STAPEL_BRUTEFORCESOLVER_H
