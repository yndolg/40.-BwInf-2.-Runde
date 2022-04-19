//
// Created by niels on 06.03.22.
//

#include <algorithm>
#include <array>

#include "State.h"
using namespace std;


std::string State::visualize() const {
    std::array<std::string, 3> lines;
    for(auto c: positions){
        auto r = visualize_char(c);
        for(int i = 0; i < r.size(); i++){
            lines[i] += " " + r[i];
        }
    }
    std::string result;
    for(const auto& line: lines){
        result.append(line + "\n");
    }
    return result;
}

std::array<std::string, 3> State::visualize_char(std::bitset<7> c) {
    std::array<std::string, 3> r;

    if(c[0] && c[5]){
        r[0].append("┏");
    }else if(c[0]){
        r[0].append("╺");
    }else if(c[5]){
        r[0].append("╻");
    }else{
        r[0].append(" ");
    }

    if(c[5] && c[6] && c[4]){
        r[1].append("┣");
    }else if(c[4] && c[6]){
        r[1].append("┏");
    }else if(c[5] && c[6]){
        r[1].append("┗");
    }else if(c[6]){
        r[1].append("╺");
    }else if(c[5] && c[4]){
        r[1].append("┃");
    }else if(c[5] || c[4]){
        r[1].append(c[5] ? "╹": "╻");
    }else{
        r[1].append(" ");
    }

    if(c[3] && c[4]){
        r[2].append("┗");
    }else if(c[3]){
        r[2].append("╺");
    }else if(c[4]){
        r[2].append("╹");
    }else{
        r[2].append(" ");
    }

    // middle part
    r[0].append(c[0] ? "━": " ");
    r[1].append(c[6] ? "━": " ");
    r[2].append(c[3] ? "━": " ");


    // right part
    if(c[0] && c[1]){
        r[0].append("┓");
    }else if(c[0]){
        r[0].append("╸");
    }else if(c[1]){
        r[0].append("╻");
    }else{
        r[0].append(" ");
    }

    if(c[1] && c[6] && c[2]){
        r[1].append("┫");
    }else if(c[1] && c[6]){
        r[1].append("┛");
    }else if(c[2] && c[6]){
        r[1].append("┓");
    }else if(c[6]){
        r[1].append("╸");
    }else if(c[1] && c[2]){
        r[1].append("┃");
    }else if(c[1] || c[2]){
        r[1].append(c[1] ? "╹": "╻");
    }else{
        r[1].append(" ");
    }

    if(c[3] && c[2]){
        r[2].append("┛");
    }else if(c[3]){
        r[2].append("╸");
    }else if(c[2]){
        r[2].append("╹");
    }else{
        r[2].append(" ");
    }
    return r;
}

State State::fromString(const std::string& s) {
    State state;
    for(auto c: s){
        state.positions.emplace_back(digitFromChar(c));
    }
    return state;
}

string State::toString() {
    std::string r;
    for(auto digit: positions){
        auto v = digit.to_ulong();
        auto pos = std::find(TABLE.begin(), TABLE.end(), v);
        if(pos != TABLE.end()){
            r.push_back("0123456789ABCDEF"[std::distance(TABLE.begin(), pos)]);
        }else{
            r.push_back('X');
        }
    }
    return r;
}

long long State::getValue() {
    long long v = 0;
    for(auto digit: positions){
        v*=16;
        auto bits = digit.to_ulong();
        auto pos = std::find(TABLE.begin(), TABLE.end(), bits);
        if(pos != TABLE.end()){
            v += std::distance(TABLE.begin(), pos);
        }else{
           return -1;
        }
    }
    return v;
}

bool State::operator==(const State &rhs) const {
    return positions == rhs.positions;
}

bool State::operator!=(const State &rhs) const {
    return !(rhs == *this);
}
