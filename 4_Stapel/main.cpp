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

#include "boost/math/special_functions/binomial.hpp"
#include <stack>

using namespace std;
int n, k, m;
int K;


// takes a square (not augmented!) matrix in, that has previously been reduced by Gauss elimination
void resubIteratively2(vector<vector<int>> matrix){
    struct stack_frame{
        vector<int> determined_vars;
    };
    stack<struct stack_frame, vector<struct stack_frame>> m_stack;
    while(!m_stack.empty()){
        auto task = m_stack.top();
        m_stack.pop();

    }
}
void resubIteratively(int var, vector<int> solution, int c, vector<int> row_of_var, vector<vector<int>> ones_of_var) {
    struct stack_frame {
        int pos;
        int count;
        bool prev_value;
    };

    std::stack<struct stack_frame, vector<struct stack_frame>> m_stack; // var, c, pos
    m_stack.push({var, c,false});
    int progress = 0;
    while (!m_stack.empty()) {
        auto task = m_stack.top();
        m_stack.pop();

        if(task.pos < solution.size()-1)
            solution[task.pos+1] = task.prev_value;

        if (task.pos < 0) {
            cout << "Solution: ";
            for (int i = 0; i < solution.size(); i++) {
                if(solution[i])
                    cout << " " << (int) i;
            }
            cout << "\n";
            continue; // This was a solution, a leaf node without any children
        }

        if (row_of_var[task.pos] >= 0) {
            // this is a determined variable
            bool v = false;
            for (const auto pos: ones_of_var[task.pos]) {
                v = v ^ solution[pos];
            }
            if (task.count - v >= 0) {
                m_stack.push({task.pos - 1,task.count - v, v});
            }
        } else {
            // this is a free variable
            for (bool i: {true, false}) {
                if (task.count - i >= 0) {
                    m_stack.push({task.pos - 1, task.count - i, i});
                }
            }
        }
    }
}
vector<int> get_true_positions(const vector<int>& v){
    vector<int> r;
    for(int i = 0; i < v.size(); i++){
        if(v[i])
            r.push_back(i);
    }
    return r;
}

class RecursiveSubstitution{
    long long progress = 0;
    chrono::steady_clock::time_point last_progress;

    static long long total_progress;
    double total;
    chrono::steady_clock::time_point starting_time;
    vector<int> row_of_var;
    vector<vector<int>> matrix;
    vector<vector<int>> ones_in_row;
    vector<vector<int>> ones_of_var;
    vector<vector<int>> results;

    void resub_using_bitsets(int var, std::bitset<256> current_solution, int c){

    }

    void resub(int var, vector<int>& solution, int c) {
        while (row_of_var[var] >= 0 && c >= 0 && var >= 0 && c <= var + 1) {
            bool v = false;
            for (const auto pos: ones_of_var[var]) {
                v = v ^ solution[pos];
            }
            solution[var] = v;

            c -= v;
            var--;
        }
        if (c > var + 1) {
            return; //There is no hope left, we can stop trying.
        }
        if (c < 0) {
            return;
        }

        if (var < 0) {
            cout << "Found a solution:";
            for(const auto p: get_true_positions(solution)){
                cout << " " << p;
            }
            cout << "\n";
            results.push_back(solution);
            return;
        }
        // Variable has no row
        if (row_of_var[var] < 0) {
            progress += 1;
            if(progress % (1 << 24) == 0){
                cout << progress << "\n";
            }
            solution[var] = 0;
            resub(var - 1, solution, c);
            solution[var] = 1;
            resub(var - 1, solution, c - 1);
            solution[var] = 0;
            return;
        }
        cout << "This should never happen!\n";
    }

    vector<vector<int>> prepare(){
        //Re-Substitution
        int n_vars = matrix[0].size() - 1;
        vector<int> solution(n_vars, 0);

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
        total = boost::math::binomial_coefficient<double>(undetermined, K);
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
        resub(n_vars - 1, solution, K + 1);

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

    ifstream ifs("/media/data/schule/BWINF/40_2/4_Stapel/Eingabe/stapel4.txt");

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


    vector<vector<int>> result = gauss(matrix);
    matrix = gauss(matrix);
    cout << "The matrix after gaussian elimination: (with " << countOnesInMatrix(matrix) << " ones)\n";
    print_matrix(matrix);


    RecursiveSubstitution s;
    auto solutions = s.solve(matrix);

    for (const auto &solution: solutions) {
        cout << "Solution:";
        for (const auto &v: solution) {
            cout << " " << v;
        }
        cout << "\n";
    }
}
