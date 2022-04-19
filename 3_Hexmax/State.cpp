//
// Created by niels on 06.03.22.
//

#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <algorithm>
#include <array>

#include "State.h"

using namespace std;

std::string State::visualize() const {
    std::array<std::string, 3> lines;
    for (auto c: positions) {
        auto r = visualize_char(c);
        for (int i = 0; i < r.size(); i++) {
            lines[i] += " " + r[i];
        }
    }
    std::string result;
    for (const auto &line: lines) {
        result.append(line + "\n");
    }
    return result;
}

/*
 * Zeichnet ein einziges Zeichen mit Box-Drawing-Zeichen. Die Bits in c
 * entsprechen folgenden Segmenten:
 *     ┏━0━┓
 *     5   1
 *     ┣━6━┫
 *     4   2
 *     ┗━3━┛
 * Da das Ergebnis über 3 Zeilen geht, werden drei Strings zurückgegeben.
 */
std::array<std::string, 3> State::visualize_char(std::bitset<7> c) {
    std::array<std::string, 3> r;

    // erste Zeile
    string x;
    if (c[0] && c[5]) {
        x += "┏";
    } else if (c[0]) {
        x += "╺";
    } else if (c[5]) {
        x += "╻";
    } else {
        x += " ";
    }
    x += c[0] ? "━" : " ";
    if (c[0] && c[1]) {
        x += "┓";
    } else if (c[0]) {
        x += "╸";
    } else if (c[1]) {
        x += "╻";
    } else {
        x += " ";
    }
    r[0] = x;

    // zweite Zeile
    x = "";
    if (c[5] && c[6] && c[4]) {
        x += "┣";
    } else if (c[4] && c[6]) {
        x += "┏";
    } else if (c[5] && c[6]) {
        x += "┗";
    } else if (c[6]) {
        x += "╺";
    } else if (c[5] && c[4]) {
        x += "┃";
    } else if (c[5] || c[4]) {
        x += c[5] ? "╹" : "╻";
    } else {
        x += " ";
    }
    x += c[6] ? "━" : " ";
    if (c[1] && c[6] && c[2]) {
        x += "┫";
    } else if (c[1] && c[6]) {
        x += "┛";
    } else if (c[2] && c[6]) {
        x += "┓";
    } else if (c[6]) {
        x += "╸";
    } else if (c[1] && c[2]) {
        x += "┃";
    } else if (c[1] || c[2]) {
        x += c[1] ? "╹" : "╻";
    } else {
        x += " ";
    }
    r[1] = x;

    // dritte Zeile
    x = "";
    if (c[3] && c[4]) {
        x += "┗";
    } else if (c[3]) {
        x += "╺";
    } else if (c[4]) {
        x += "╹";
    } else {
        x += " ";
    }
    x += c[3] ? "━" : " ";
    if (c[3] && c[2]) {
        x += "┛";
    } else if (c[3]) {
        x += "╸";
    } else if (c[2]) {
        x += "╹";
    } else {
        x += " ";
    }
    r[2] = x;
    return r;
}

State State::fromString(const std::string &s) {
    State state;
    for (auto c: s) {
        state.positions.emplace_back(digitFromChar(c));
    }
    return state;
}

/*
 * Gibt die Zwischenstände nach den Umlegungen als String zurück. Zusätzlich
 * wird eine Linie gezeichnet, um zu zeigen, von wo nach wo das Stäbchen
 * bewegt wurde.
 */
string
State::visualisiereUmlegungen(const vector<pair<pair<int, int>, pair<int, int>>> &umlegungen, const string &vorher) {
    stringstream ss;
    auto vorher_state = State::fromString(vorher);
    ss << vorher_state.visualize();
    for (const auto &move: umlegungen) {
        string pfeil = " ";
        const auto &from = move.first;
        const auto &to = move.second;
        for (int i = 0; i < vorher_state.positions.size(); i++) {
            if (i == from.first) {
                if (from.first < to.first) {
                    pfeil += " ┗━━";
                } else if (from.first == to.first) {
                    pfeil += " ┃ ";
                } else if (from.first > to.first) {
                    pfeil += "━┛";
                }
            } else if (i == to.first) {
                if (from.first < to.first) {
                    pfeil += "━┓  ";
                } else if (from.first > to.first) {
                    pfeil += " ┏━━";
                }
            } else if (i > min(to.first, from.first) && i < max(to.first, from.first)) {
                pfeil += "━━━━";
            } else {
                pfeil += "    ";
            }
        }

        ss << pfeil << endl;
        vorher_state.positions[from.first][from.second] = false;
        vorher_state.positions[to.first][to.second] = true;
        ss << vorher_state.visualize();
    }
    return ss.str();
}