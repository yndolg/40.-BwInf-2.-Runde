//
// Created by niels on 11.04.22.
//

#ifndef STAPEL_UTILS_H
#define STAPEL_UTILS_H

#include <vector>
#include <string>

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

    static std::vector<std::vector<int>> gauss(std::vector<std::vector<int>> matrix);

    static Instance readInstanceFromFile(std::ifstream& ifs);

    static std::vector<std::vector<int>> transpose(std::vector<std::vector<int>>);

    static void print_matrix(const std::vector<std::vector<int>> &matrix);
};


#endif //STAPEL_UTILS_H
