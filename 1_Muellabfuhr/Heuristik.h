//
// Created by niels on 16.03.22.
//

#ifndef INC_1_MUELLABFUHR_HEURISTIK_H
#define INC_1_MUELLABFUHR_HEURISTIK_H


#include <vector>
#include <tuple>
#include <unordered_map>

class Heuristik {public:
    Heuristik(std::vector<std::array<int, 3>> edges, int n);

    std::vector<std::vector<int>> adj;
    std::vector<int> in;
    std::vector<int> out;

    void dfs(int at);
public:

    std::vector<std::vector<int>> solve();
};


#endif //INC_1_MUELLABFUHR_HEURISTIK_H
