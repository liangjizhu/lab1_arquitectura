#include "imagesoa.hpp"
#include "binaryio.hpp"
#include "imageinfo.hpp"
#include "color.hpp"
#include "progargs.hpp"

#include <vector>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>
#include <tuple>

// Constantes descriptivas
constexpr uint16_t BYTE_MASK = 0xFF;
constexpr uint16_t MAX_COLOR_VALUE_8BIT = 255;
constexpr uint16_t MAX_COLOR_VALUE_16BIT = 65535;
constexpr uint8_t BITS_PER_BYTE = 8;
constexpr size_t COLOR_TABLE_RESERVE_SIZE = 256;

// Crear índice de colores
std::unordered_map<std::string, int> buildColorIndex(const ColorChannels& channels, ColorChannels& colorTable) {
    std::unordered_map<std::string, int> colorIndex;
    colorIndex.reserve(COLOR_TABLE_RESERVE_SIZE);

    size_t tableIndex = 0;

    for (size_t i = 0; i < channels.size(); ++i) {
        std::string const colorKey = std::to_string(channels.getRedChannel()[i]) + "," +
                                     std::to_string(channels.getGreenChannel()[i]) + "," +
                                     std::to_string(channels.getBlueChannel()[i]);

        auto [it, inserted] = colorIndex.emplace(colorKey, static_cast<int>(tableIndex));
        if (inserted) {
            colorTable.getRedChannel().push_back(channels.getRedChannel()[i]);
            colorTable.getGreenChannel().push_back(channels.getGreenChannel()[i]);
            colorTable.getBlueChannel().push_back(channels.getBlueChannel()[i]);
            tableIndex++;
        }
    }

    return colorIndex;
}

// Ordenar la tabla de colores en orden lexicográfico RGB
std::vector<size_t> sortColorTable(ColorChannels& colorTable) {
    std::vector<size_t> indices(colorTable.getRedChannel().size());
    std::iota(indices.begin(), indices.end(), 0);

    std::ranges::sort(indices.begin(), indices.end(), [&colorTable](size_t indexA, size_t indexB) {
        return std::tie(colorTable.getRedChannel()[indexA], colorTable.getGreenChannel()[indexA], colorTable.getBlueChannel()[indexA]) <
               std::tie(colorTable.getRedChannel()[indexB], colorTable.getGreenChannel()[indexB], colorTable.getBlueChannel()[indexB]);
    });

    return indices;
}

// Crear tabla de colores ordenada
ColorChannels createSortedColorTable(const ColorChannels& colorTable, const std::vector<size_t>& indices) {
    ColorChannels sortedColorTable(colorTable.size());

    for (unsigned long const index : indices) {
        sortedColorTable.getRedChannel().push_back(colorTable.getRedChannel()[index]);
        sortedColorTable.getGreenChannel().push_back(colorTable.getGreenChannel()[index]);
        sortedColorTable.getBlueChannel().push_back(colorTable.getBlueChannel()[index]);
    }

    return sortedColorTable;
}

// Reconstruir el índice de colores para la tabla ordenada
std::unordered_map<std::string, int> rebuildColorIndex(const ColorChannels& sortedColorTable) {
    std::unordered_map<std::string, int> colorIndex;
    colorIndex.reserve(COLOR_TABLE_RESERVE_SIZE);

    for (size_t i = 0; i < sortedColorTable.size(); ++i) {
        std::string const colorKey = std::to_string(sortedColorTable.getRedChannel()[i]) + "," +
                               std::to_string(sortedColorTable.getGreenChannel()[i]) + "," +
                               std::to_string(sortedColorTable.getBlueChannel()[i]);
        colorIndex[colorKey] = static_cast<int>(i);
    }

    return colorIndex;
}

// Crear tabla de colores y mapa de índices (SoA)
std::tuple<ColorChannels, std::unordered_map<std::string, int>>
createColorTableSoA(const ColorChannels& channels) {
    ColorChannels colorTable(channels.size());
    auto colorIndex = buildColorIndex(channels, colorTable);

    // Ordenar la tabla de colores
    auto indices = sortColorTable(colorTable);
    auto sortedColorTable = createSortedColorTable(colorTable, indices);

    // Reconstruir el índice de colores
    auto sortedColorIndex = rebuildColorIndex(sortedColorTable);

    return {sortedColorTable, sortedColorIndex};
}

// Añadir tabla de colores al archivo comprimido
void appendColorTableSoA(std::vector<uint8_t>& compressedData, const ColorChannels& colorTable, const PPMHeader& header) {
    colorTable.writeToBinary(compressedData, header);
}


// Añadir índices de píxeles al archivo comprimido
void appendPixelIndicesSoA(std::vector<uint8_t>& compressedData,
                           const ColorChannels& channels,
                           const std::unordered_map<std::string, int>& colorIndex) {
    for (size_t i = 0; i < channels.size(); ++i) {
        std::string const colorKey = std::to_string(channels.getRedChannel()[i]) + "," +
                                     std::to_string(channels.getGreenChannel()[i]) + "," +
                                     std::to_string(channels.getBlueChannel()[i]);

        int const index = colorIndex.at(colorKey);
        compressedData.push_back(static_cast<uint8_t>(index & BYTE_MASK));
        compressedData.push_back(static_cast<uint8_t>(index >> BITS_PER_BYTE));
    }
}


// Función principal de compresión SoA
void compressSoA(const FilePaths& paths){
    std::string const outputFile = ensureCppmExtension(paths.outputFile);

    const std::vector<uint8_t> fileData = BinaryIO::readBinaryFile(paths.inputFile);
    if (fileData.empty()) {
      std::cerr << "Error: No se pudo abrir o leer el archivo de entrada: " << paths.inputFile << '\n';
      return;
    }

    PPMHeader header{};
    if (!readPPMHeader(paths.inputFile, header)) {
      std::cerr << "Error al leer el encabezado del archivo PPM." << '\n';
      return;
    }

    // Crear objeto ColorChannels para manejar los canales
    ColorChannels channels(static_cast<size_t>(header.width) * static_cast<size_t>(header.height));
    channels.extractFromBinary(fileData, header);

    // Crear la tabla de colores y el índice
    auto [colorTable, colorIndex] = createColorTableSoA(channels);
    std::string headerStr = generateHeader(header, static_cast<int>(colorTable.size()));

    // Crear datos comprimidos
    std::vector<uint8_t> compressedData;
    compressedData.insert(compressedData.end(), headerStr.begin(), headerStr.end());
    appendColorTableSoA(compressedData, colorTable, header);
    appendPixelIndicesSoA(compressedData, channels, colorIndex);

    // Escribir el archivo comprimido
    BinaryIO::writeBinaryFile(outputFile, compressedData);
}

