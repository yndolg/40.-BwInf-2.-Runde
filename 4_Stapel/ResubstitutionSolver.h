//
// Created by niels on 28.02.22.
//

#ifndef STAPEL_RESUBSTITUTIONSOLVER_H
#define STAPEL_RESUBSTITUTIONSOLVER_H

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
class ResubstitutionSolver {

    vector<int> row_of_var;
    vector<vector<int>> matrix;

    vector<bitset<size>> row_bitsets;
    vector<bitset<size>> cols_bitsets;


    vector<vector<int>> ones_in_row;
    vector<vector<int>> ones_of_col;

    vector<bitset<size>> results;
    int n_cols;

    int K;

    /*
     * Re-substitutes recursively all columns < col.
     */
    void resub(int col, bitset<size> &codeword, int c, bitset<size> syndrome) {

        // Estimate progress
        if (col > 160) {
            double total = pow(2, n_cols);
            double done;
            for (int i = n_cols; i >= 0; i--) {
                if (codeword[i]) {
                    done += pow(2, i - 1);
                }
            }
            cout << done / total << "\n";
        }


        // Set the positions can be deduced directly
        // row_of_var[col] >= 0: this variable has a pivot
        while (col >= 0 && row_of_var[col] >= 0 && c >= 0 && c <= col + 1) {
            // This column can be computed from the previous values: the sum off all the activated columns in the
            // row has become zero in the end, if it does not, we'll have to add this. This works, because it
            // is always the last column that can be activated to satisfy a certain row
            bool v = (row_bitsets[row_of_var[col]] & codeword).count() % 2;

            // If the column has been activated, we'll have to add it to the syndrome
            if (v) {
                codeword[col] = 1;
                syndrome ^= cols_bitsets[col];
                --c;
            }
            col--;
        }


        // Abort, if there are not enough columns left to reach the target weight
        if (c > col + 1) {
            return;
        }

        // Abort, if we're over the maximum weight
        if (c < 0) {
            return;
        }


        // Substitution has reached the last column
        if (col < 0) {
            cout << "Found a solution:";
            for (const auto p: Utils::get_true_positions(codeword)) {
                cout << " " << p;
            }
            cout << "\n";
            results.push_back(codeword);
            return;
        }

        // Variable is free
        if (row_of_var[col] < 0) {
            auto mysolution = codeword;
            mysolution[col] = 0;
            resub(col - 1, mysolution, c, syndrome);

            auto mysolution2 = codeword;
            mysolution2[col] = 1;
            resub(col - 1, mysolution2, c - 1, syndrome ^ cols_bitsets[col]);
            return;
        }

        // This should never happen. Each column is either free or non-free
        cout << "An error occurred";
    }

public:
    vector<vector<int>> solve() {

        auto starting_time = chrono::steady_clock::now();
        bitset<size> solution(0);

        resub(n_cols - 1, solution, K + 1, bitset<size>());


        cout << "Finished resubstitution after "
             << chrono::duration<double>(chrono::steady_clock::now() - starting_time).count() << " seconds.\n";
        vector<vector<int>> r;
        for (const auto &s: results) {
            r.push_back(get_true_positions(s));
        }
        return r;
    }

    explicit ResubstitutionSolver(Utils::Instance instance) {

        matrix = std::move(instance.H);
        K = instance.k;
        n_cols = matrix[0].size();
    }

    void prepare() {
        matrix = Utils::gauss(matrix);

        // precompute which row  has the pivot of a given column
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

        //precompute the positions off all ones of a row, excluding the pivot elements
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

        // Precompute the positions of all ones in a column
        ones_of_col = vector<vector<int>>(n_cols, vector<int>(0));
        for (int col = 0; col < n_cols; col++) {
            if (row_of_var[col] >= 0) {
                ones_of_col[col] = ones_in_row[row_of_var[col]];
            }
        }

        // precompute bitsets of the columns
        for (int i = 0; i < n_cols; i++) {
            bitset<size> word;
            word.reset();
            for (int j = 0; j < matrix.size(); j++) {
                if (matrix[j][i])
                    word.set(j, matrix[j][i]);
            }
            cols_bitsets.push_back(word);
        }

        // precompute bitsets of the rows, for more efficient computations
        for (const auto &row: matrix) {
            bitset<size> bitset;
            for (int i = 0; i < row.size(); i++)
                bitset[i] = row[i];
            row_bitsets.push_back(bitset);
        }

        // TODO: Compute how many variables are free and estimate running time
    }
};


#endif //STAPEL_RESUBSTITUTIONSOLVER_H
