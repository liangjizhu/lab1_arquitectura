#ifndef COLOR_HPP
#define COLOR_HPP

#include "imageinfo.hpp"
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <cstddef>


struct HashTuple {
    size_t operator()(const std::tuple<uint16_t, uint16_t, uint16_t>& t) const {
        // Obtener los tres elementos de la tupla
        const uint16_t& r = std::get<0>(t);
        const uint16_t& g = std::get<1>(t);
        const uint16_t& b = std::get<2>(t);

        // Generar un hash combinando los tres componentes (r, g, b)
        size_t h1 = std::hash<uint16_t>{}(r);
        size_t h2 = std::hash<uint16_t>{}(g);
        size_t h3 = std::hash<uint16_t>{}(b);

        // Combinamos los hashes de los tres valores
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

struct HashColor {
    size_t operator()(uint32_t color) const {
        // Usamos un simple hash de los bits de color
        return std::hash<uint32_t>()(color);
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
    void reserve(size_t size) {
      redChannel.reserve(size);
      greenChannel.reserve(size);
      blueChannel.reserve(size);
    }
    ColorChannels(size_t size);

    // Métodos para extraer y escribir datos binarios
    void extractFromBinary(const std::vector<uint8_t>& fileData, const PPMHeader& header);
    void writeToBinary(std::vector<uint8_t>& buffer, const PPMHeader& header) const;
    void extractFromBinaryWithFrequency(
    const std::vector<uint8_t>& fileData, 
    const PPMHeader& header, 
    std::unordered_map<uint32_t, int, HashColor>& colorFrequency);
    // Metodo para comparar colores
    [[nodiscard]] bool areEqual(size_t index1, size_t index2) const;


    // Tamaño de los canales
    [[nodiscard]] size_t size() const;

    private:
    std::vector<uint16_t> redChannel;
    std::vector<uint16_t> greenChannel;
    std::vector<uint16_t> blueChannel;
};

#endif // COLOR_HPP
