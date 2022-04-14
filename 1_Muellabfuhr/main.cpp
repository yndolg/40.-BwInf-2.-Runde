#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include <set>
#include <limits>
#include <map>
#include <random>
#include <algorithm>
#include <functional>
#include <iomanip>
#include "Heuristik.h"

using namespace std;

vector<vector<pair<int, int>>> calc_adj(const vector<tuple<int, int, int>> &edges, int n) {
    vector<vector<pair<int, int>>> adj(n, vector<pair<int, int>>());
    for (const auto edge: edges) {
        adj[get<0>(edge)].emplace_back(get<1>(edge), get<2>(edge));
        adj[get<1>(edge)].emplace_back(get<0>(edge), get<2>(edge));
    }
    return adj;
}

mt19937 rng;

int K = 5; // Number of cars

class GA;

class Chromosome {
    const vector<tuple<int, int, int>> &edges;
    const vector<vector<pair<int, int>>> &adj;

public:

// vector<pair<edge, direction>>
    vector<pair<int, bool>> code;
public:
    Chromosome mutate() const {
        Chromosome copy = *this;
        uniform_real_distribution<float> dist(0, 1);
        uniform_int_distribution<int> int_dist(0, copy.code.size() - 1);

        for (auto &i: copy.code) {
            if (dist(rng) < 0.5) {
                i.second = !i.second;
            }
        }
        for (int i = 0; i < (double)copy.code.size() / 300.0; i++) {
            swap(copy.code[int_dist(rng)], copy.code[int_dist(rng)]);
        }
        return copy;
    }

    static Chromosome crossover(Chromosome c1, Chromosome c2) {
        int length = c1.code.size()/5;
        uniform_int_distribution<int> pos_dist(0, c1.code.size() - 1 -length);
        int start = pos_dist(rng);
        int end = start + length;

        Chromosome child = c2;

        set<int> marked;

        for(int i = start ; i < end; i++){
            child.code[i] = c1.code[i];
            marked.insert(c1.code[i].first);
        }

        int pos = 0;
        for(int i = 0; i < c1.code.size(); i++){
            if(i >= start && i < end){
                continue;
            }
            while(marked.find(c2.code[pos].first) != marked.end())
                ++pos;
            child.code[i] = c2.code[pos];
            pos++;
        }
        return child;
    }

public:
    Chromosome(const vector<vector<pair<int, int>>> &adj, const vector<tuple<int, int, int>> &edges)
            : adj(adj), edges(edges) {
        uniform_int_distribution<int> bool_dist(0, 1);

        // New Car markers
        for( int i = 1; i <= K; i++){
            code.emplace_back(-i, false);
        }

        for (int i = 0; i < edges.size(); i++) {
            code.emplace_back(i, bool_dist(rng));
        }
        shuffle(code.begin(), code.end(), rng);
    }
};


class GA {
    const vector<tuple<int, int, int>> &edges;
    vector<vector<pair<int, int>>> adj;
    vector<vector<int>> dist_cache;

    int n;
public:
    GA(const vector<tuple<int, int, int>> &edges, int n) : edges(edges), n(n) {
        adj = calc_adj(edges, n);
        for(int i = 0; i < n; i++){
            dist_cache.emplace_back(n, -1);
        }
    }

    void emit(vector<vector<int>> highlights = {}) {
        vector<string> colors = {"green", "red", "blue", "peru", "orchid"};
        int max_len = 0;
        for (const auto edge: edges) {
            max_len = max(max_len, get<2>(edge));
        }
        map<pair<int, int>, float> lens;
        for (const auto edge: edges) {
            int a, b;
            a = get<0>(edge);
            b = get<1>(edge);
            if(a > b)
                swap(a, b);
            lens[{a,b}] = 5;((float)get<2>(edge)/(float)max_len) * 10.0f;
        }

        for(int i = 0; i < highlights.size(); i++) {
            ofstream outfile("out"+ to_string(i)+".dot");
            outfile << "digraph {\n"
                       " edge [len=2] \n";
            outfile << " subgraph Base {\n"
                       "  edge [dir=none]\n";
            for (const auto edge: edges) {
                int a, b;
                a = get<0>(edge);
                b = get<1>(edge);
                if(a > b)
                    swap(a, b);
                outfile << "  " << get<0>(edge) << " -> " << get<1>(edge) << " [label=\"" << get<2>(edge) << "\", len="<< lens[{a,b}] <<"];\n";
            }
            outfile << "};\n";

            outfile << " subgraph {\n"
                       "  edge [color= " << colors[i] << "]\n";
            for (int j = 0; j < highlights[i].size() - 1; j++) {
                int a, b;
                a = highlights[i][j];
                b = highlights[i][j + 1];
                if(a > b)
                    swap(a, b);
                outfile << "  " << highlights[i][j] << " -> " << highlights[i][j + 1] << "[len="<< lens[{a,b}] << "];\n";
            }
            outfile << " }\n";

            outfile << "}";
            outfile.close();
            system(("dot -Kneato -o ../out"+ to_string(i)+".png -Tpng out"+ to_string(i)+".dot").c_str());
        }
    }

