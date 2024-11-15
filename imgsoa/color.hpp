#ifndef COLOR_HPP
#define COLOR_HPP

#include "imageinfo.hpp"
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <cstddef>

struct HashTuple {
    size_t operator()(const std::tuple<uint16_t, uint16_t, uint16_t>& colorTuple) const {
        // Obtener los tres elementos de la tupla
        const uint16_t& red = std::get<0>(colorTuple);
        const uint16_t& green = std::get<1>(colorTuple);
        const uint16_t& blue = std::get<2>(colorTuple);

        // Generar un hash combinando los tres componentes (red, green, blue)
        size_t const hashRed = std::hash<uint16_t>{}(red);
        size_t const hashGreen = std::hash<uint16_t>{}(green);
        size_t const hashBlue = std::hash<uint16_t>{}(blue);

        // Combinamos los hashes de los tres valores
        return hashRed ^ (hashGreen << 1) ^ (hashBlue << 2);
    }
};

struct HashColor {
    size_t operator()(uint32_t colorValue) const {
        // Usamos un simple hash de los bits de color
        return std::hash<uint32_t>()(colorValue);
    }
};

// Clase para manejar los canales de color en SoA
class ColorChannels {
public:
    // Métodos existentes para obtener referencias constantes
    [[nodiscard]] const std::vector<uint16_t>& getRedChannel() const { return redChannel; }
    [[nodiscard]] const std::vector<uint16_t>& getGreenChannel() const { return greenChannel; }
    [[nodiscard]] const std::vector<uint16_t>& getBlueChannel() const { return blueChannel; }

    // Métodos para obtener referencias mutables
    [[nodiscard]] std::vector<uint16_t>& getRedChannel() { return redChannel; }
    [[nodiscard]] std::vector<uint16_t>& getGreenChannel() { return greenChannel; }
    [[nodiscard]] std::vector<uint16_t>& getBlueChannel() { return blueChannel; }

    // Otros métodos y constructores
    void reserve(size_t channelSize) {
        redChannel.reserve(channelSize);
        greenChannel.reserve(channelSize);
        blueChannel.reserve(channelSize);
    }

    ColorChannels(size_t channelSize);

    // Métodos para extraer y escribir datos binarios
    void extractFromBinary(const std::vector<uint8_t>& imageData, const PPMHeader& header);
    void writeToBinary(std::vector<uint8_t>& outputBuffer, const PPMHeader& header) const;
    void extractFromBinaryWithFrequency(
        const std::vector<uint8_t>& imageData,
        const PPMHeader& header,
        std::unordered_map<uint32_t, int, HashColor>& colorFrequencyMap);

    // Método para comparar colores
    [[nodiscard]] bool areEqual(size_t indexA, size_t indexB) const;

    // Tamaño de los canales
    [[nodiscard]] size_t size() const;

private:
    std::vector<uint16_t> redChannel;
    std::vector<uint16_t> greenChannel;
    std::vector<uint16_t> blueChannel;
};

#endif // COLOR_HPP
