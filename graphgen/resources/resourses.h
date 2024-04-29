#ifndef LAB4_BMP_GRAPH_GENERATOR_RESOURSES_H
#define LAB4_BMP_GRAPH_GENERATOR_RESOURSES_H

#include <string>
#include <filesystem>

std::string get_path(int digit) {
    std::filesystem::path path = std::filesystem::current_path() / "resources" / "digits";
    return path / (std::to_string(digit) + ".txt");
}

#endif  //LAB4_BMP_GRAPH_GENERATOR_RESOURSES_H