    vector<vector<int>> solve() {
        vector<vector<int>> solution;
        vector<Chromosome> pool;
        pool.reserve(50);
        for (int i = 0; i < 50; i++)
            pool.emplace_back(adj, edges);

        ofstream plot("plot2.dat");
        plot << "generation score" << endl;
        int GENERATIONS = 50000;
        for (int generation = 0; generation < GENERATIONS; generation++) {
            vector<pair<int, reference_wrapper<Chromosome>>> scores;
            scores.reserve(pool.size());
            for (auto &chromosome: pool) {
                auto len = total_length(chromosome);

                scores.emplace_back(len, chromosome);
            }

            sort(scores.begin(), scores.end(),
                 [](const pair<int, reference_wrapper<Chromosome>> &p1,
                    const pair<int, reference_wrapper<Chromosome>> &p2) {
                     return p1.first < p2.first;
                 });

            if(generation == GENERATIONS-1)
                solution = get_paths(scores[0].second);


            vector<Chromosome> new_pool;
            new_pool.reserve(pool.size());
            for(int i = 0; i < pool.size()/10.f;i++){
                new_pool.emplace_back(adj, edges);
            }
            for(int i = 0; i < 6; i++){
                new_pool.push_back(scores[i].second.get());
            }
            uniform_int_distribution<int> sampling_dist(0, pool.size() / 10.0);
            double avg = 0;
            while (new_pool.size() < pool.size()) {
                auto r1 = sampling_dist(rng);
                const auto &v1 = scores[r1].second.get();
                const auto &v2 = scores[sampling_dist(rng)].second.get();

                avg += (double) scores[r1].first / (double) pool.size();

                new_pool.push_back(Chromosome::crossover(v1, v2).mutate());
            }
            pool = std::move(new_pool);

            if(generation % 100 == 0){
                cout << "Generation " << fixed << setprecision(5) << generation << " – Average Length: " << avg << " ";
                plot << generation << " " << avg << "\n";
                cout << "Cache hit-miss ratio: " << cache_hit << "/" << cache_miss << " (" << (double)cache_hit /(cache_miss + cache_hit) * 100.0<< "%)\n";
                cache_hit = 0; cache_miss = 0;
            }
        }
        return solution;
    }

    void printEstimates(){
        long total_length = 0;
        for(const auto& e: edges){
            total_length += get<2>(e);
        }
        cout << "Graph statistics: \n";
        cout << " total length: " << total_length << "\n";
        cout << " lower bound: " << total_length / 5 << "\n";
        cout << " upper bound: " << total_length * 2  << "\n";

    }

private:
    int total_length(Chromosome &c) {
        int max_sum = 0;
        int sum = 0;
        int pos = 0;

        int start = 0;
        while(c.code[start].first >= 0)
            start++;
        start += 1;
        for (int i = start; i < c.code.size() + start; i++) {
            auto waypoint = c.code[i%c.code.size()];
            if(waypoint.first < 0){ // Divider
                sum += dijkstra_cache(pos, 0); // Zurück zum Depot
                max_sum = max(max_sum, sum);
                sum = 0;
                pos = 0;
            }else{ // Edge
                int p1, p2, len;
                tie(p1, p2, len) = edges[waypoint.first];
                if (waypoint.second)
                    swap(p1, p2);

                sum += dijkstra_cache(pos, p1);
                sum += len;
                pos = p2;
            }
        }

        return max_sum;
    }

    vector<vector<int>> get_paths(Chromosome& c){

        vector<vector<int>> paths;
        int start = 0;
        while(c.code[start].first >= 0)
            start++;
        start += 1;
        int pos = 0;
        vector<int> current;

        for (int i = start; i < c.code.size() + start; i++) {
            auto waypoint = c.code[i%c.code.size()];
            if(waypoint.first < 0){ // Divider
                auto r = dijkstra(pos, 0, true); // Zurück zum Depot
                current.insert(current.end(), r.second.begin(), r.second.end());
                paths.push_back(current);

                current = {};
                pos = 0;
            }else{ // Edge
                int p1, p2, len;
                tie(p1, p2, len) = edges[waypoint.first];
                if (waypoint.second)
                    swap(p1, p2);

                auto p = dijkstra(pos, p1, true);
                current.insert(current.end(), p.second.begin(), p.second.end());
                pos = p2;
            }
        }
        return paths;
    }

    int cache_hit;
    int cache_miss;
    int dijkstra_cache(int start, int end){
        if(dist_cache[start][end] < 0){
            cache_miss += 1;
            dist_cache[start][end] = dijkstra(start, end).first;
        }else{
            cache_hit+=1;
        }
        return dist_cache[start][end];
    }

    pair<int, vector<int>> dijkstra(int start, int end, bool reconstruct = true) {
        vector<int> prev(adj.size(), -1);
        vector<int> d(adj.size(), numeric_limits<int>::max());

        set<pair<int, int>> q;
        q.insert({0, start});
        d[start] = 0;
        while (!q.empty()) {

            int v = q.begin()->second;
            q.erase(q.begin());

            if (v == end) {
                vector<int> path;
                if(reconstruct){
                    int pos = v;
                    path.push_back(v);
                    while(pos != start){
                        pos = prev[pos];
                        path.push_back(pos);
                    }
                    reverse(path.begin(), path.end());
                }
                return {d[v], path};
            }

            for (auto edge: adj[v]) {
                int to = edge.first;
                int len = edge.second;

                if (d[v] + len < d[to]) {
                    q.erase({d[to], to});
                    d[to] = d[v] + len;
                    prev[to] = v;
                    q.insert({d[to], to});
                }
            }
        }
        return {};
    }

};


int main() {
    int n, m;
    ifstream in("../Daten/muellabfuhr7.txt");

    in >> n >> m;
    vector<tuple<int, int, int>> edges;
    for (int i = 0; i < m; i++) {
        int a, b, c;
        in >> a >> b >> c;
        edges.emplace_back(a, b, c);
    }
    GA ga(edges, n);
    ga.emit();
    ga.printEstimates();
    auto solution = ga.solve();
    ga.emit(solution);
    return 0;
}
