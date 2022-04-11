//
// Created by niels on 16.03.22.
//

#include "Heuristik.h"

using namespace std;

vector<vector<int>> Heuristik::solve() {
    return dfs(0);
}

Heuristik::Heuristik(std::vector<std::array<int,3>> edges, int n) {
    adj = vector<vector<int>>(n);

    in = vector<int>(n, 0);
    out = vector<int>(n, 0);

    for(auto edge: edges){
        in[edge[0]]++;
        out[edge[0]]++;
        in[edge[1]]++;
        out[edge[0]]++;

        adj[edge[0]].push_back(edge[1]);
        adj[edge[1]].push_back(edge[0]);
    }
}

void Heuristik::dfs(int at) {
    while(out[at] != 0){
        auto next_edge = adj[at][--out[at]];
    }
}
