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
