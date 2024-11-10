#include "imagesoa.hpp"
#include "binaryio.hpp"
#include "imageinfo.hpp"
#include "color.hpp"
#include <vector>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>
#include <sstream>
#include <tuple>

// Constantes descriptivas
constexpr uint16_t BYTE_MASK = 0xFF;
constexpr uint16_t MAX_COLOR_VALUE_8BIT = 255;
constexpr uint16_t MAX_COLOR_VALUE_16BIT = 65535;
constexpr uint8_t BITS_PER_BYTE = 8;
constexpr size_t COLOR_TABLE_RESERVE_SIZE = 256;

// Crear tabla de colores y mapa de índices (SoA) usando ColorChannels
std::tuple<ColorChannels, std::unordered_map<std::string, int>>
createColorTableSoA(const ColorChannels& channels) {
    std::unordered_map<std::string, int> colorIndex;
    colorIndex.reserve(COLOR_TABLE_RESERVE_SIZE);

    ColorChannels colorTable(channels.size());
    size_t tableIndex = 0;

    for (size_t i = 0; i < channels.size(); ++i) {
        std::string colorKey = std::to_string(channels.getRedChannel()[i]) + "," +
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

    // Ordenar la tabla de colores en orden lexicográfico RGB
    std::vector<size_t> indices(colorTable.getRedChannel().size());
    std::iota(indices.begin(), indices.end(), 0);

    std::sort(indices.begin(), indices.end(), [&colorTable](size_t a, size_t b) {
        return std::tie(colorTable.getRedChannel()[a], colorTable.getGreenChannel()[a], colorTable.getBlueChannel()[a]) <
               std::tie(colorTable.getRedChannel()[b], colorTable.getGreenChannel()[b], colorTable.getBlueChannel()[b]);
    });

    // Crear una tabla de colores ordenada
    ColorChannels sortedColorTable(colorTable.size());
    for (size_t i = 0; i < indices.size(); ++i) {
        size_t index = indices[i];
        sortedColorTable.getRedChannel().push_back(colorTable.getRedChannel()[index]);
        sortedColorTable.getGreenChannel().push_back(colorTable.getGreenChannel()[index]);
        sortedColorTable.getBlueChannel().push_back(colorTable.getBlueChannel()[index]);
    }

    // Reconstruir el índice de colores para la tabla ordenada
    colorIndex.clear();
    for (size_t i = 0; i < sortedColorTable.size(); ++i) {
        std::string colorKey = std::to_string(sortedColorTable.getRedChannel()[i]) + "," +
                               std::to_string(sortedColorTable.getGreenChannel()[i]) + "," +
                               std::to_string(sortedColorTable.getBlueChannel()[i]);
        colorIndex[colorKey] = static_cast<int>(i);
    }

    return {sortedColorTable, colorIndex};
}

// Generar encabezado del archivo comprimido
std::string generateHeaderSoA(const PPMHeader& header, int colorTableSize) {
  std::ostringstream headerStream;
  headerStream << "C6 " << header.width << " " << header.height << " " << header.maxColorValue << " " << colorTableSize << "\n";
  return headerStream.str();
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
    std::string colorKey = std::to_string(channels.getRedChannel()[i]) + "," +
                           std::to_string(channels.getGreenChannel()[i]) + "," +
                           std::to_string(channels.getBlueChannel()[i]);

    int index = colorIndex.at(colorKey);
    compressedData.push_back(static_cast<uint8_t>(index & BYTE_MASK));
    compressedData.push_back(static_cast<uint8_t>(index >> BITS_PER_BYTE));
  }
}


// Función principal de compresión SoA
void compressSoA(const std::string& inputFile, std::string outputFile) {
  const std::vector<uint8_t> fileData = BinaryIO::readBinaryFile(inputFile);
  if (fileData.empty()) {
    std::cerr << "Error: No se pudo abrir o leer el archivo de entrada: " << inputFile << '\n';
    return;
  }

  PPMHeader header;
  if (!readPPMHeader(inputFile, header)) {
    std::cerr << "Error al leer el encabezado del archivo PPM." << '\n';
    return;
  }

  // Verificar y agregar la extensión '.cppm' si no está presente
  if (outputFile.find(".cppm") == std::string::npos) {
    outputFile += ".cppm";
  }

  // Crear objeto ColorChannels para manejar los canales
  ColorChannels channels(static_cast<size_t>(header.width) * static_cast<size_t>(header.height));
  channels.extractFromBinary(fileData, header);

  // Crear la tabla de colores y el índice
  auto [colorTable, colorIndex] = createColorTableSoA(channels);
  std::string headerStr = generateHeaderSoA(header, static_cast<int>(colorTable.size()));

  // Crear datos comprimidos
  std::vector<uint8_t> compressedData;
  compressedData.insert(compressedData.end(), headerStr.begin(), headerStr.end());
  appendColorTableSoA(compressedData, colorTable, header);
  appendPixelIndicesSoA(compressedData, channels, colorIndex);

  // Escribir el archivo comprimido
  BinaryIO::writeBinaryFile(outputFile, compressedData);
}