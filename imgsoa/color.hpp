//
// Created by liang on 27/10/24.
//

#ifndef COLOR_HPP
#define COLOR_HPP

#include <vector>
#include <string>
#include <iostream>
#include <functional>  // Necesario para std::hash
#include <cstdint>

class ColorPalette {
  public:
  void addColor(uint8_t red, uint8_t green, uint8_t blue);
  [[nodiscard]] std::tuple<uint8_t, uint8_t, uint8_t> getColor(size_t index) const;
  [[nodiscard]] size_t size() const;
  void printPalette() const;

  private:
  std::vector<uint8_t> reds;
  std::vector<uint8_t> greens;
  std::vector<uint8_t> blues;
};



struct Color {
    uint8_t red;
    uint8_t green;
    uint8_t blue;

    // Constructor
    Color() : red(0), green(0), blue(0) {} // Constructor por defecto
    Color(uint8_t r, uint8_t g, uint8_t b) : red(r), green(g), blue(b) {}

    // Permite la conversión a tupla para facilitar la comparación y acceso
    std::tuple<uint8_t, uint8_t, uint8_t> toTuple() const {
        return std::make_tuple(red, green, blue);
    }

    bool operator==(const Color& other) const {
        return red == other.red && green == other.green && blue == other.blue;
    }
    bool operator!=(const Color& other) const {
        return red != other.red || green != other.green || blue != other.blue;
    }
};

namespace std {
    template <>
    struct hash<Color> {
        size_t operator()(const Color& color) const {
            return ((std::hash<uint8_t>()(color.red) ^ (std::hash<uint8_t>()(color.green) << 1)) >> 1) ^ (std::hash<uint8_t>()(color.blue) << 1);
        }
    };
}

namespace std {
    template <>
    struct hash<std::tuple<uint8_t, uint8_t, uint8_t>> {
        size_t operator()(const std::tuple<uint8_t, uint8_t, uint8_t>& color) const {
            auto [r, g, b] = color;
            return ((std::hash<uint8_t>()(r) ^ (std::hash<uint8_t>()(g) << 1)) >> 1) ^ (std::hash<uint8_t>()(b) << 1);
        }
    };
}
#endif // COLOR_HPP