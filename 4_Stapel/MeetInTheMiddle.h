//
// Created by niels on 28.02.22.
//

#ifndef STAPEL_MEETINTHEMIDDLE_H
#define STAPEL_MEETINTHEMIDDLE_H

#include <vector>
#include <bitset>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <algorithm>

#include "boost/math/special_functions/binomial.hpp"

using namespace std;
template<int size>
class MeetInTheMiddle {
private:
    void calc_all_possibilities(unordered_map<bitset<size>, vector<int>>& solution, const vector<bitset<size>>& bitsets, vector<int>& names,  int pos, int n, bitset<size> val){
        if(n == 0){
            solution.insert(make_pair(val, names));
            return;
        }
        if(pos == bitsets.size()){
            return;
        }
        calc_all_possibilities(
                solution,
                bitsets,
                names,
                pos+1,
                n,
                val
                );
        names.push_back(pos);
        calc_all_possibilities(
                solution,
                bitsets,
                names,
                pos +1,
                n-1,
                val ^ bitsets[pos]
                );
        names.pop_back();
    }


    vector<bitset<size>> to_bitsets(vector<vector<int>> mat){
        vector<bitset<size>> bitsets;

        for(int i = 0; i < mat[0].size(); i++){
            bitset<size> bitset;

            for(int j = 0; j < mat.size(); j++)
                bitset[j] = mat[j][i];
            bitsets.push_back(bitset);
        }
        return bitsets;
    }

    int cut;
    int count = 0;
    long long total;
    void search(const unordered_map<bitset<size>, vector<int>>& other_side, const vector<bitset<size>> basis, vector<int>& names, int pos, bitset<size> val, int n){
        if(n == 0){
            count++;
            if(count % 1000000 == 0){
                cout << " " << 100.0*count/(double) total << "%\n";
            }
            auto x = other_side.find(val);
            if(x != other_side.end()){
                vector<int> solution = names;
                solution.insert(solution.begin(), x->second.begin(), x->second.end());
                sort(solution.begin(), solution.end());
                cout << "Match: ";
                for(auto u: solution){
                    cout << u << " ";
                }
                cout << endl;
            }
            return;
        }
        if(pos == basis.size()){
            return;
        }

        search(other_side, basis, names, pos+1, val , n);
        names.push_back(pos+cut);
        search(other_side, basis, names, pos+1, val ^ basis[pos], n - 1);
        names.pop_back();
    }

public:
    void solve(vector<vector<int>> mat, int number_of_cards){
        auto bitsets = to_bitsets(mat);

        cut = bitsets.size()/2;

        vector<bitset<size>> b1 = vector<bitset<size>>(bitsets.begin(), bitsets.begin()+cut);
        vector<bitset<size>> b2 = vector<bitset<size>>(bitsets.begin()+cut, bitsets.end());

        for(int split = 0; split <= number_of_cards; split++){
            int a = split;
            int b = number_of_cards-split;
            cout << "For a = " << a << " and b = " << b << ":\n";
            cout << " constructing map of " << boost::math::binomial_coefficient<double>(b1.size(), a) << " elements\n";
            unordered_map<bitset<size>,vector<int>> words1;
            vector<int> names;
            calc_all_possibilities(words1, b1,names, 0, a, bitset<size>());
            words1.rehash(words1.size()*3);
            count = 0;
            names.clear();
            total = boost::math::binomial_coefficient<double>(b1.size(), b);
            cout << " searching through " << total << " elements\n";
            search(words1, b2, names,0, bitset<size>(), b);
            cout << "Found " << words1.size() << " words in B1 and searched for them in the " << count << " words of b2\n";
        }
    }

};


#endif //STAPEL_MEETINTHEMIDDLE_H
