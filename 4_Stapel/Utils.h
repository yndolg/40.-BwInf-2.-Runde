//
// Created by niels on 11.04.22.
//

#ifndef STAPEL_UTILS_H
#define STAPEL_UTILS_H

#include <vector>
#include <string>
#include <boost/dynamic_bitset.hpp>

class Utils {
public:
    struct Instance{
        std::vector<std::vector<int>> H;
        int k;
    };

    template<typename T>
    static std::vector<int> get_true_positions(const T &v) {
        std::vector<int> r;
        for (int i = 0; i < v.size(); i++) {
            if (v[i])
                r.push_back(i);
        }
        return r;
    }

    static void gauss(std::vector<std::vector<int>>& matrix);

    static Instance readInstanceFromFile(std::ifstream& ifs);

    static std::vector<std::vector<int>> transpose(std::vector<std::vector<int>>);

    static void print_matrix(const std::vector<std::vector<int>> &matrix);

    static void efficientGauss(std::vector<std::vector<int>> &matrix);

    static std::vector<boost::dynamic_bitset<>>
    efficientGauss(std::vector<std::vector<int>> &matrix, const std::vector<int> &permutation);

    static void efficientGauss(std::vector<boost::dynamic_bitset<>> &bit_mat);
};


#endif //STAPEL_UTILS_H
