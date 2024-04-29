#include <image/BMP.h>
#include <image/DigitsDrawer.h>
#include <algorithm>

BMP::BMP(size_t _width, size_t _height)
    : width(_width), height(_height), pixels(_width * _height * 3) {
    std::fill(pixels.begin(), pixels.end(), 255);
}

// saves image in specified path
void BMP::save(const std::string& path) const noexcept {
    std::ofstream outputFile(path, std::ios::binary);
    for (auto i : Header(width, height).get_bytes()) {
        outputFile.write(i.first, i.second);
    }
    outputFile.write(reinterpret_cast<const char*>(pixels.data()), Header(width, height).data_size);
    outputFile.close();
};

// sets pixel in point (x, y) in color (r, g, b)
void BMP::draw_pixel(size_t x, size_t y, u_char r, u_char g, u_char b) noexcept {
    if (get_pos(x, y) + 2 >= pixels.size())
        return;
    pixels[get_pos(x, y)] = r;
    pixels[get_pos(x, y) + 1] = g;
    pixels[get_pos(x, y) + 2] = b;
}

// draws black segment from (x1, y1) to (x2, y2)
void BMP::draw_segment(int x1, int y1, int x2, int y2) noexcept {
    int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
    dx = x2 - x1;
    dy = y2 - y1;
    dx1 = std::abs(dx);
    dy1 = std::abs(dy);
    px = 2 * dy1 - dx1;
    py = 2 * dx1 - dy1;
    if (dy1 <= dx1) {
        if (dx >= 0) {
            x = x1;
            y = y1;
            xe = x2;
        } else {
            x = x2;
            y = y2;
            xe = x1;
        }
        draw_pixel(x, y);
        for (i = 0; x < xe; i++) {
            x = x + 1;
            if (px < 0) {
                px = px + 2 * dy1;
            } else {
                if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
                    y = y + 1;
                } else {
                    y = y - 1;
                }
                px = px + 2 * (dy1 - dx1);
            }
            draw_pixel(x, y);
        }
    } else {
        if (dy >= 0) {
            x = x1;
            y = y1;
            ye = y2;
        } else {
            x = x2;
            y = y2;
            ye = y1;
        }
        draw_pixel(x, y);
        for (i = 0; y < ye; i++) {
            y = y + 1;
            if (py <= 0) {
                py = py + 2 * dx1;
            } else {
                if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
                    x = x + 1;
                } else {
                    x = x - 1;
                }
                py = py + 2 * (dx1 - dy1);
            }
            draw_pixel(x, y);
        }
    }
}

// draws black filled circle in point (x, y) with radius r
void BMP::draw_circle(int x, int y, int r) noexcept {
    for (int x_i = x - r; x_i <= x + r; x_i++) {
        for (int y_i = y - r; y_i <= y + r; y_i++) {
            if ((x - x_i) * (x - x_i) + (y - y_i) * (y - y_i) < r * r) {
                draw_pixel(x_i, y_i);
            }
        }
    }
}

// draws any not negative number in point (x, y)
void BMP::draw_number(size_t x, size_t y, int number, size_t scale) {
    std::vector<int> digits;
    if (number == 0) {
        digits.push_back(0);
    }
    while (number) {
        digits.push_back(number % 10);
        number /= 10;
    }
    std::reverse(digits.begin(), digits.end());
    size_t add = 0;
    for (auto digit : digits) {
        DigitsDrawer::draw_digit(
            x, y + add, digit, [this](size_t x, size_t y) { draw_pixel(x, y); }, scale);
        add += between_digits_offset * scale;
    }
}
