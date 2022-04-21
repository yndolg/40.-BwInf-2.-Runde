//
// Created by niels on 11.04.22.
//

#include <algorithm>
#include <iostream>
#include "ISDSolver.h"

using namespace std;


/*
 * Effizientere Variante des Gauss-Verfahrens, die Bitsets verwendet.
 * Im ISD-Algorithmus wird die Laufzeit vom Gauss-Algorithmus dominiert,
 * sodass die Optimierungen lohnenswert sind.
 */
void ISDSolver::efficientGauss(std::vector<boost::dynamic_bitset<>> &bit_mat) { // Theta(n^2m)
    int h = 0;
    int k = 0;
    while (h < bit_mat.size() && k < bit_mat[0].size()) {
        int i_max = h;
        while (i_max < bit_mat.size() && bit_mat[i_max][k] == 0) {
            i_max += 1;
        }
        if (i_max == bit_mat.size()) {
            k++;
        } else {
            // XOR-Swap-Algorithmus, std::swap wäre durch die Speicherverwaltung der
            // temporären Variable ein Flaschenhals
            if (h != i_max) {
                bit_mat[h] ^= bit_mat[i_max];
                bit_mat[i_max] ^= bit_mat[h];
                bit_mat[h] ^= bit_mat[i_max];
            }
            for (int i = h + 1; i < bit_mat.size(); i++) {
                if (bit_mat[i][k]) {
                    bit_mat[i] ^= bit_mat[h];
                }
            }
            h++;
            k++;
        }
    }

    // Resubstitution
    for (int row = bit_mat.size() - 1; row >= 0; row--) {
        auto leading_one = bit_mat[row].find_first();
        // diese Zeile von allen darüberliegenden Zeilen entfernen, wenn diese
        // eine 1 an der entsprechenden Stelle haben
        for (int i = 0; i < row; i++) {
            if (bit_mat[i][leading_one]) {
                bit_mat[i] ^= bit_mat[row];
            }
        }
    }
}

std::vector<std::vector<int>> ISDSolver::solve(Utils::Instance instance) {

    // Einmal Gauss-Jordan-Algorithmus anpassen. Dadurch werden linear abhängige
    // Zeilen aus der Matrix entfernt, sodass sich die Anzahl der Zeilen nochmal
    // ändert.
    Utils::gauss(instance.H);

    auto n_cols = instance.H[0].size();
    auto n_rows = instance.H.size();

    int t = instance.k + 1;

    // In allen Threads geteilt, sodass die anderen Threads abbrechen können,
    // wenn ein Thread eine Lösung gefunden hat.
    vector<int> return_value(0);

#pragma omp parallel default(none) firstprivate(rng) shared(return_value, n_cols, n_rows, instance, t, versuche)
    {
        vector<boost::dynamic_bitset<>> H_perm;
        for (int row = 0; row < n_rows; row++) {
            H_perm.emplace_back(n_cols, 0);
        }
        boost::dynamic_bitset<> pivots(n_cols, 0);
        vector<int> m_i;
        m_i.reserve(n_rows);
        vector<int> k_i;
        k_i.reserve(n_rows);

        while (return_value.empty()) {
            #pragma omp critical
            versuche += 1;

            // die Spalten von H permutieren und in H_perm abspeichern
            auto permutation = zufaelligePermutation(n_cols);
            for (int row = 0; row < n_rows; row++) {
                for (int col = 0; col < n_cols; col++) {
                    H_perm[row][col] = instance.H[row][permutation[col]];
                }
            }

            // H_perm in reduzierte Spaltenform bringen
            efficientGauss(H_perm);

            // Pivots finden, um leicht das Inverse der Teilmatrix zu bilden
            m_i.clear();
            k_i.clear();
            pivots.clear();
            for (int row = 0; row < n_rows; row++) {
                auto col = H_perm[row].find_first();
                pivots.set(col);
                m_i.push_back(col);
                k_i.push_back(row);
            }

            // p wird auf 1 festgelegt. Folglich kann einfach über die Spalten iteriert werden.
            // Dadurch wird über alle p-großen Teilmengen iteriert
            int p = 1;
            for (int s = 0; s < n_cols; s++) {
                // Nur über die Spalten iterieren, die nicht in der invertierbaren Submatrix sind
                if (pivots.test(s))
                    continue;

                vector<int> syndrom(n_rows, 0);

                for (int j = 0; j < n_rows; j++)
                    syndrom[j] = H_perm[j][s];

                // Hamming-Gewicht bestimmen
                int w = 0;
                for (int bit: syndrom) 
                    w += bit;
                
                if (w == t - p) {
                    vector<int> solution(n_cols, 0);

                    // Die ausgewählte Spalte wird verwendet
                    solution[s] = 1;

                    // Berechnen, welche Spalten aus der invertierbaren Submatrix
                    // ebenfalls verwendet werden müssen
                    for (int i = 0; i < n_rows; i++) {
                        if (syndrom[k_i[i]] == 1) {
                            solution[m_i[i]] = 1;
                        }
                    }

                    // Permutation rückgängig machen
                    vector<int> solution_perm(n_cols);
                    for (int i = 0; i < n_cols; i++) {
                        solution_perm[permutation[i]] = solution[i];
                    }

                    return_value = Utils::get_true_positions(solution_perm);
                }
            }

        }
    }
    return {return_value};
}

ISDSolver::ISDSolver() {
    std::random_device device;
    rng = mt19937(device());
}

vector<int> ISDSolver::zufaelligePermutation(int n) {
    vector<int> permutation(n);
    for (int i = 0; i < n; i++) {
        permutation[i] = i;
    }

    shuffle(permutation.begin(), permutation.end(), rng);
    return permutation;
}
