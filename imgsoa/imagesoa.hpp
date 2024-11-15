#ifndef IMAGESOA_HPP
#define IMAGESOA_HPP

#include "color.hpp"
#include "imageinfo.hpp"
#include "progargs.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <limits>
/********************************************* COMPRESS SOA
 * *********************************************/
// Función principal para comprimir una imagen en formato SOA
void compressSoA(const FilePaths& paths);

// Crea un índice de colores único a partir de los canales de color
std::unordered_map<std::string, int> buildColorIndex(const ColorChannels& channels, ColorChannels& colorTable);

// Ordenar la tabla de colores en orden RGB
std::vector<size_t> sortColorTable(ColorChannels& colorTable);

// Crea una nueva tabla de colores (sortedColorTable) siguiendo el orden de los índices proporcionados
ColorChannels createSortedColorTable(const ColorChannels& colorTable, const std::vector<size_t>& indices);

// Reconstruir el índice de colores para la tabla ordenada
std::unordered_map<std::string, int> rebuildColorIndex(const ColorChannels& sortedColorTable);

// Crear tabla de colores y mapa de índices (SoA)
std::tuple<ColorChannels, std::unordered_map<std::string, int>>
createColorTableSoA(const ColorChannels& channels);

// Añadir la tabla de colores al archivo comprimido
void appendColorTableSoA(std::vector<uint8_t>& compressedData,
                         const ColorChannels& colorTable,
                         const PPMHeader& header);

// Añadir los índices de los píxeles al archivo comprimido
void appendPixelIndicesSoA(std::vector<uint8_t>& compressedData,
                           const ColorChannels& channels,
                           const std::unordered_map<std::string, int>& colorIndex);
/********************************************************************************************************/
/********************************************* MAX LEVEL SOA *********************************************/
void modifyMaxLevelInputChannels(ColorChannels imageChannels, std::vector<uint8_t>& outputBytes, PPMHeader header, uint32_t antiguoNivel);
void processMaxLevel(const FilePaths& paths, uint16_t maxLevel);
/********************************************* CUTFREQ SOA *********************************************/

void readImageAndStoreChannels(const std::string& inputFile, ColorChannels& colorChannels, std::unordered_map<uint32_t, int, HashColor>& colorFrequency);

std::unordered_set<std::tuple<uint16_t, uint16_t, uint16_t>, HashTuple> encontrar_colores_menos_frecuentes_2(
    const std::unordered_map<uint32_t, int, HashColor>& frecuencia, int n);

void sustituirColoresEnImagen(ColorChannels& colorChannels, const std::unordered_map<std::tuple<uint16_t, uint16_t, uint16_t>, std::tuple<uint16_t, uint16_t, uint16_t>, HashTuple>& replacementMap);

struct KDNode {
    std::tuple<uint16_t, uint16_t, uint16_t> color;
    std::unique_ptr<KDNode> left = nullptr;
    std::unique_ptr<KDNode> right = nullptr;

    KDNode(const std::tuple<uint16_t, uint16_t, uint16_t>& color)
        : color(color) {}
};
// Construcción balanceada del árbol KD
std::unique_ptr<KDNode> construirKDTree(std::vector<std::tuple<uint16_t, uint16_t, uint16_t>>& colors, int depth);
// Calcular la distancia al cuadrado entre dos colores
double calcularDistanciaCuadrada(const std::tuple<uint16_t, uint16_t, uint16_t>& color1,
                                 const std::tuple<uint16_t, uint16_t, uint16_t>& color2);
struct BusquedaVecino {
    std::tuple<uint16_t, uint16_t, uint16_t> color;  // El color que estamos buscando
    double minDistanciaCuadrada;                     // La distancia mínima encontrada hasta ahora
    std::tuple<uint16_t, uint16_t, uint16_t> mejorColor;  // El mejor color encontrado hasta ahora

    BusquedaVecino(const std::tuple<uint16_t, uint16_t, uint16_t>& col)
        : color(col), minDistanciaCuadrada(std::numeric_limits<double>::infinity()) {}
};
void buscarVecinoMasCercanoOptimizado(KDNode* root, BusquedaVecino& busqueda, int depth);

void writePPM(const std::string& outputFile, const PPMHeader& header, const ColorChannels& colorChannels);

std::unordered_map<std::tuple<uint16_t, uint16_t, uint16_t>, std::tuple<uint16_t, uint16_t, uint16_t>, HashTuple> encontrarColoresReemplazo(
    const std::unordered_set<std::tuple<uint16_t, uint16_t, uint16_t>, HashTuple>& colorsToRemoveSet,
    const ColorChannels& colorChannels);

void processCutfreq(const std::string& inputFile, int numColors, const std::string& outputFile);
/********************************************************************************************************/

struct ImageSOA {
  std::vector<uint8_t> redChannel;
  std::vector<uint8_t> greenChannel;
  std::vector<uint8_t> blueChannel;
  int width;
  int height;
};

ImageSOA resizeImageSOA(const ImageSOA& image, int newWidth, int newHeight);

// Define Pixel and Image for AoS (Array of Structs)
struct Pixel {
  uint8_t r, g, b;
};

using Image = std::vector<std::vector<Pixel>>;

// Function declarations for your image manipulation functions
Image vectorToImage(const std::vector<uint8_t>& data, int width, int height, int channels);
std::vector<uint8_t> imageToVector(const Image& image, int channels);
Image resizeImageAoS(const Image& image, int newWidth, int newHeight);

// Helper structures for resize and interpolation functions
struct Coordinate {
  float x;
  float y;
};

struct TargetCoordinates {
  int x;
  int y;
};

struct InterpolationChannelParams {
  uint8_t topLeft;
  uint8_t topRight;
  uint8_t bottomLeft;
  uint8_t bottomRight;
  float xWeight;
  float yWeight;
};

struct InterpolationPixelParams {
  Pixel topLeft;
  Pixel topRight;
  Pixel bottomLeft;
  Pixel bottomRight;
  float xWeight;
  float yWeight;
};

struct ScaleRatios {
  float xRatio;
  float yRatio;
};

// These functions will be used in tests for image resizing
Image vectorToImage(const std::vector<uint8_t>& data, int width, int height, int channels);
std::vector<uint8_t> imageToVector(const Image& image, int channels);
Image resizeImageAoS(const Image& image, int newWidth, int newHeight);

#endif // IMAGESOA_HPP
