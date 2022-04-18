//
// Created by niels on 11.04.22.
//

#include <algorithm>
#include <iostream>
#include "ISDSolver.h"

#include <omp.h>

using namespace std;

std::vector<std::vector<int>> ISDSolver::solve(Utils::Instance instance) {

    Utils::gauss(instance.H);

    auto n_cols = instance.H[0].size();
    auto n_rows = instance.H.size(); // the number of rows might get reduced

    int t = instance.k + 1;

    vector<int> return_value(0);
    omp_set_num_threads(8);
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
            Utils::efficientGauss(H_perm);

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
