//
// Created by niels on 08.03.22.
//

#include "DPSolver.h"

#include <utility>
#include <string>
#include <sstream>
#include <iostream>
#include <limits>

using namespace std;

int INFINITY = numeric_limits<int>::max()/2;

std::string DPSolver::solve(const std::string& string, int i) {
    word = string;
    max_moves = i;

    word_digits.reserve(word.size());
    for(char c: word){
        word_digits.push_back(State::digitFromChar(c));
    }


    memo = vector<int>((string.size()+1) * (1+2*max_moves), -1);

    auto r = reconstruct(i);
    cout << "Cache rate: " << cacheHit  << "/" << (cacheMiss+cacheHit) <<"\n";

    int c = 0;
    for(auto x: memo)
        if(x == -1)
            c += 1;
    cout << "Cache fill: " << c << "/" << memo.size() << "\n";
    return r;
}

const string HEX_DIGITS = "0123456789ABCDEF";

std::string DPSolver::reconstruct(int max_moves){
    int surplus = 0;
    int pos = 0;
    int moves_left = max_moves;
    string s;
    while(pos < word.size()){
        for(int i = 15; i >= 0; i--){
            auto r = difference(State::digitFromChar(word[pos]), State::digitFromChar(HEX_DIGITS[i]));
            int added_moves = r.first;
            int added_surplus = r.second;

            int new_surplus = surplus + added_surplus;
            int new_pos = pos + 1;

            if(calcMemo(new_pos, new_surplus) + added_moves + max(0, added_surplus) <= moves_left){
                surplus = new_surplus;
                s += HEX_DIGITS[i];
                pos = new_pos;
                moves_left -= added_moves + max(0, added_surplus);
                break;
            }
        }
    }
    return s;
}


int DPSolver::calc(int pos, int surplus) {
    if(pos == word.length()){
        if(surplus == 0)
            return 0;
        else
            return INFINITY;
    }

    int min_v = INFINITY;

    for (int i = 15; i >= 0; i--){

        auto r = difference(word_digits[pos], State::digitFromNumber(i));

        int added_moves = r.first;
        int added_surplus = r.second;

        auto needed_moves = calcMemo(pos + 1, surplus + added_surplus) + added_moves + max(added_surplus, 0);

        if(needed_moves < min_v){
            min_v = needed_moves;
        }
    }
    return min_v;
}

// pair<moves, surplus>
pair<int, int> DPSolver::difference(std::bitset<7> c1, std::bitset<7> c2) {
    size_t excess = (c1 & ~c2).count();
    size_t needed = (~c1 & c2).count();
    if(excess > needed){
        return {needed, excess - needed};
    }else{
        return {excess, - (needed - excess)};
    }
}

std::string DPSolver::getMoves(std::string old, std::string next) {
    vector<pair<int, int>> excess;
    vector<pair<int, int>> needed;

    auto old_state = State::fromString(old);
    auto new_state = State::fromString(next);

    if(old.length() != next.length())
        return "impossible";
    for(int i = 0; i < old.length(); i++){
        for(int j = 0; j < 7; j++){
            if(new_state.positions[i][j] && !old_state.positions[i][j]){
                needed.emplace_back(i,j);
            }else if(!new_state.positions[i][j] && old_state.positions[i][j]){
                excess.emplace_back(i,j);
            }
        }
    }
    if(needed.size() != excess.size())
        return "impossible";
    stringstream ss;
    for(int i = 0; i < needed.size(); i++){
        ss << excess[i].first << ", " << excess[i].second << " -> " << needed[i].first << ", " << needed[i].second << "\n";
    }
    return ss.str();
}

int DPSolver::calcMemo(int pos,  int d) {
    // This can not lead anywhere, as it already uses to many moves
    //  --> result does not even fit into cache
    if(abs(d) > max_moves)
        return INFINITY;

    auto it = memo[memoKey({pos, d})];
    if(it >= 0){
        cacheHit++;
        return it;
    }
    cacheMiss++;

    auto r = calc(pos, d);
    memo[memoKey({pos, d})] = r;
    return r;
}

int DPSolver::memoKey(DPSolver::memo_key m) const {
    return (m.pos * (2*max_moves+1)) + m.d + max_moves;
}

bool DPSolver::memo_key::operator==(const DPSolver::memo_key &rhs) const  {
    return pos == rhs.pos &&
           d == rhs.d;
}

bool DPSolver::memo_key::operator!=(const DPSolver::memo_key &rhs) const {
    return !(rhs == *this);
}