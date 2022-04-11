//
// Created by niels on 29.01.22.
//

#include <fstream>
#include <utility>
#include <vector>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <omp.h>
#include <set>
#include <bitset>
#include <unordered_set>

#include "Resubstitution.h"
#include "MeetInTheMiddle.h"


using namespace std;
int n, k, m;

/*
 * Ideen: - in standard-form umwandeln und nur die relevanten teile des Arrays speichern
 *        - Bitsets verwenden
 */

vector<vector<int>> gauss(vector<vector<int>> matrix) {
    int h = 0;
    int k = 0;
    while (h < matrix.size() && k < matrix[0].size()) {
        int i_max = h;
        while (i_max < matrix.size() && matrix[i_max][k] == 0) {
            i_max += 1;
        }
        if (i_max == matrix.size()) {
            k++;
        } else {
            swap(matrix[h], matrix[i_max]);
            for (int i = h + 1; i < matrix.size(); i++) {
                int f = matrix[i][k] / matrix[h][k];
                for (int j = k; j < matrix[0].size(); j++) {
                    matrix[i][j] = (matrix[i][j] + matrix[h][j] * f) % 2; // XOR
                }
            }
            h++;
            k++;
        }
    }

    // remove all zero-lines
    matrix.erase(std::remove_if(matrix.begin(), matrix.end(), [](auto el){
        return std::count(el.begin(), el.end(), 1) == 0;
    }), matrix.end());


    // bring into reduced echelon form
    for(int row = matrix.size() - 1; row >= 0; row--){
        // this should work, as every line has a one (because all the others were removed before)
        auto leading_one = std::distance(matrix[row].begin(), std::find(matrix[row].begin(), matrix[row].end(), 1));
        // diese Reihe von allen darüberliegenden Reihen entfernen, wenn diese eine 1 an der entsprechenden Stelle haben
        for(int i = 0; i < row; i++){
            if(matrix[i][leading_one]){
                // Row_i = Row_i ^ Row_row
                std::transform(matrix[i].begin(), matrix[i].end(), matrix[row].begin(), matrix[i].begin(), std::bit_xor<>());
            }
        }
    }
    return matrix;
}


int countOnesInMatrix(vector<vector<int>> matrix){
    int c = 0;
    for(const auto& row: matrix){
        for(const auto& v: row){
            c += v;
        }
    }
    return c;
}
void print_matrix(const vector<vector<int>>& matrix){
    for(const auto& row: matrix){
        for(const auto el: row){
            cout << el << " ";
        }
        cout << "\n";
    }
}

int main() {

    ifstream ifs("/media/data/schule/BWINF/40_2/4_Stapel/Eingabe/stapel3.txt");

    ifs >> n;
    ifs >> k;
    ifs >> m;

    vector<vector<uint8_t>> cards;
    for (int i = 0; i < n; i++) {
        vector<uint8_t> card;
        for (int j = 0; j < m; j++) {
            char s;
            ifs >> s;
            card.push_back(s == '1');
        }
        cards.push_back(card);
    }

    vector<vector<int>> matrix;
    for (int bit = 0; bit < cards[0].size(); bit++) {
        vector<int> equation;
        equation.reserve(cards.size());
        for (auto &card: cards) {
            equation.push_back(card[bit]);
        }
        equation.push_back(0); // == 0 (mod 2)
        matrix.push_back(equation);
    }

    cout << "Running meet-in-the-middle:\n";
    //MeetInTheMiddle<129> mitm;
    //mitm.solve(matrix, k+1);


    omp_set_num_threads(8);
    vector<vector<int>> result = gauss(matrix);
    matrix = gauss(matrix);
    cout << "The matrix after gaussian elimination: (with " << countOnesInMatrix(matrix) << " ones)\n";
    print_matrix(matrix);


    Resubstitution<256> s;
    auto solutions = s.solve(matrix, k);

    for (const auto &solution: solutions) {
        cout << "Solution:";
        for (const auto &v: solution) {
            cout << " " << v;
        }
        cout << "\n";
    }
}
