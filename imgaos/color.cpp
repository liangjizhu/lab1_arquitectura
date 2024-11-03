#include "color.hpp"

// Constructor para inicializar colores
Color::Color(uint16_t r, uint16_t g, uint16_t b) : red(r), green(g), blue(b) {}

// Cargar color desde datos binarios según maxColorValue en header
Color Color::fromBinary(const uint8_t* data, const PPMHeader& header) {
    if (header.maxColorValue > 255) {
        // Cargar como 6 bytes (16 bits por componente en orden RGB)
        return Color(
            data[0] | (data[1] << 8),  // Red
            data[2] | (data[3] << 8),  // Green
            data[4] | (data[5] << 8)   // Blue
        );
    } else {
        // Cargar como 3 bytes (8 bits por componente en orden RGB)
        return Color(data[0], data[1], data[2]);
    }
}

// Escribir color a datos binarios según maxColorValue en header
void Color::writeToBinary(std::vector<uint8_t>& buffer, const PPMHeader& header) const {
    if (header.maxColorValue > 65535) {
        // Guardar como 12 bytes (32 bits por componente)
        buffer.push_back(static_cast<uint8_t>(red & 0xFF));
        buffer.push_back(static_cast<uint8_t>((red >> 8) & 0xFF));
        buffer.push_back(static_cast<uint8_t>((red >> 16) & 0xFF));
        buffer.push_back(static_cast<uint8_t>((red >> 24) & 0xFF));

        buffer.push_back(static_cast<uint8_t>(green & 0xFF));
        buffer.push_back(static_cast<uint8_t>((green >> 8) & 0xFF));
        buffer.push_back(static_cast<uint8_t>((green >> 16) & 0xFF));
        buffer.push_back(static_cast<uint8_t>((green >> 24) & 0xFF));

        buffer.push_back(static_cast<uint8_t>(blue & 0xFF));
        buffer.push_back(static_cast<uint8_t>((blue >> 8) & 0xFF));
        buffer.push_back(static_cast<uint8_t>((blue >> 16) & 0xFF));
        buffer.push_back(static_cast<uint8_t>((blue >> 24) & 0xFF));
    } else if (header.maxColorValue > 255) {
        // Guardar como 6 bytes (16 bits por componente en orden RGB)
        buffer.push_back(static_cast<uint8_t>(red & 0xFF));
        buffer.push_back(static_cast<uint8_t>((red >> 8) & 0xFF));

        buffer.push_back(static_cast<uint8_t>(green & 0xFF));
        buffer.push_back(static_cast<uint8_t>((green >> 8) & 0xFF));

        buffer.push_back(static_cast<uint8_t>(blue & 0xFF));
        buffer.push_back(static_cast<uint8_t>((blue >> 8) & 0xFF));
    } else {
        // Guardar como 3 bytes (8 bits por componente en orden RGB)
        buffer.push_back(static_cast<uint8_t>(red));
        buffer.push_back(static_cast<uint8_t>(green));
        buffer.push_back(static_cast<uint8_t>(blue));
    }
}

// Comparador de igualdad para usar Color como clave en unordered_map
bool Color::operator==(const Color& other) const {
    return red == other.red && green == other.green && blue == other.blue;
}

// Función para imprimir el color (útil para depuración)
void Color::print() const {
    std::cout << "R: " << red << " G: " << green << " B: " << blue << std::endl;
}

// Especialización de std::hash para la estructura Color
namespace std {
    size_t hash<Color>::operator()(const Color& color) const {
        return ((std::hash<uint16_t>()(color.red) ^
                 (std::hash<uint16_t>()(color.green) << 1)) >> 1) ^
                 (std::hash<uint16_t>()(color.blue) << 1);
    }
}
