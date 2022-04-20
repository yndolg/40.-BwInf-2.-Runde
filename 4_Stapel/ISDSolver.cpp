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
void ISDSolver::efficientGauss(std::vector<boost::dynamic_bitset<>>& bit_mat){ // Theta(n^2m)
    int h = 0;
    int k = 0;
    while (h < bit_mat.size() && k < bit_mat[0].size()) {       // O(n*m*n), Omega(min(n,m)*m*n)
        int i_max = h;
        while (i_max < bit_mat.size() && bit_mat[i_max][k] == 0) {
            i_max += 1;
        }
        if (i_max == bit_mat.size()) {
            k++;
        } else {
            // XOR-Swap-Algorithmus, std::swap hat einen großen Overhead durch die temporäre Variable
            if(h != i_max){                                      // O(n)
                bit_mat[h] ^= bit_mat[i_max];
                bit_mat[i_max] ^= bit_mat[h];
                bit_mat[h] ^= bit_mat[i_max];
            }
            for (int i = h + 1; i < bit_mat.size(); i++) {       // O(nm)
                if(bit_mat[i][k]){
                    bit_mat[i] = bit_mat[i] ^ bit_mat[h];        // O(n)
                }
            }
            h++;
            k++;
        }
    }

    // Resubstitution
    for(int row = bit_mat.size() - 1; row >= 0; row--){       // Theta(n^2m)
        auto leading_one = bit_mat[row].find_first();         // O(n)
        // diese Reihe von allen darüberliegenden Reihen entfernen, wenn diese eine 1 an der entsprechenden Stelle haben
        for(int i = 0; i < row; i++){                         // Theta(nm)
            if(bit_mat[i][leading_one]){
                bit_mat[i] ^= bit_mat[row];                   // Theta(n)
            }
        }
    }
}
std::vector<std::vector<int>> ISDSolver::solve(Utils::Instance instance) {

    Utils::gauss(instance.H);

    auto n_cols = instance.H[0].size();
    auto n_rows = instance.H.size();

    int t = instance.k + 1;

    vector<int> return_value(0);
    #pragma omp parallel default(none) firstprivate(rng) shared(return_value, n_cols, n_rows, instance, t, attempts)
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
            attempts += 1;

            // die Spalten von H permutieren und in H_perm abspeichern
            auto permutation = getRandomPermutation(n_cols);
            for (int row = 0; row < n_rows; row++) {
                for (int col = 0; col < n_cols; col++) {
                    H_perm[row][col] = instance.H[row][permutation[col]];
                }
            }

            // H_perm in reduzierte Spaltenform bringen
            efficientGauss(H_perm);

            // find pivots
            m_i.clear();
            k_i.clear();
            pivots.clear();
            for (int row = 0; row < n_rows; row++) {
                auto col = H_perm[row].find_first();
                pivots.set(col);
                m_i.push_back(col);
                k_i.push_back(row);
            }

           /* vector<int> not_m_i;
            for (int i = 0; i < n_cols; i++) {
                if (std::find(m_i.begin(), m_i.end(), i) == m_i.end())
                    not_m_i.push_back(i);
            }*/


            // calculate all p-bit subsets of not_m_i, or less, if not_m_i.size() < 2
            int p = 1;
            for (int p2 = 0; p2 < n_cols; p2 ++) {
                if(pivots.test(p2))
                    continue;
                vector<int> syndrome(n_rows, 0);

                for (int j = 0; j < n_rows; j++)
                    syndrome[j] = H_perm[j][p2];

                // if column p has weight of exactly t - 1 => match
                int w = 0;
                for (int bit: syndrome) {
                    w += bit;
                }

                if (w == t - p) {
                    vector<int> solution(n_cols, 0);

                    // solution has bits in p set and the columns of the pivots in the rows of the syndrome (p)
                    solution[p2] = 1;

                    for (int i = 0; i < n_rows; i++) {
                        if (syndrome[k_i[i]] == 1) {
                            solution[m_i[i]] = 1;
                        }
                    }

                    // unpermute the solution
                    vector<int> solution_perm(n_cols);
                    for (int i = 0; i < n_cols; i++) {
                        solution_perm[permutation[i]] = solution[i];
                    }

                    /* cout << "Found a solution:";
                     for (const auto x: Utils::get_true_positions(solution_perm)) {
                         cout << " " << x;
                     }
                     cout << "\n";*/
                    return_value = Utils::get_true_positions(solution_perm);
                }
            }

        }
    }
    return {return_value};
}

ISDSolver::ISDSolver() {
    std::random_device device;
    rng = mt19937(10);
}

vector<int> ISDSolver::getRandomPermutation(int n) {
    vector<int> permutation(n);
    for (int i = 0; i < n; i++) {
        permutation[i] = i;
    }

    shuffle(permutation.begin(), permutation.end(), rng);
    return permutation;
}
