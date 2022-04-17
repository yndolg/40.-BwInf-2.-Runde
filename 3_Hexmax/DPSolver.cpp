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

int INFINITY = numeric_limits<int>::max() / 2;

std::string DPSolver::solve(const std::string &string, int i) {
    word = string;
    max_moves = i;

    word_digits.reserve(word.size());
    for (char c: word) {
        word_digits.push_back(State::digitFromChar(c));
    }

    memo = vector<int>((string.size() + 1) * (1 + 2 * max_moves), -1);

    auto r = reconstruct(i);

    int c = 0;
    for (auto x: memo)
        if (x == -1)
            c += 1;

    cout << "Cache fill: " << c << "/" << memo.size() << "\n";
    return r;
}

const string HEX_DIGITS = "0123456789ABCDEF";

std::string DPSolver::reconstruct(int max_moves) {

    calcBottomUp();

    int surplus = 0;
    int moves_left = max_moves;
    string s;

    for (int pos = 0; pos < word.size(); pos++) {
        for (int i = 15; i >= 0; i--) {
            auto r = difference(State::digitFromChar(word[pos]), State::digitFromChar(HEX_DIGITS[i]));
            int added_moves = r.first;
            int added_surplus = r.second;

            int new_surplus = surplus + added_surplus;

            if (getDP(pos + 1, new_surplus) + added_moves + max(0, added_surplus) <= moves_left) {
                surplus = new_surplus;
                s += HEX_DIGITS[i];
                moves_left -= added_moves + max(0, added_surplus);
                break;
            }
        }
    }

    return s;
}

void DPSolver::calcBottomUp() {
    for (int pos = word.length(); pos >= 0; pos--) {
        for (int surplus = -max_moves; surplus <= max_moves; surplus++) {

            if (pos == word.length()) {
                if (surplus == 0)
                    memo[memoKey(pos, surplus)] = 0;
                else
                    memo[memoKey(pos, surplus)] = INFINITY;
                continue;
            }

            int min_v = INFINITY;
            int min_p = -1;
            for (int i = 15; i >= 0; i--) {
                auto r = difference(word_digits[pos], State::digitFromNumber(i));

                int added_moves = r.first;
                int added_surplus = r.second;

                auto needed_moves =
                        getDP(pos + 1, surplus + added_surplus) + added_moves + max(added_surplus, 0);

                if (needed_moves < min_v) {
                    min_v = needed_moves;
                }
            }

            memo[memoKey(pos, surplus)] = min_v;
        }
    }
}

// pair<moves, surplus>
pair<int, int> DPSolver::difference(std::bitset<7> c1, std::bitset<7> c2) {
    size_t excess = (c1 & ~c2).count();
    size_t needed = (~c1 & c2).count();
    if (excess > needed) {
        return {needed, excess - needed};
    } else {
        return {excess, -(needed - excess)};
    }
}

std::string DPSolver::getMoves(std::string old, std::string next) {
    vector<pair<int, int>> excess;
    vector<pair<int, int>> needed;

    auto old_state = State::fromString(old);
    auto new_state = State::fromString(next);

    if (old.length() != next.length())
        return "impossible";
    cout << old_state.visualize();

    stringstream ss;

    for (int i = 0; i < old.length(); i++) {
        for (int j = 0; j < 7; j++) {
            if (new_state.positions[i][j] && !old_state.positions[i][j]) {
                needed.emplace_back(i, j);
            } else if (!new_state.positions[i][j] && old_state.positions[i][j]) {
                excess.emplace_back(i, j);
            }
        }
        while (!(excess.empty() || needed.empty())) {
            ss << excess.back().first << ", " << excess.back().second << " -> " << needed.back().first << ", "
               << needed.back().second
               << "\n";
            old_state.positions[excess.back().first][excess.back().second] = false;
            old_state.positions[needed.back().first][needed.back().second] = true;
            ss << old_state.visualize();
            excess.pop_back();
            needed.pop_back();
        }

    }
    if (!(needed.empty() && excess.empty()))
        return "impossible";
    return ss.str();
}

int DPSolver::getDP(int pos, int d) {
    // This can not lead anywhere, as it already uses to many moves
    //  --> result does not even fit into cache
    if (abs(d) > max_moves)
        return INFINITY;

    return memo[memoKey(pos, d)];
}

int DPSolver::memoKey(int pos, int d) const {
    return (pos * (2 * max_moves + 1)) + d + max_moves;
}