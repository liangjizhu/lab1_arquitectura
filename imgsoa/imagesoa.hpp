#ifndef IMAGESOA_HPP
#define IMAGESOA_HPP

#include "imageinfo.hpp"
#include "color.hpp"
#include <string>
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <tuple>

// Función para procesar el comando 'maxlevel'
void processMaxLevel(const std::string& inputFile, int maxLevel);

// Función para la compresión usando Structure of Arrays (SoA)
void compressSoA(const std::string& inputFile, std::string outputFile);

// Crear tabla de colores y mapa de índices (SoA) usando ColorChannels
std::tuple<ColorChannels, std::unordered_map<std::string, int>>
createColorTableSoA(const ColorChannels& channels);

// Generar el encabezado del archivo comprimido
std::string generateHeaderSoA(const PPMHeader& header, int colorTableSize);

// Añadir la tabla de colores al archivo comprimido
void appendColorTableSoA(std::vector<uint8_t>& compressedData,
                         const ColorChannels& colorTable,
                         const PPMHeader& header);

// Añadir los índices de los píxeles al archivo comprimido
void appendPixelIndicesSoA(std::vector<uint8_t>& compressedData,
                           const ColorChannels& channels,
                           const std::unordered_map<std::string, int>& colorIndex);

#endif // IMAGESOA_HPP
