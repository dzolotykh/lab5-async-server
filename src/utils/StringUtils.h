#ifndef LAB5_STRINGUTILS_H
#define LAB5_STRINGUTILS_H

#include <string>
#include <cstddef>
#include <random>

class StringUtils {
public:
    static std::string random_string(size_t length);

    static constexpr char numbers_alph[] =
            "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    static constexpr size_t numbers_alph_size = sizeof(numbers_alph) - 1;

    static constexpr char numbers[] = "0123456789";
    static constexpr size_t numbers_size = sizeof(numbers) - 1;

    static constexpr char alph[] = "abcdefghijklmnopqrstuvwxyz";
    static constexpr size_t alph_size = sizeof(alph) - 1;

    static constexpr char ALPH[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    static constexpr size_t ALPH_size = sizeof(ALPH) - 1;
private:
    static std::random_device rd;
    static std::mt19937 gen;
};


#endif //LAB5_STRINGUTILS_H
