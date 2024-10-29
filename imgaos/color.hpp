#ifndef COLOR_HPP
#define COLOR_HPP

#include <vector>
#include <string>
#include <iostream>
#include <functional>  // Necesario para std::hash
#include <cstdint>

struct Color {
    uint8_t red;
    uint8_t green;
    uint8_t blue;

    Color() : red(0), green(0), blue(0) {}
    Color(uint8_t r, uint8_t g, uint8_t b) : red(r), green(g), blue(b) {}

    // Comparador de igualdad para usar Color como clave en unordered_map
    bool operator==(const Color& other) const {
        return red == other.red && green == other.green && blue == other.blue;
    }
    bool operator!=(const Color& other) const {
        return red != other.red || green != other.green || blue != other.blue;
    }
    
    // Función para imprimir el color (útil para depuración)
    void print() const {
        std::cout << "R: " << static_cast<int>(red)
                  << " G: " << static_cast<int>(green)
                  << " B: " << static_cast<int>(blue) << std::endl;
    }
};

inline std::ostream& operator<<(std::ostream& os, const Color& color) {
    os << "(" << static_cast<int>(color.red) << ", "
       << static_cast<int>(color.green) << ", "
       << static_cast<int>(color.blue) << ")";
    return os;
}

// Especialización de std::hash para la estructura Color
/*namespace std {
    template <>
    struct hash<Color> {
        size_t operator()(const Color& color) const {
            return ((std::hash<uint8_t>()(color.red) ^
                     (std::hash<uint8_t>()(color.green) << 1)) >> 1) ^
                     (std::hash<uint8_t>()(color.blue) << 1);
        }
    };
}*/

namespace std {
    template <>
    struct hash<Color> {
        size_t operator()(const Color& color) const {
            return (static_cast<size_t>(color.red) << 16) | (static_cast<size_t>(color.green) << 8) | static_cast<size_t>(color.blue);
        }
    };
}

class ColorPalette {
    public:
    void addColor(const Color& color);
    Color getColor(size_t index) const;
    size_t size() const;
    void printPalette() const;

    private:
    std::vector<Color> colors;
};

#endif // COLOR_HPP
