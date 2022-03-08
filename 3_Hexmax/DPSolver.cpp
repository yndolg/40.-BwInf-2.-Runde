//
// Created by niels on 08.03.22.
//

#include "DPSolver.h"

#include <utility>
#include <string>
#include <sstream>
#include <iostream>

using namespace std;

std::string DPSolver::solve(const std::string& string, int i) {
    word = string;
    auto r = calcMemo(0, i, 0);
    cout << "Cache rate: " << cacheHit  << "/" << (cacheMiss+cacheHit) <<"\n";
    return r;
}
const string HEX_DIGITS = "0123456789ABCDEF";

std::string DPSolver::calc(int pos, int c, int surplus) {
    if(c < 0)
        return "X";

    if(pos == word.length()){
        if(surplus == 0)
            return "";
        else
            return "X";
    }

    for (int z = 15; z >= 0; z--){
        auto new_state = State::fromString(word.substr(pos, 1)).positions[0];
        auto old_state = State::fromString(HEX_DIGITS.substr(z, 1)).positions[0];

        auto r = difference(old_state, new_state);

        int added_moves = r.first;
        int added_surplus = r.second;

        auto v = calcMemo(pos + 1,
                      c - added_moves - max(added_surplus, 0),
                      surplus + added_surplus);
        if(v != "X"){
            return HEX_DIGITS[z] + v;
        }
    }
    return "X";
}

// pair<moves, surplus>
pair<int, int> DPSolver::difference(std::bitset<7> c1, std::bitset<7> c2) {
    int excess = 0;
    int needed = 0;
    for(int i= 0 ; i < 7; i++){
        if(c1[i] && !c2[i]){
            ++excess;
        }
        if(!c1[i] && c2[i]){
            ++needed;
        }
    }
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

std::string DPSolver::calcMemo(int pos, int c, int d) {
    auto it = memo.find({pos, c, d});
    if(it != memo.end()){
        cacheHit++;
        return it->second;
    }
    cacheMiss++;
    auto r = calc(pos, c, d);
    memo[{pos, c, d}] = r;
    return r;
}

bool DPSolver::memo_key::operator==(const DPSolver::memo_key &rhs) const  {
    return pos == rhs.pos &&
           c == rhs.c &&
           d == rhs.d;
}

bool DPSolver::memo_key::operator!=(const DPSolver::memo_key &rhs) const {
    return !(rhs == *this);
}
