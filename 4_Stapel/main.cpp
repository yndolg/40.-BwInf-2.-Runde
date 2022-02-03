//
// Created by niels on 29.01.22.
//

#include <fstream>
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

#include "boost/math/special_functions/binomial.hpp"
#include <stack>

using namespace std;
int n, k, m;
int K;

thread_local static float progress = 0;
thread_local chrono::steady_clock::time_point last_progress;
static long long total_progress;
long long total;
chrono::steady_clock::time_point starting_time;
vector<int> row_of_var;
vector<vector<char>> matrix;
vector<vector<int>> ones_in_row;
vector<vector<int>> ones_of_var;

vector<int> order;
vector<vector<unsigned char>> results;
/*
// takes in the matrix and produces a permutation, that still puts the variables in a correct order, based on their
// dependencies, but hopefully in a way, that is faster to compute
// To be specific: it greedily always selects the variable that is still most dependet on, essentially like a
// topological sort
vector<int> calculateOptimalOrdering(){
    vector<int> result;
    set<int> variables_left;
    int n_vars = matrix[0].size() -1; // It was an extended coefficient matrix
    for(int i = 0; i < n_vars; i++)
        variables_left.insert(i);
    while(!variables_left.empty()){
        vector<int> depends_on(n_vars, 0);
        vector<int> depended_by(n_vars, 0);
        for(int var : variables_left){
            if(row_of_var[var] < 0)
                continue; // You don't depend on anything
            for(int i = 0; i < n_vars; i++){
                if(matrix[row_of_var[var]][i]){
                    // if the dependency has not yet been assigned
                    if(variables_left.find(i) != variables_left.end()){
                        // You can't depend on yourself
                        if(var != i){
                            depends_on[var] += 1;
                            depended_by[i] += 1;
                        }
                    }
                }
            }
        }

        int candidate = -1;
        int candidate_dependencies = -1;
        for(int var: variables_left){
            // If it is still depends on some other variable, don't assign it yet
            if(depends_on[var] == 0){
                if(depended_by[var] > candidate_dependencies){
                    candidate_dependencies = depended_by[var];
                    candidate = var;
                }
            }
        }
        if(candidate < 0){
            cout << "Error: the dependencies were cyclic. This should not have happened.\n";
        }
        result.push_back(candidate);
        variables_left.erase(candidate);
    }
    return result;
}*/

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
void resubIteratively(int var, vector<int> solution, int c) {
    struct stack_frame {
        int pos;
        int count;
        bool prev_value;
    };

    std::stack<struct stack_frame, vector<struct stack_frame>> m_stack; // var, c, pos
    m_stack.push({var, c,false});
    int progress = 0;
    while (!m_stack.empty()) {
        /*progress++;
        if(progress % 10000000 == 0){
            cout << "Progress: " << progress << "\n";
        }*/
        auto task = m_stack.top();
        m_stack.pop();

        if(task.pos < solution.size()-1)
            solution[order[task.pos+1]] = task.prev_value;

        if (task.pos < 0) {
            cout << "Solution: ";
            for (int i = 0; i < solution.size(); i++) {
                if(solution[i])
                    cout << " " << (int) i;
            }
            cout << "\n";
            continue; // This was a solution, a leaf node without any children
        }

        if (row_of_var[order[task.pos]] >= 0) {
            // this is a determined variable
            bool v = false;
            for (const auto pos: ones_of_var[order[task.pos]]) {
                v = v ^ solution[order[pos]];
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

void resub(int var, vector<unsigned char> &solution, int c) {
    while (row_of_var[var] >= 0 && c >= 0 && var >= 0 && c <= var + 1) {
        bool v = 0;
        for (const auto pos: ones_of_var[var]) {
            v = v ^ solution[pos];
        }

        solution[var] = v;


        c -= v;
        var--;
        if (var + 1 >= c && var + 1 >= 0 && c >= 0) {
            //progress += boost::math::binomial_coefficient<float>(var+1, c);
        }

    }
    if (c > var + 1) {
        return; //There is no hope left, we can stop trying. And the Binomial-Coefficient would have been 0 anyways
    }
    if (c < 0) {
        return;
    }

    if (var < 0) {
#pragma omp critical
        {
            cout << "Found a solution!\n";
            results.push_back(solution);
        }
        progress += 1;
        return;
    }


    // Variable has no row
    if (row_of_var[var] < 0) {
        /*auto elapsed_time = chrono::duration_cast<chrono::milliseconds>(
                chrono::steady_clock::now() - last_progress).count();
        if (elapsed_time > 1000 ) {
            last_progress = chrono::steady_clock::now();
#pragma omp critical
            {
                auto elapsed_time = chrono::duration_cast<chrono::seconds>(
                        chrono::steady_clock::now() - starting_time);
                cout << "Progress: " << progress << " in " << elapsed_time.count() << "s << (Thread "
                     << omp_get_thread_num() << ", " << (progress/boost::math::binomial_coefficient<double>(solution.size(), K+1))*100.0<<"%) \n";
            }
        }*/
        if (var > solution.size() - 10 && c > K + (1 /
                                                   2) /*boost::math::binomial_coefficient<double>(var+1, c) > 0.05*boost::math::binomial_coefficient<double>(solution.size(), K+1)*/ ) {
            cout << "Starting two tasks...\n";
#pragma omp task default(none) shared( var, c) firstprivate(solution)
            {
                solution[var] = 0;
                resub(var - 1, solution, c);
            }
#pragma omp task default(none) shared(var, c) firstprivate(solution)
            {

                solution[var] = 1;
                resub(var - 1, solution, c - 1);
            }
        } else {
            solution[var] = 0;
            resub(var - 1, solution, c);
            solution[var] = 1;
            resub(var - 1, solution, c - 1);
        }

        return;

    }
    cout << "This should never happen!\n";
}

vector<vector<char>> gauss(vector<vector<char>> matrix) {
    int h = 0;
    int k = 0;
    while (h < matrix.size() && k < matrix[0].size()) {
        int i_max = h;
        int min_ones = 1000000000;
        int min_row = -1;
        for(int i = h; i < matrix.size(); i++){
            if(matrix[i][k] != 0){
                int ones_in_row = std::count(matrix[i].begin(), matrix[i].end(), 1);
                if(ones_in_row < min_ones){
                    min_ones = ones_in_row;
                    min_row = i;
                }
            }
        }
        /*while (i_max < matrix.size() && matrix[i_max][k] == 0) {
            i_max += 1;
        }*/
        if (min_row < 0) {
            k++;
        } else {
            i_max = min_row;
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
    return matrix;
}

vector<vector<int>> solve(){
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
    total = pow(2, undetermined);
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
    resubIteratively(n_vars - 1, solution, K + 1);
    omp_set_nested(1);
    omp_set_num_threads(8);
    /*
#pragma omp parallel default(none) shared(n_vars, solution, K)
    {
#pragma omp single
        resub(n_vars - 1, solution, K + 1);
    }

*/    for (const auto &s: results) {
        vector<int> v;
        for (int i = 0; i < s.size(); i++) {
            if (s[i] == 1)
                v.push_back(i);
        }
        r.push_back(v);
    }
    return r;
}
int countOnesInMatrix(vector<vector<char>> matrix){
    int c = 0;
    for(const auto& row: matrix){
        for(const auto& v: row){
            c += v;
        }
    }
    return c;
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

    for (int bit = 0; bit < cards[0].size(); bit++) {
        vector<char> equation;
        equation.reserve(cards.size());
        for (auto &card: cards) {
            equation.push_back(card[bit]);
        }
        equation.push_back(0); // == 0 (mod 2)
        matrix.push_back(equation);
    }

    cout << "The input matrix had " << countOnesInMatrix(matrix) << " ones.\n";
    auto result = gauss(matrix);
    cout << "After running it through gaussian elimination, it had " << countOnesInMatrix(result) << " ones left.\n";
    int min = 1000000000;
    std::mt19937 rng;
    for(int i = 0; i < 10; i++){
        auto matrix_copy = matrix;
        std::shuffle(matrix_copy.begin(), matrix_copy.end(), rng);
        auto result = gauss(matrix_copy);
        min = std::min(min, countOnesInMatrix(result));
    }
    cout << "After running it through gaussian elimination for 10 times, it had " << min << " ones left. \n";
    matrix = gauss(matrix);
    auto solutions = solve();
    for (const auto &solution: solutions) {
        cout << "Solution:";
        for (const auto &v: solution) {
            cout << " " << v;
        }
        cout << "\n";
    }
}
