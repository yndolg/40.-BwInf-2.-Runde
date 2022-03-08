//
// Created by niels on 06.03.22.
//

#include <algorithm>
#include <array>

#include "State.h"
using namespace std;

array<int, 16> table = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};

std::string State::visualize() const {
    std::array<std::string, 5> lines;
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

std::array<std::string, 5> State::visualize_char(std::bitset<7> c) {
    std::array<std::string, 5> r;

    if(c[0] && c[5]){
        r[0].append("┏");
    }else if(c[0]){
        r[0].append("╺");
    }else if(c[5]){
        r[0].append("╻");
    }else{
        r[0].append(" ");
    }

    r[1].append(c[5] ? "┃" : " ");

    if(c[5] && c[6] && c[4]){
        r[2].append("┣");
    }else if(c[4] && c[6]){
        r[2].append("┏");
    }else if(c[5] && c[6]){
        r[2].append("┗");
    }else if(c[6]){
        r[2].append("╺");
    }else if(c[5] && c[4]){
        r[2].append("┃");
    }else if(c[5] || c[4]){
        r[2].append(c[5] ? "╹": "╻");
    }else{
        r[2].append(" ");
    }

    r[3].append(c[4] ? "┃" : " ");


    if(c[3] && c[4]){
        r[4].append("┗");
    }else if(c[3]){
        r[4].append("╺");
    }else if(c[4]){
        r[4].append("╹");
    }else{
        r[4].append(" ");
    }

    // middle part
    for(int i = 0; i < 3; i++){
        r[0].append(c[0] ? "━": " ");
        r[1].append(" ");
        r[2].append(c[6] ? "━": " ");
        r[3].append(" ");
        r[4].append(c[3] ? "━": " ");
    }

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

    r[1].append(c[1] ? "┃" : " ");

    if(c[1] && c[6] && c[2]){
        r[2].append("┫");
    }else if(c[1] && c[6]){
        r[2].append("┛");
    }else if(c[2] && c[6]){
        r[2].append("┓");
    }else if(c[6]){
        r[2].append("╸");
    }else if(c[1] && c[2]){
        r[2].append("┃");
    }else if(c[1] || c[2]){
        r[2].append(c[1] ? "╹": "╻");
    }else{
        r[2].append(" ");
    }

    r[3].append(c[2] ? "┃" : " ");

    if(c[3] && c[2]){
        r[4].append("┛");
    }else if(c[3]){
        r[4].append("╸");
    }else if(c[2]){
        r[4].append("╹");
    }else{
        r[4].append(" ");
    }
    return r;
}

State State::fromString(const std::string& s) {
    State state;
    for(auto c: s){
        int n;
        if(c >= '0' && c <= '9')
            n = c - '0';
        if(c >= 'A' && c <= 'F')
            n = 10 + (c- 'A');
        state.positions.emplace_back(table[n]);
    }
    return state;
}

string State::toString() {
    std::string r;
    for(auto digit: positions){
        auto v = digit.to_ulong();
        auto pos = std::find(table.begin(), table.end(), v);
        if(pos != table.end()){
            r.push_back("0123456789ABCDEF"[std::distance(table.begin(), pos)]);
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
        auto pos = std::find(table.begin(), table.end(), bits);
        if(pos != table.end()){
            v += std::distance(table.begin(), pos);
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
