//
// Created by niels on 28.02.22.
//

#ifndef STAPEL_RESUBSTITUTION_H
#define STAPEL_RESUBSTITUTION_H

#include <chrono>
#include <vector>
#include <bitset>
#include <unordered_set>
#include <iostream>
#include <omp.h>
#include <algorithm>

using namespace std;

template<int size>
class Resubstitution {
    std::chrono::steady_clock::time_point last_progress;

    static long long total_progress;
    double total;
    std::chrono::steady_clock::time_point starting_time;
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

    int K;

    template <typename T>
    vector<int> get_true_positions(const T& v){
        vector<int> r;
        for(int i = 0; i < v.size(); i++){
            if(v[i])
                r.push_back(i);
        }
        return r;
    }

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
    vector<vector<int>> solve(vector<vector<int>> mat, int number_of_cards){
        matrix = std::move(mat);
        K = number_of_cards;
        return prepare();
    }
};


#endif //STAPEL_RESUBSTITUTION_H
