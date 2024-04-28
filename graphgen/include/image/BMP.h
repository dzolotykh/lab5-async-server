#ifndef BMP_H
#define BMP_H

#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>

class BMP {
   public:
    BMP(size_t _width, size_t _height);

    // saves image in specified path
    void save(const std::string& path) const noexcept;

    // sets pixel in point (x, y) in color (r, g, b)
    void draw_pixel(size_t x, size_t y, u_char r = 0, u_char g = 0, u_char b = 0) noexcept;

    // draws black filled circle in point (x, y) with radius r
    void draw_circle(int x, int y, int r) noexcept;

    // draws black segment from (x1, y1) to (x2, y2)
    void draw_segment(int x1, int y1, int x2, int y2) noexcept;

    // draws any not negative number in point (x, y)
    void draw_number(size_t x, size_t y, int number, size_t scale);

   private:
    std::vector<uint8_t> pixels;
    const size_t width, height;

    const size_t between_digits_offset = 10;

    inline size_t get_pos(size_t x, size_t y) const noexcept { return (x * height + y) * 3; }

    struct Header {
        std::string encoding_type = "BM";
        int file_size;
        size_t width;
        size_t height;
        std::string reserved = "\0\0\0\0";
        int data_offset = 54;
        int header_size = 40;
        short planes = 1;
        short bpp = 24;
        int compression = 0;
        int data_size;
        int h_resolution = 0;
        int v_resolution = 0;
        int colors = 0;
        int importantColors = 0;

        Header(size_t _width, size_t _height)
            : width(_width),
              height(_height),
              data_size(3 * _width * _height),
              file_size(54 + 3 * _width * _height) {}

        std::vector<std::pair<const char*, int>> get_bytes() const noexcept {
            std::vector<std::pair<const char*, int>> bytes_arr;

            bytes_arr.emplace_back(reinterpret_cast<const char*>(&encoding_type), 2);
            bytes_arr.emplace_back(reinterpret_cast<const char*>(&file_size), 4);
            bytes_arr.emplace_back(reinterpret_cast<const char*>(&reserved), 4);
            bytes_arr.emplace_back(reinterpret_cast<const char*>(&data_offset), 4);
            bytes_arr.emplace_back(reinterpret_cast<const char*>(&header_size), 4);
            bytes_arr.emplace_back(reinterpret_cast<const char*>(&width), 4);
            bytes_arr.emplace_back(reinterpret_cast<const char*>(&height), 4);
            bytes_arr.emplace_back(reinterpret_cast<const char*>(&planes), 2);
            bytes_arr.emplace_back(reinterpret_cast<const char*>(&bpp), 2);
            bytes_arr.emplace_back(reinterpret_cast<const char*>(&compression), 4);
            bytes_arr.emplace_back(reinterpret_cast<const char*>(&data_size), 4);
            bytes_arr.emplace_back(reinterpret_cast<const char*>(&h_resolution), 4);
            bytes_arr.emplace_back(reinterpret_cast<const char*>(&v_resolution), 4);
            bytes_arr.emplace_back(reinterpret_cast<const char*>(&colors), 4);
            bytes_arr.emplace_back(reinterpret_cast<const char*>(&importantColors), 4);

            return bytes_arr;
        }
    };
};

#endif  // BMP_H
