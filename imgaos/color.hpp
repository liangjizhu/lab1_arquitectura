#ifndef COLOR_HPP
#define COLOR_HPP

#include <vector>
#include <string>
#include <iostream>
#include <functional>
#include <cstdint>
#include "imageinfo.hpp"  // Para usar PPMHeader y leer maxColorValue

// Estructura para representar un color en formato RGB
struct Color {
    uint16_t red;
    uint16_t green;
    uint16_t blue;

    // Constructor para inicializar colores
    Color(uint16_t r, uint16_t g, uint16_t b);

    // Cargar color desde datos binarios según maxColorValue en header
    static Color fromBinary(const uint8_t* data, const PPMHeader& header);

    // Escribir color a datos binarios según maxColorValue en header
    void writeToBinary(std::vector<uint8_t>& buffer, const PPMHeader& header) const;

    // Comparador de igualdad para usar Color como clave en unordered_map
    bool operator==(const Color& other) const;

    // Función para imprimir el color (útil para depuración)
    void print() const;
};

// Especialización de std::hash para la estructura Color
namespace std {
    template <>
    struct hash<Color> {
        size_t operator()(const Color& color) const;
    };
}

#endif // COLOR_HPP
