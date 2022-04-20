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


//insgesamt: O(nm * max(n, m))
void Utils::gauss(std::vector<std::vector<int>>& matrix) {
    int h = 0;
    int k = 0;

    // m-Mal --> O(nm^2)
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
                        matrix[i][j] ^=  matrix[h][j];
                    }
                }
            }
            // O(1)
            h++;
            k++;
        }
    }

    // O(n*m)
    // Null-Zeilen entfernen
    matrix.erase(std::remove_if(matrix.begin(), matrix.end(), [](auto el){
        return std::count(el.begin(), el.end(), 1) == 0;
    }), matrix.end());


    // In reduzierte Spaltenform bringen
    // n-Mal --> O(nm)*n
    for(int row = matrix.size() - 1; row >= 0; row--){
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
