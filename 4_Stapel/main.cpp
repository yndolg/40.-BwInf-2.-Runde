//
// Created by niels on 29.01.22.
//

#include <fstream>
#include <utility>
#include <vector>
#include <iostream>
#include <numeric>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <chrono>
#include <omp.h>
#include <set>
#include <random>
#include <bitset>
#include <unordered_set>

#include "boost/math/special_functions/binomial.hpp"
#include <stack>

using namespace std;
int n, k, m;
int K;

/*
 * Ideen: - in standard-form umwandeln und nur die relevanten teile des Arrays speichern
 *        - Bitsets verwenden
 */
template <typename T>
vector<int> get_true_positions(const T& v){
    vector<int> r;
    for(int i = 0; i < v.size(); i++){
        if(v[i])
            r.push_back(i);
    }
    return r;
}

template<int size>
class MeetInTheMiddleSubstitution{
    vector<bitset<size>> words;

    bitset<size> current_word;
            public:
    MeetInTheMiddleSubstitution(const vector<vector<int>>& matrix){
        for(int i = 0; i < matrix.size(); i++){

        }
    }
    void solve(int next_var, int ones_left){

    }
};

template<int size>
class RecursiveSubstitution{
    chrono::steady_clock::time_point last_progress;

    static long long total_progress;
    double total;
    chrono::steady_clock::time_point starting_time;
    vector<int> row_of_var;
    vector<vector<int>> matrix;

    vector<bitset<size>> bit_matrix;

    unordered_set<bitset<size>> cache;

    vector<vector<int>> ones_in_row;
    vector<vector<int>> ones_of_var;
    vector<bitset<size>> results;
    int n_vars;

    vector<bitset<size>> base_words;
    int cache_hits;

    void resub(int var, std::bitset<size>& solution, int c, bitset<size> current_word) {

        if(cache.find(current_word) != cache.end()){
            cache_hits++;
            //cout << "Cache hit. Returning.\n";
            return;
        }

        while (row_of_var[var] >= 0 && c >= 0 && var >= 0 && c <= var + 1) {
            bool v = (bit_matrix[row_of_var[var]] & solution).count() % 2;
            solution[var] = v;
            if(v){
                current_word ^= base_words[var];
                cache.insert(current_word);
            }
            c -= v;
            var--;
        }

        if (c > var + 1) {
            cache.insert(current_word);
            return; //There is no hope left, we can stop trying.
        }

        if (c < 0) {
            cache.insert(current_word);
            return;
        }

        if (var < 0) {
            cout << "Found a solution:";
            for(const auto p: get_true_positions(solution)){
                cout << " " << p;
            }
            cout << "\n";
            results.push_back(solution);
            cache.insert(current_word);
            return;
        }
        // Variable has no row
        if (row_of_var[var] < 0) {
            thread_local static long long progress;
            progress += 1;
            if(progress % (1 << 20) == 0){
                cout << "Thread " << omp_get_thread_num() << ": " << progress << " Cache: "<<cache_hits<<"\n";
            }
            auto mysolution = solution;

            mysolution[var] = 0;
            resub(var - 1, mysolution, c, current_word);
            mysolution[var] = 1;
            resub(var - 1, mysolution, c - 1, current_word ^ base_words[var]);
            cache.insert(current_word);
            return;
        }
        cout << "This should never happen!\n";
    }

    vector<vector<int>> prepare(){
        //Re-Substitution
        n_vars = matrix[0].size() - 1;
        for(const auto& row: matrix){
            bitset<size> bitset;
            for(int i = 0; i < row.size(); i++)
                bitset[i] = row[i];
            bit_matrix.push_back(bitset);
        }

        bitset<size> solution(0);

        row_of_var = vector<int>(n_vars, -1);
        for (int i = 0; i < matrix.size(); i++) {
            int j = 0;
            while (matrix[i][j] != 1 && j < matrix[0].size()) {
                j++;
            }
            if (matrix[i][j] == 1) {
                row_of_var[j] = i;
            }
        }

        int undetermined = count(row_of_var.begin(), row_of_var.end(), -1);
        total = 1;// boost::math::binomial_coefficient<double>(undetermined, K);
        starting_time = chrono::steady_clock::now();
        cout << "Found " << undetermined << " undetermined variables, resulting in a search-space of " << total
             << " elements. \n";

        vector<vector<int>> r;

        //precalculating some stuff
        for (int row = 0; row < matrix.size(); row++) {
            vector<int> ones;
            int first_one = 0;
            while (matrix[row][first_one] != 1)
                first_one++;
            for (int i = first_one + 1; i < matrix[row].size(); i++) {
                if (matrix[row][i] == 1) {
                    ones.push_back(i);
                }
            }
            ones_in_row.push_back(ones);
        }

        ones_of_var = vector<vector<int>>(n_vars, vector<int>(0));
        for (int var = 0; var < n_vars; var++) {
            if (row_of_var[var] >= 0) {
                ones_of_var[var] = ones_in_row[row_of_var[var]];
            }
        }
        starting_time = chrono::steady_clock::now();

        for(int i = 0; i < matrix[0].size(); i++){
            bitset<size> word;
            word.reset();
            for(int j = 0; j < matrix.size(); j++){
                if(matrix[j][i])
                    word.set(j, matrix[j][i]);
            }
            base_words.push_back(word);
        }

        /*#pragma omp parallel default(none) shared(solution, K)
        #pragma omp single*/
        resub(n_vars - 1 , solution, K + 1, bitset<size>());


        cout << "Finished resubstitution after " << chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now()- starting_time).count()/1000.0 << " seconds.\n";
        for (const auto &s: results) {
            r.push_back(get_true_positions(s));
        }
        return r;
    }

public:
    vector<vector<int>> solve(vector<vector<int>> mat){
        matrix = std::move(mat);
        return prepare();

    }
};

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

    K = k;

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

    omp_set_num_threads(8);
    vector<vector<int>> result = gauss(matrix);
    matrix = gauss(matrix);
    cout << "The matrix after gaussian elimination: (with " << countOnesInMatrix(matrix) << " ones)\n";
    print_matrix(matrix);


    RecursiveSubstitution<256> s;
    auto solutions = s.solve(matrix);

    for (const auto &solution: solutions) {
        cout << "Solution:";
        for (const auto &v: solution) {
            cout << " " << v;
        }
        cout << "\n";
    }
}
