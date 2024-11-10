#include "color.hpp"
// #include <algorithm>

constexpr uint16_t BYTE_MASK = 0xFF;
constexpr uint8_t BITS_PER_BYTE = 8;
constexpr uint16_t MAX_COLOR_VALUE_8BIT = 255;
constexpr uint8_t COLOR_COMPONENT_INDEX_BLUE_HIGH = 5; // Nueva constante para el índice del byte alto de azul (para el clang-tidy)

// Constructor para inicializar los canales
ColorChannels::ColorChannels(size_t size) {
    redChannel.reserve(size);
    greenChannel.reserve(size);
    blueChannel.reserve(size);
}

// Extraer datos binarios a canales
void ColorChannels::extractFromBinary(const std::vector<uint8_t>& fileData, const PPMHeader& header) {
    const size_t numPixels = static_cast<size_t>(header.width) * static_cast<size_t>(header.height);
    const size_t pixelSize = (header.maxColorValue > MAX_COLOR_VALUE_8BIT) ? 6 : 3;
    const size_t pixelStart = fileData.size() - numPixels * pixelSize;

    redChannel.resize(numPixels);
    greenChannel.resize(numPixels);
    blueChannel.resize(numPixels);

    for (size_t i = 0, j = pixelStart; i < numPixels; ++i, j += pixelSize) {
        if (header.maxColorValue > MAX_COLOR_VALUE_8BIT) {
            redChannel[i] = static_cast<uint16_t>(fileData[j] | (fileData[j + 1] << BITS_PER_BYTE));
            greenChannel[i] = static_cast<uint16_t>(fileData[j + 2] | (fileData[j + 3] << BITS_PER_BYTE));
            blueChannel[i] = static_cast<uint16_t>(fileData[j + 4] | (fileData[j + COLOR_COMPONENT_INDEX_BLUE_HIGH] << BITS_PER_BYTE));
        } else {
            redChannel[i] = fileData[j];
            greenChannel[i] = fileData[j + 1];
            blueChannel[i] = fileData[j + 2];
        }
    }
}

// Escribir datos binarios desde canales
void ColorChannels::writeToBinary(std::vector<uint8_t>& buffer, const PPMHeader& header) const {
    auto append16Bits = [&buffer](uint16_t value) {
        buffer.push_back(static_cast<uint8_t>(value & BYTE_MASK));
        buffer.push_back(static_cast<uint8_t>(value >> BITS_PER_BYTE));
    };

    for (size_t i = 0; i < redChannel.size(); ++i) {
        if (header.maxColorValue > MAX_COLOR_VALUE_8BIT) {
            append16Bits(redChannel[i]);
            append16Bits(greenChannel[i]);
            append16Bits(blueChannel[i]);
        } else {
            buffer.push_back(static_cast<uint8_t>(redChannel[i] & BYTE_MASK));
            buffer.push_back(static_cast<uint8_t>(greenChannel[i] & BYTE_MASK));
            buffer.push_back(static_cast<uint8_t>(blueChannel[i] & BYTE_MASK));
        }
    }
}

// Comparar colores en los índices dados
bool ColorChannels::areEqual(size_t index1, size_t index2) const {
    return redChannel[index1] == redChannel[index2] &&
           greenChannel[index1] == greenChannel[index2] &&
           blueChannel[index1] == blueChannel[index2];
}

// Obtener el tamaño de los canales
size_t ColorChannels::size() const {
    return redChannel.size();
}