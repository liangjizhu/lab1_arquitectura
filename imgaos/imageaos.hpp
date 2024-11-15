#ifndef IMAGEAOS_HPP
#define IMAGEAOS_HPP

#include "color.hpp"
#include "imageinfo.hpp"
#include "progargs.hpp"
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>
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

/********************************************* CUTFREQ AOS *********************************************/
std::vector<Color> encontrar_colores_menos_frecuentes(const std::unordered_map<Color, int, HashColor>& frecuencia, int n);

std::vector<Color> encontrar_colores_menos_frecuentes_2(const std::unordered_map<Color, int, HashColor>& frecuencia, int n);

void escribirPPM(const std::string& filename, const std::vector<Color>& pixeles, int width, int height);

std::tuple<int, int> getPPMDimensions(const std::string& inputFile);

struct KDNode {
    Color color;
    std::unique_ptr<KDNode> left;  // Cambiado a std::unique_ptr
    std::unique_ptr<KDNode> right; // Cambiado a std::unique_ptr
    KDNode(Color color) : color(color), left(nullptr), right(nullptr) {}
};
// Construcción del K-D Tree en 3 dimensiones (RGB)
std::unique_ptr<KDNode> construirKDTree(std::vector<Color>& colores, int depth);
// Función de búsqueda de vecino más cercano
// Función de distancia cuadrada para evitar la raíz cuadrada
double distanciaCuadrada(const Color& colora, const Color& colorb);

std::pair<Color, double> buscarVecinoMasCercano(std::unique_ptr<KDNode>& root, const Color& target, int depth);
// Función para encontrar el color más cercano para una lista de colores menos frecuentes
std::vector<Color> encontrarColoresCercanos(std::unique_ptr<KDNode>& root, const std::vector<Color>& coloresMenosFrecuentes);

std::vector<Color> readPixelsFromImage(const std::string& inputFile, int width, int height);

struct ImageData {
    std::vector<Color> pixels;
    std::vector<Color> uniqueColors;
    std::vector<int> colorCount;
    int width = 0;
    int height = 0;
};
ImageData loadImageData(const std::string& inputFile);
// Función para preparar los colores restantes para el KD-Tree
std::vector<Color> prepareRemainingColors(const ImageData& data,
                                        const std::vector<Color>& menosFrecuentes);

// Función para encontrar los colores menos frecuentes
std::vector<Color> findLessFrequentColors(const ImageData& data, int numColors);

// Función para sustituir los colores menos frecuentes
void substituteColors(std::vector<Color>& pixels,
                     const std::vector<Color>& menosFrecuentes,
                     std::unique_ptr<KDNode>& kdTreeRoot);

// Función principal refactorizada
void processCutfreq(const std::string& inputFile, int numColors, const std::string& outputFile);
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
Image loadPPM(const std::string& filename);

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