#ifndef IMAGEAOS_HPP
#define IMAGEAOS_HPP

#include "color.hpp"
#include "imageinfo.hpp"
#include "progargs.hpp"
#include <vector>
#include <unordered_map>
#include <cstdint>

/********************************************* MAX LEVEL AOS *********************************************/
// Cambiar los componentes del color según la intensidad
Color simpleToSimpleChangeLevel(Color inputColor, const uint32_t antiguoMaxLevel, uint16_t maxLevel);

// Cambiar los colores al cambiar la intensidad de la imagen
std::vector<Color> modifyMaxLevelInputPixels(const std::vector<Color>& inputColores, const uint32_t antiguoMaxLevel, uint16_t maxLevel);

// Función principal para el escalado de intensidad en formato AOS
void processMaxLevel(const FilePaths& paths, uint16_t maxLevel);
/********************************************************************************************************/

/********************************************* COMPRESS AOS *********************************************/
// Extraer los píxeles de la imagen a partir de los datos binarios (fileData)
std::vector<Color> extractImagePixels(const std::vector<uint8_t>& fileData, const PPMHeader& header);

// Genera una tabla de colores únicos (colorTable) y un mapa de índices (colorIndex)
std::pair<std::vector<Color>, std::unordered_map<Color, int>> createColorTable(const std::vector<Color>& imagePixels);

// Añade la tabla de colores al archivo comprimido
void appendColorTable(std::vector<uint8_t>& compressedData, const std::vector<Color>& colorTable, const PPMHeader& header);

// Añade los índices de los píxeles al archivo comprimido
void appendPixelIndices(std::vector<uint8_t>& compressedData, const std::vector<Color>& imagePixels, const std::unordered_map<Color, int>& colorIndex);

// Función principal para comprimir una imagen en formato AOS
void compressAoS(const FilePaths& paths);
/********************************************************************************************************/

// Otras funciones para 'resize', 'cutfreq', etc.
// Define Pixel and Image types here
struct Pixel {
  uint8_t r, g, b;
};
using Image = std::vector<std::vector<Pixel>>;

// Function declarations
Image vectorToImage(const std::vector<uint8_t>& data, int width, int height, int channels);
std::vector<uint8_t> imageToVector(const Image& image, int channels);
Image resizeImageAoS(const Image& image, int newWidth, int newHeight);

class ImageAOS {
    public:
    ImageAOS(int width, int height, const std::vector<uint8_t>& pixels);

    void resize(int newWidth, int newHeight);

    private:
    int width;
    int height;
    std::vector<uint8_t> pixels; // Almacena los píxeles en formato RGB
};


#endif // IMAGEAOS_HPP