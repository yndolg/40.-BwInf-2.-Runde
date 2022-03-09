//
// Created by niels on 06.03.22.
//

#ifndef INC_3_HEXMAX_STATE_H
#define INC_3_HEXMAX_STATE_H

#include <vector>
#include <bitset>
#include <string>
#include <array>

class State {
public:
    std::vector<std::bitset<7>> positions;
    std::string visualize() const;
    static std::array<std::string, 5> visualize_char(std::bitset<7> c);

    static State fromString(const std::string& s);
    long long int getValue();
    std::string toString();

    bool operator==(const State &rhs) const;

    bool operator!=(const State &rhs) const;

    constexpr static const std::array<int, 16> TABLE = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};

    static std::bitset<7> digitFromChar(char c){
        int n;
        if(c >= '0' && c <= '9')
            n = c - '0';
        if(c >= 'A' && c <= 'F')
            n = 10 + (c- 'A');
        return TABLE[n];
    };
};

struct StateHash
{
    size_t operator()(const State& pos) const
    {
        size_t h = 12345;
        for(const auto& p:pos.positions){
            h = h * 101 + (unsigned) p.to_ulong();
        }
        return h;
    }
};


#endif //INC_3_HEXMAX_STATE_H
