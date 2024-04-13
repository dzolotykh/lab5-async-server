#include "StringUtils.h"

std::random_device StringUtils::rd = std::random_device();
std::mt19937 StringUtils::gen = std::mt19937(StringUtils::rd());

std::string StringUtils::random_string(size_t length) {
    std::string str;
    str.reserve(length);
    for (size_t i = 0; i < length; ++i) {
        str.push_back(numbers_alph[gen() % numbers_alph_size]);
    }
    return str;
}