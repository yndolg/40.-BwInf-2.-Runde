//
// Created by niels on 18.04.22.
//

#ifndef HEXMAX_TSPSOLVER_H
#define HEXMAX_TSPSOLVER_H


#include <string>
#include <vector>
#include <random>
#include "State.h"

class TSPSolver {
    typedef std::vector<std::pair<int, int>> Individuum;
    std::mt19937 rng;
public:
    TSPSolver();
    std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> solve(const std::string& from, const std::string& to);

    static float length(const std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>& moves);
    // Anzahl der Mutationen
    float MUTATION_RATE = 0.01;
    // Größte der Population
    int POPULATION_SIZE = 100;
    // Anzahl der Individuen, die ohne Veränderung übernommen werden
    int ELITES = 10;
    // Anzahl der vollständig zufälligen Individuen in jeder Generation
    int NEW_INDIVIDUUMS = 10;

private:
    void shuffle(Individuum& individuum);

    Individuum mutate(const Individuum& individuum);
    static float length(Individuum individuum);

    static bool is_valid(Individuum ind, const State& state);
};


#endif //HEXMAX_TSPSOLVER_H
