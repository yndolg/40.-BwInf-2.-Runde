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

    static std::vector<std::vector<int>> gauss(std::vector<std::vector<int>> matrix);

    static Instance readInstanceFromFile(const std::string& path);

    static std::vector<std::vector<int>> transpose(std::vector<std::vector<int>>);

    static void print_matrix(const std::vector<std::vector<int>> &matrix)
};


#endif //STAPEL_UTILS_H
