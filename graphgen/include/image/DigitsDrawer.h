#ifndef DIGITSDRAWER_H
#define DIGITSDRAWER_H

#include <../resources/resourses.h>
#include <algorithm>
#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>

/* Class for drawing single digits. Uses functors for drawing pixels,
 * because of this it can be used for any images classes,
 * that provide a function for drawing pixel in pos (x, y)
 */
class DigitsDrawer {
   public:
    // throws exception if the specified number is not a digit
    template <typename T_draw_pixel>
    static void draw_digit(size_t x, size_t y, int digit, T_draw_pixel draw_pixel, size_t scale) {
        if (digit < 0 || digit > 9)
            throw std::runtime_error("digit should be in segment [0..9], but provided " +
                                     std::to_string(digit));

        draw_from_file(x, y, get_path(digit), draw_pixel, scale);
    }

   private:
    template <typename T_draw_pixel>
    static void draw_from_file(size_t x, size_t y, const std::string &filepath,
                               T_draw_pixel draw_pixel, size_t scale) {
        std::ifstream f(filepath);
        if (!f.is_open()) {
            throw std::runtime_error("could not open file for drawing with path: " + filepath);
        }
        std::vector<std::string> lines;
        std::string buff;
        while (std::getline(f, buff)) {
            lines.push_back(buff);
        }
        f.close();
        std::reverse(lines.begin(), lines.end());
        for (size_t i = 0; i < lines.size(); ++i) {
            for (size_t j = 0; j < lines[i].size(); ++j) {
                if (lines[i][j] == '#') {
                    for (size_t add_x = 0; add_x < scale; ++add_x) {
                        for (size_t add_y = 0; add_y < scale; ++add_y) {
                            draw_pixel(x + i * scale + add_x, y + j * scale + add_y);
                        }
                    }
                }
            }
        }
    }
};

#endif    // DIGITSDRAWER_H
