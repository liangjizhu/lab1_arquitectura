#include "color.hpp"
// #include <array>
#include <span>

// Constantes descriptivas
constexpr uint16_t BYTE_MASK = 0xFF;
constexpr uint16_t MAX_COLOR_VALUE_8BIT = 255;
constexpr uint16_t MAX_COLOR_VALUE_16BIT = 65535;
constexpr size_t COLOR_DATA_SIZE_16BIT = 6;
constexpr uint8_t BITS_PER_BYTE = 8; // Nueva constante para los bits por byte
constexpr uint8_t COLOR_COMPONENT_INDEX_BLUE_HIGH = 5; // Nueva constante para el índice del byte alto de azul (para el clang-tidy)

// Constructor para inicializar colores
Color::Color(RGBColor color) noexcept : rgb(color) {}

// Cargar color desde datos binarios según maxColorValue en header
Color Color::fromBinary(const uint8_t* data, const PPMHeader& header) noexcept {
    const std::span<const uint8_t> colorData(data, COLOR_DATA_SIZE_16BIT);

    if (header.maxColorValue > MAX_COLOR_VALUE_8BIT) {
        return Color({
            static_cast<uint16_t>(colorData[0] | (colorData[1] << BITS_PER_BYTE)),
            static_cast<uint16_t>(colorData[2] | (colorData[3] << BITS_PER_BYTE)),
            static_cast<uint16_t>(colorData[4] | (colorData[COLOR_COMPONENT_INDEX_BLUE_HIGH] << BITS_PER_BYTE))
        });
    }
    return Color({colorData[0], colorData[1], colorData[2]});
}

// Escribir color a datos binarios según maxColorValue en header
//void Color::writeToBinaryMod(std::vector<uint8_t>& buffer, const PPMHeader& header) const {
    //auto append16Bits = [&buffer](uint16_t value) {
        //buffer.push_back(static_cast<uint8_t>(value & BYTE_MASK));
        //buffer.push_back(static_cast<uint8_t>(value >> BITS_PER_BYTE));
    //};

    //if (header.maxColorValue > MAX_COLOR_VALUE_8BIT) {
        //append16Bits(rgb.red);
        //append16Bits(rgb.green);
        //append16Bits(rgb.blue);
    //} else {
        //buffer.push_back(static_cast<uint8_t>(rgb.red & BYTE_MASK));
        //buffer.push_back(static_cast<uint8_t>(rgb.green & BYTE_MASK));
        //buffer.push_back(static_cast<uint8_t>(rgb.blue & BYTE_MASK));
    //}
//}

// Escribir color a datos binarios según maxColorValue en header
void Color::writeToBinary(std::vector<uint8_t>& buffer, const PPMHeader& header) const {
    auto append16Bits = [&buffer](uint16_t value) {
        buffer.push_back(static_cast<uint8_t>(value & BYTE_MASK));
        buffer.push_back(static_cast<uint8_t>(value >> BITS_PER_BYTE));
    };

    if (header.maxColorValue > MAX_COLOR_VALUE_16BIT) {
        for (auto value : {rgb.red, rgb.green, rgb.blue}) {
            buffer.push_back(static_cast<uint8_t>(value & BYTE_MASK));
            buffer.push_back(static_cast<uint8_t>((value >> BITS_PER_BYTE) & BYTE_MASK));
            buffer.push_back(static_cast<uint8_t>((value >> (2 * BITS_PER_BYTE)) & BYTE_MASK));
            buffer.push_back(static_cast<uint8_t>((value >> (3 * BITS_PER_BYTE)) & BYTE_MASK));
        }
    } else if (header.maxColorValue > MAX_COLOR_VALUE_8BIT) {
        append16Bits(rgb.red);
        append16Bits(rgb.green);
        append16Bits(rgb.blue);
    } else {
        buffer.push_back(static_cast<uint8_t>(rgb.red & BYTE_MASK));
        buffer.push_back(static_cast<uint8_t>(rgb.green & BYTE_MASK));
        buffer.push_back(static_cast<uint8_t>(rgb.blue & BYTE_MASK));
    }
}

// Comparador de igualdad para usar Color como clave en unordered_map
bool Color::operator==(const Color& other) const noexcept {
    return rgb.red == other.rgb.red && rgb.green == other.rgb.green && rgb.blue == other.rgb.blue;
}

// Especialización de std::hash para la estructura Color
namespace std {
    size_t hash<Color>::operator()(const Color& color) const noexcept {
        size_t hashValue = std::hash<uint16_t>()(color.rgb.red);
        hashValue ^= std::hash<uint16_t>()(color.rgb.green) << 1;
        hashValue ^= std::hash<uint16_t>()(color.rgb.blue) << 2;
        return hashValue;
    }
}
