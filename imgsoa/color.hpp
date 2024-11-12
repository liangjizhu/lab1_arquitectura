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

namespace std {
    template <>
    struct hash<std::tuple<uint16_t, uint16_t, uint16_t>> {
        size_t operator()(const std::tuple<uint16_t, uint16_t, uint16_t>& color) const noexcept {
            size_t h1 = std::hash<uint16_t>{}(std::get<0>(color));
            size_t h2 = std::hash<uint16_t>{}(std::get<1>(color));
            size_t h3 = std::hash<uint16_t>{}(std::get<2>(color));
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };
}


/*struct Color {
    uint8_t red;
    uint8_t green;
    uint8_t blue;

    // Constructor por defecto
    Color() : red(0), green(0), blue(0) {}
    
    // Constructor con valores específicos
    Color(uint8_t r, uint8_t g, uint8_t b) : red(r), green(g), blue(b) {}

    // Permite la conversión a tupla para facilitar la comparación y acceso
    std::tuple<uint8_t, uint8_t, uint8_t> toTuple() const {
        return std::make_tuple(red, green, blue);
    }

    // Operadores de comparación
    bool operator==(const Color& other) const {
        return red == other.red && green == other.green && blue == other.blue;
    }

    bool operator!=(const Color& other) const {
        return red != other.red || green != other.green || blue != other.blue;
    }
};

// Clase Palette que usa SOA (Structure of Arrays)
class ColorPalette {
public:
    // Agregar un color al arreglo
    void addColor(uint8_t red, uint8_t green, uint8_t blue) {
        reds.push_back(red);
        greens.push_back(green);
        blues.push_back(blue);
    }

    // Obtener un color dado un índice
    [[nodiscard]] Color getColor(size_t index) const {
        return Color(reds[index], greens[index], blues[index]);
    }

    // Obtener el tamaño de la paleta
    [[nodiscard]] size_t size() const {
        return reds.size();
    }

    // Imprimir todos los colores de la paleta
    void printPalette() const {
        for (size_t i = 0; i < size(); ++i) {
            std::cout << "Color " << i + 1 << ": ("
                      << (int)reds[i] << ", "
                      << (int)greens[i] << ", "
                      << (int)blues[i] << ")\n";
        }
    }

private:
    // Vectores de los componentes de los colores (rojo, verde, azul)
    std::vector<uint8_t> reds;
    std::vector<uint8_t> greens;
    std::vector<uint8_t> blues;
};

// Especialización de hash para la estructura Color
namespace std {
    template <>
    struct hash<Color> {
        size_t operator()(const Color& color) const {
            return ((std::hash<uint8_t>()(color.red) ^ (std::hash<uint8_t>()(color.green) << 1)) >> 1) ^ (std::hash<uint8_t>()(color.blue) << 1);
        }
    };
}

// Especialización de hash para la tupla de tres uint8_t (Color)
namespace std {
    template <>
    struct hash<std::tuple<uint8_t, uint8_t, uint8_t>> {
        size_t operator()(const std::tuple<uint8_t, uint8_t, uint8_t>& color) const {
            auto [r, g, b] = color;
            return ((std::hash<uint8_t>()(r) ^ (std::hash<uint8_t>()(g) << 1)) >> 1) ^ (std::hash<uint8_t>()(b) << 1);
        }
    };
}*/

#endif // COLOR_HPP
