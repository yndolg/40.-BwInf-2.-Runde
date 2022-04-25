#ifndef INC_3_HEXMAX_DPSOLVER_H
#define INC_3_HEXMAX_DPSOLVER_H

#include <string>
#include <bitset>
#include <unordered_map>
#include <unordered_set>

#include "Zustand.h"

class DPSolver {
private:
    int max_umlegungen;

    std::vector<int> memo;
    std::vector<int> parent;

    std::string word;
    std::vector<std::bitset<7>> word_digits;

    static std::pair<int, int> unterschied(std::bitset<7> c1, std::bitset<7> c2);

    [[nodiscard]] int memoKey(int pos, int d) const;

    std::string rekonstruiere();

    int getMemo(int pos, int surplus);

    void dpVorberechnen();


public:
    std::string solve(const std::string &ursprung, int i);

    static std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>
    getMoves(const std::string &alt, const std::string &ziel);

};


#endif //INC_3_HEXMAX_DPSOLVER_H
