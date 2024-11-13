#include "color.hpp"
#include <unordered_map>
#include <iostream>

constexpr uint16_t BYTE_MASK = 0xFF;
constexpr uint8_t BITS_PER_BYTE = 8;
constexpr uint16_t MAX_COLOR_VALUE_8BIT = 255;
constexpr size_t COLOR_TABLE_RESERVE_SIZE = 256;
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

void ColorChannels::extractFromBinaryWithFrequency(
    const std::vector<uint8_t>& fileData, 
    const PPMHeader& header, 
    std::unordered_map<uint32_t, int, HashColor>& colorFrequency) 
{
    // Asegurarse de que el tamaño del archivo es consistente con las dimensiones de la imagen
    const size_t totalPixels = static_cast<size_t>(header.width) * static_cast<size_t>(header.height);
    if (fileData.size() < totalPixels * 3) {
        std::cerr << "Error: Tamaño de datos inconsistente con las dimensiones de la imagen\n";
        return;
    }

    // Redimensionar los canales solo si no se ha hecho antes
    redChannel.resize(totalPixels);
    greenChannel.resize(totalPixels);
    blueChannel.resize(totalPixels);

    // Reservar espacio en el unordered_map para mejorar el rendimiento
    colorFrequency.reserve(totalPixels);

    // Llenar cada canal con los valores correspondientes y calcular la frecuencia de cada color
    for (size_t i = 0, j = 0; i < totalPixels; ++i, j += 3) {
        uint16_t red = fileData[j];
        uint16_t green = fileData[j + 1];
        uint16_t blue = fileData[j + 2];

        // Almacenar en los canales
        redChannel[i] = red;
        greenChannel[i] = green;
        blueChannel[i] = blue;

        // Combinar los valores RGB en un solo valor uint32_t
        uint32_t combinedColor = (static_cast<uint32_t>(red) << 16) | 
                                 (static_cast<uint32_t>(green) << 8) | 
                                 static_cast<uint32_t>(blue);

        // Actualizar la frecuencia del color en el mapa
        colorFrequency[combinedColor]++;
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
