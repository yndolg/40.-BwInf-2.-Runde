//
// Created by niels on 08.03.22.
//

#include "DPSolver.h"
#include "State.h"

#include <utility>
#include <string>
#include <sstream>
#include <iostream>
#include <limits>

using namespace std;

int INFINITY = numeric_limits<int>::max() / 2;

/*
 * Findet den die größte Hex-Zahl, die mit i Umlegungen aus 'ursprung'
 * gebildet werden kann.
 */
std::string DPSolver::solve(const std::string &ursprung, int i) {
    word = ursprung;
    max_umlegungen = i;

    word_digits.reserve(word.size());
    for (char c: word) {
        word_digits.push_back(State::digitFromChar(c));
    }

    memo = vector<int>((ursprung.size() + 1) * (1 + 2 * max_umlegungen), -1);
    dpVorberechnen();
    auto r = rekonstruiere();
    return r;
}

const string HEX_DIGITS = "0123456789ABCDEF";

/*
 * Rekonstruiert die größte Hex-Zahl. Vorher muss 'dpVorberechnen()'
 * aufgerufen worden sein.
 */
std::string DPSolver::rekonstruiere() {
    int ueberschuss = 0;
    int umlegungen = max_umlegungen;
    string s;
    for (int pos = 0; pos < word.size(); pos++) {
        for (int i = 15; i >= 0; i--) {
            auto r = unterschied(State::digitFromChar(word[pos]), State::digitFromChar(HEX_DIGITS[i]));
            if (getDP(pos + 1, ueberschuss + r.second) + r.first <= umlegungen) {
                ueberschuss = ueberschuss + r.second;
                s += HEX_DIGITS[i];
                umlegungen -= r.first;
                break;
            }
        }
    }
    return s;
}

/*
 * Berechnet 'memo' vor. memo[e][p] gibt an, wie viele Umlegungen mindestens benötigt
 * werden, um aus den letzten 'e' Stellen mit 'p' zusätzlichen Stäbchen eine valide
 * Hex-Zahl zu legen. 'p' kann dabei auch negativ sein. Das Legen von zusätzlichen
 * Stäbchen zählt nicht als Umlegung, nur das Nehmen.
 */
void DPSolver::dpVorberechnen() {
    for (int pos = word.length(); pos >= 0; pos--) {
        for (int surplus = -max_umlegungen; surplus <= max_umlegungen; surplus++) {
            if (pos == word.length()) {
                if (surplus == 0)
                    memo[memoKey(pos, surplus)] = 0;
                else
                    memo[memoKey(pos, surplus)] = INFINITY;
                continue;
            }

            int min_v = INFINITY;
            for (int i = 15; i >= 0; i--) {
                auto r = unterschied(word_digits[pos], State::digitFromNumber(i));

                auto needed_moves = getDP(pos + 1, surplus + r.second) + r.first;
                if (needed_moves < min_v) {
                    min_v = needed_moves;
                }
            }

            memo[memoKey(pos, surplus)] = min_v;
        }
    }
}

/*
 * Berechnet, (0) wie viele Umlegungen benötigt werden, um von c1 zu c2 zu
 *                kommen. Dabei zählt das Wegnehmen von Stäbchen als Umlegung,
 *                das hinzufügen aber nicht. So werden Umlegungen zwischen
 *                Stellen nur einmal gezählt.
 *            (1) wie viele Stäbchen dabei überschüssig werden
 */
pair<int, int> DPSolver::unterschied(std::bitset<7> c1, std::bitset<7> c2) {
    size_t excess = (c1 & ~c2).count();
    size_t needed = (~c1 & c2).count();
    auto diff = excess - needed;
    return {min(excess, needed) + max<int>(diff, 0), diff};
}

/*
 * Gibt den Wert aus 'memo' aus. Liegt der Wert außerhalb des relevanten
 * Bereichs, wird INFINITY zurückgegeben.
 */
int DPSolver::getDP(int pos, int d) {
    // Benutzt bereits zu viele Umlegungen. Der Wert muss nicht in 'memo'
    // gespeichert werden.
    if (abs(d) > max_umlegungen)
        return INFINITY;

    return memo[memoKey(pos, d)];
}

int DPSolver::memoKey(int pos, int d) const {
    return (pos * (2 * max_umlegungen + 1)) + d + max_umlegungen;
}


/*
 * Berechnet Umlegungen, um von 'alt' auf 'ziel' zu kommen und dabei nie
 * eine Stelle vollständig zu leeren in O(n)
 */
vector<pair<pair<int, int>, pair<int, int>>> DPSolver::getMoves(const std::string &alt, const std::string &ziel) {
    vector<pair<int, int>> excess;
    vector<pair<int, int>> needed;

    auto alt_state = State::fromString(alt);
    auto ziel_state = State::fromString(ziel);

    if (alt.length() != ziel.length()) {
        cout << "Umlegung nicht möglich, unterschiedliche Zahl an Stellen.\n";
        return {};
    }

    vector<pair<pair<int, int>, pair<int, int>>> r;
    for (int i = 0; i < alt.length(); i++) {
        for (int j = 0; j < 7; j++) {
            if (ziel_state.positions[i][j] && !alt_state.positions[i][j]) {
                needed.emplace_back(i, j);
            } else if (!ziel_state.positions[i][j] && alt_state.positions[i][j]) {
                excess.emplace_back(i, j);
            }
        }
        while (!(excess.empty() || needed.empty())) {
            r.emplace_back(excess.back(), needed.back());
            excess.pop_back();
            needed.pop_back();
        }
    }
    if (!excess.empty() || !needed.empty()) {
        cout << "Umlegung nicht möglich, unterschiedliche Anzahl an Stäbchen.\n";
        return {};
    }
    return r;
}

