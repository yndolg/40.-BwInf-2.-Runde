#include <iostream>
#include <algorithm>
#include <iomanip>
#include "TSPSolver.h"
#include "Zustand.h"
#include "DPSolver.h"

using namespace std;


/*
 * Findet Umlegungen, bei denen sich möglichst wenig bewegt werden muss über
 * einen evolutionären Algorithmus
 */
std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>
TSPSolver::solve(const std::string &from, const std::string &to) {

    vector<Individuum> population;

    auto from_state = Zustand::fromString(from);

    // Umlegungen des Greedy-Algorithmus als Ausgangspunkt verwenden
    auto moves_greedy = DPSolver::getMoves(from, to);
    Individuum individuum;
    for (auto &move: moves_greedy) {
        individuum.push_back(move.first);
        individuum.push_back(move.second);
    }
    population.push_back(individuum);

    // Den Rest mit zufälligen Individuuen initialisieren
    while (population.size() < POPULATION_SIZE) {
        Individuum u = individuum;
        shuffle(u);
        if (is_valid(u, from_state))
            population.push_back(u);
    }

    Individuum best = individuum;

    for (int generation = 0; generation < 1000; generation++) {

        // Längen der Wege berechnen und sortieren
        vector<pair<int, Individuum>> lengths;
        for (const auto &ind: population) {
            lengths.emplace_back(length(ind), ind);
        }
        std::sort(lengths.begin(), lengths.end());
        best = lengths[0].second;

        // Verteilung zur Selektion
        geometric_distribution dist(0.05);
        population.clear();


        int avg_dist = 0;
        for (int i = 0; i < ELITES; i++) {
            population.push_back(lengths[i].second);
            avg_dist += lengths[i].first;
        }

        for (int i = 0; i < NEW_INDIVIDUUMS; i++) {
            Individuum u = individuum;
            shuffle(u);
            if (is_valid(u, from_state))
                population.push_back(u);
        }

        while (population.size() < POPULATION_SIZE) {
            auto u = mutate(lengths[min<int>(dist(rng), lengths.size() - 1)].second);
            if (is_valid(u, from_state))
                population.push_back(u);
        }
    }

    // Rückgabewert aus 'best' erstellen
    vector<pair<pair<int, int>, pair<int, int>>> moves;
    for (int i = 0; i < best.size(); i += 2) {
        moves.emplace_back(best[i], best[i + 1]);
    }
    return moves;
}

TSPSolver::TSPSolver() {
    random_device dev;
    rng = mt19937(dev());
}

/*
 * Mischt eine Lösung, um eine vollständig zufällige Lösung zu
 * generieren. Dabei dürfen nur gerade mit geraden und ungerade mit
 * ungeraden Positionen getauscht werden, da bei geraden Positionen
 * ein Stäbchen aufgenommen und bei ungeraden ein Stäbchen abgelegt
 * wird.
 */
void TSPSolver::shuffle(TSPSolver::Individuum &individuum) {
    vector<pair<int, int>> needed, excess;
    for (int i = 0; i < individuum.size(); i += 2) {
        excess.push_back(individuum[i]);
        needed.push_back(individuum[i + 1]);
    }
    std::shuffle(needed.begin(), needed.end(), rng);
    std::shuffle(excess.begin(), excess.end(), rng);
    for (size_t i = 0; i < needed.size(); i++) {
        individuum[2 * i] = excess[i];
        individuum[2 * i + 1] = needed[i];
    }
}

/*
 * Mutiert das Individuum durch Tauschen von Elementen
 */
TSPSolver::Individuum TSPSolver::mutate(const TSPSolver::Individuum &individuum) {
    Individuum child = individuum;
    uniform_int_distribution<int> dist(0, (individuum.size() / 2) - 1);
    uniform_int_distribution<int> bool_dist(0, 1);
    for (int i = 0; i < max(1, (int) (MUTATION_RATE * individuum.size())); i++) {
        int type = bool_dist(rng);
        swap(child[dist(rng) * 2 + type], child[dist(rng) * 2 + type]);
    }
    return child;
}

/*
 * Überprüft, ob bei den Umlegungen eine Stelle vollständig geleert wird
 */
bool TSPSolver::is_valid(TSPSolver::Individuum ind, const Zustand &old_state) {
    vector<int> counts;
    for (const auto &pos: old_state.stellen) {
        counts.push_back(pos.count());
    }
    for (int i = 0; i < ind.size(); i += 2) {
        counts[ind[i].first] -= 1;
        counts[ind[i + 1].first] += 1;
        if (counts[ind[i].first] <= 0) {
            return false;
        }
    }
    return true;
}

/*
 * Berechnet die Länge des Weges beim Umlegen
 */
float TSPSolver::length(TSPSolver::Individuum individuum) {
    float len = 0;
    pair<int, int> current_pos = individuum[0];
    // Positionen der Stäbchen innerhalb einer Stelle
    const vector<array<float, 2>> POSITIONS = {
            {0.5, 0},
            {1,   0.5},
            {1,   1.5},
            {0.5, 2},
            {0,   1.5},
            {0,   0.5},
            {0.5, 1}
    };
    for (int i = 1; i < individuum.size(); i++) {
        // Distanz über Satz des Pythagoras berechnen
        auto start_pos = POSITIONS[current_pos.second];
        start_pos[0] += current_pos.first;
        auto end_pos = POSITIONS[individuum[i].second];
        start_pos[0] += individuum[i].first;
        len += sqrt(pow(end_pos[0] - start_pos[0], 2) + pow(end_pos[1] - start_pos[1], 2));
        current_pos = individuum[i];
    }
    return len;
}

float TSPSolver::length(const std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> &moves) {
    Individuum ind;
    for (const auto move: moves) {
        ind.push_back(move.first);
        ind.push_back(move.second);
    }
    return length(ind);
}
