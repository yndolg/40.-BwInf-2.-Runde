//
// Created by niels on 11.04.22.
//

#include "BruteforceSolver.h"
#include <bitset>
#include <omp.h>
#include <chrono>
#include <iostream>
#include <vector>
#include "Utils.h"
#include <algorithm>
#include <functional>
#include <fstream>

using namespace std;

void Utils::efficientGauss(std::vector<boost::dynamic_bitset<>>& bit_mat){
    int h = 0;
    int k = 0;

    // m times --> O(nm^2)
    while (h < bit_mat.size() && k < bit_mat[0].size()) {
        int i_max = h;

        // O(n)
        while (i_max < bit_mat.size() && bit_mat[i_max][k] == 0) {
            i_max += 1;
        }
        if (i_max == bit_mat.size()) { // O(1)
            k++;
        } else { // O(nm)
            // O(m)
            swap(bit_mat[h], bit_mat[i_max]);
            // n-times --> O(nm)
            for (int i = h + 1; i < bit_mat.size(); i++) {
                if(bit_mat[i][k]){
                    bit_mat[i] = bit_mat[i] ^ bit_mat[h];
                }
            }
            // O(1)
            h++;
            k++;
        }
    }


    // O(n*m)
    // remove all zero-lines
    bit_mat.erase(std::remove_if(bit_mat.begin(), bit_mat.end(), [](auto el){
        return el.count() == 0;
    }), bit_mat.end());


    // bring into reduced echelon form
    // n times --> O(nm)*n
    for(int row = bit_mat.size() - 1; row >= 0; row--){

        // this should work, as every line has a one (because all the others were removed before)
        auto leading_one = bit_mat[row].find_first();
        // diese Reihe von allen darüberliegenden Reihen entfernen, wenn diese eine 1 an der entsprechenden Stelle haben
        for(int i = 0; i < row; i++){ // n times --> O(nm)
            if(bit_mat[i][leading_one]){
                bit_mat[i] = bit_mat[i] ^ bit_mat[row];
            }
        }
    }
}
//total: O(nm * max(n, m))
void Utils::gauss(std::vector<std::vector<int>>& matrix) {
    int h = 0;
    int k = 0;

    // m times --> O(nm^2)
    while (h < matrix.size() && k < matrix[0].size()) {
        int i_max = h;

        // O(n)
        while (i_max < matrix.size() && matrix[i_max][k] == 0) {
            i_max += 1;
        }
        if (i_max == matrix.size()) { // O(1)
            k++;
        } else { // O(nm)
            // O(m)
            swap(matrix[h], matrix[i_max]);
            // n-times --> O(nm)
            for (int i = h + 1; i < matrix.size(); i++) {
                if(matrix[i][k]){
                    for (int j = k; j < matrix[0].size(); j++) {
                        matrix[i][j] = matrix[i][j] ^ matrix[h][j];
                    }
                }
            }
            // O(1)
            h++;
            k++;
        }
    }

    // O(n*m)
    // remove all zero-lines
    matrix.erase(std::remove_if(matrix.begin(), matrix.end(), [](auto el){
        return std::count(el.begin(), el.end(), 1) == 0;
    }), matrix.end());


    // bring into reduced echelon form
    // n times --> O(nm)*n
    for(int row = matrix.size() - 1; row >= 0; row--){
        // this should work, as every line has a one (because all the others were removed before)
        // O(m)
        auto leading_one = std::distance(matrix[row].begin(), std::find(matrix[row].begin(), matrix[row].end(), 1));
        // diese Reihe von allen darüberliegenden Reihen entfernen, wenn diese eine 1 an der entsprechenden Stelle haben
        for(int i = 0; i < row; i++){ // n times --> O(nm)
            if(matrix[i][leading_one]){
                std::transform(matrix[i].begin(), matrix[i].end(), matrix[row].begin(), matrix[i].begin(), std::bit_xor<>());
            }
        }
    }
}

 Utils::Instance Utils::readInstanceFromFile(std::ifstream& ifs) {
    int n, k, m;

    ifs >> n;
    ifs >> k;
    ifs >> m;

    vector<vector<int>> cards;
    for (int i = 0; i < n; i++) {
        vector<int> card;
        for (int j = 0; j < m; j++) {
            char s;
            ifs >> s;
            card.push_back(s == '1');
        }
        cards.push_back(card);
    }
    return Instance{cards, k };
}

std::vector<std::vector<int>> Utils::transpose(std::vector<std::vector<int>> mat) {
    vector<vector<int>> mat_transpose;
    for (int bit = 0; bit < mat[0].size(); bit++) {
        vector<int> equation;
        equation.reserve(mat.size());
        for (auto &card: mat) {
            equation.push_back(card[bit]);
        }
        mat_transpose.push_back(equation);
    }
    return mat_transpose;
}

void Utils::print_matrix(const vector<vector<int>> &matrix) {
    for (const auto &row: matrix) {
        for (const auto el: row) {
            cout << el << " ";
        }
        cout << "\n";
    }
}