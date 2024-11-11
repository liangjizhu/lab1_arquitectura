#ifndef COLOR_HPP
#define COLOR_HPP

#include <vector>
// #include <array>
#include <cstdint>
#include <functional>
#include "imageinfo.hpp"  // Para usar PPMHeader y leer maxColorValue

// Estructura para representar un color en formato RGB
struct RGBColor {
    uint16_t red;
    uint16_t green;
    uint16_t blue;
};

class Color {
    public:
    RGBColor rgb;

    // Constructor para inicializar colores
    explicit Color(RGBColor color) noexcept;
    Color() : rgb{0, 0, 0} {} // Default constructor
    // Cargar color desde datos binarios según maxColorValue en header
    [[nodiscard]] static Color fromBinary(const uint8_t* data, const PPMHeader& header) noexcept;

    // Escribir color a datos binarios según maxColorValue en header
    void writeToBinary(std::vector<uint8_t>& buffer, const PPMHeader& header) const;

    // Comparador de igualdad para usar Color como clave en unordered_map
    [[nodiscard]] bool operator==(const Color& other) const noexcept;
};

// Especialización de std::hash para la clase Color
template <>
struct std::hash<Color> {
    size_t operator()(const Color& color) const noexcept;
};

#endif // COLOR_HPP
