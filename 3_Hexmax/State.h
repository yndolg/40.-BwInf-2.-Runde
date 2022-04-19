//
// Created by niels on 06.03.22.
//

#ifndef INC_3_HEXMAX_STATE_H
#define INC_3_HEXMAX_STATE_H

#include <vector>
#include <bitset>
#include <string>
#include <array>

struct State {
    std::vector<std::bitset<7>> positions;

    [[nodiscard]] std::string visualize() const;

    static std::array<std::string, 3> visualize_char(std::bitset<7> c);

    static State fromString(const std::string &s);

    constexpr static const std::array<int, 16> TABLE = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F,
                                                        0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};

    /*
     * Gibt zurück, welche Stäbchen für die Hex-Ziffer 'i' aktiv sind
     */
    static std::bitset<7> digitFromNumber(int i) {
        return TABLE[i];
    }

    /*
     * Gibt zurück, welche Stäbchen für das Zeichen 'c' aktiv sind.
     * c muss eine Hex-Ziffer sein.
     */
    static std::bitset<7> digitFromChar(char c) {
        int n;
        if (c >= '0' && c <= '9')
            n = c - '0';
        if (c >= 'A' && c <= 'F')
            n = 10 + (c - 'A');
        return TABLE[n];
    };

    static std::string
    visualisiereUmlegungen(const std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> &umlegungen,
                           const std::string &vorher);
};

#endif //INC_3_HEXMAX_STATE_H
