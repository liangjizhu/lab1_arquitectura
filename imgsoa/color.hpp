#ifndef COLOR_HPP
#define COLOR_HPP

#include "imageinfo.hpp"

#include <vector>
#include <cstdint>
#include <cstddef>

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
