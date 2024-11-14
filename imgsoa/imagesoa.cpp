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
#include <stdexcept>
#include <utility>


#include "imagesoa.hpp"
#include "binaryio.hpp"
#include "imageinfo.hpp"
#include "color.hpp"
#include "progargs.hpp"
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>
#include <tuple>
#include <limits>
#include <cmath>
#include <memory>
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

namespace imgsoa {

  bool resizeAndSaveImage(const std::string& inputFile, const std::string& outputFile, int newWidth, int newHeight) {
    // Step 1: Read the image from the input file
    PPMHeader header;
    std::vector<uint8_t> data;
    try {
      data = readPPMData(inputFile, header);  // Ensure readPPMData is defined
    } catch (const std::runtime_error& e) {
      std::cerr << "Error: Failed to read image data - " << e.what() << '\n';
      return false;
    }

    // Step 2: Convert data to SOA format
    ImageSOA image;
    image.width = header.width;
    image.height = header.height;
    image.redChannel.resize(static_cast<size_t>(image.width) * static_cast<size_t>(image.height));
    image.greenChannel.resize(static_cast<size_t>(image.width) * static_cast<size_t>(image.height));
    image.blueChannel.resize(static_cast<size_t>(image.width) * static_cast<size_t>(image.height));

    for (size_t i = 0, j = 0; i < data.size(); i += 3, ++j) {
      image.redChannel[j] = data[i];
      image.greenChannel[j] = data[i + 1];
      image.blueChannel[j] = data[i + 2];
    }

    // Step 3: Resize the image
    ImageSOA resizedImage = resizeImageSOA(image, newWidth, newHeight);

    // Step 4: Convert resizedImage to a flat uint8_t vector
    std::vector<uint8_t> resizedData(static_cast<size_t>(newWidth) * static_cast<size_t>(newHeight) * 3);
    for (size_t j = 0, k = 0; j < resizedData.size(); j += 3, ++k) {
      resizedData[j] = resizedImage.redChannel[k];
      resizedData[j + 1] = resizedImage.greenChannel[k];
      resizedData[j + 2] = resizedImage.blueChannel[k];
    }

    // Step 5: Write resized image to output file
    try {
      writePPM(outputFile, resizedData, newWidth, newHeight);  // Ensure writePPM is defined
    } catch (const std::runtime_error& e) {
      std::cerr << "Error: Failed to write resized image - " << e.what() << '\n';
      return false;
    }

    return true;
  }

}  // namespace imgsoa

ImageSOA resizeImageSOA(const ImageSOA& image, int newWidth, int newHeight) {
    ImageSOA resizedImage;
    resizedImage.width = newWidth;
    resizedImage.height = newHeight;

    // Resize channels with size_t cast
    resizedImage.redChannel.resize(static_cast<size_t>(newWidth) * static_cast<size_t>(newHeight));
    resizedImage.greenChannel.resize(static_cast<size_t>(newWidth) * static_cast<size_t>(newHeight));
    resizedImage.blueChannel.resize(static_cast<size_t>(newWidth) * static_cast<size_t>(newHeight));

    // Calculate scaling ratios with float casts
    float xRatio = static_cast<float>(image.width - 1) / static_cast<float>(newWidth - 1);
    float yRatio = static_cast<float>(image.height - 1) / static_cast<float>(newHeight - 1);

    for (int y = 0; y < newHeight; ++y) {
        for (int x = 0; x < newWidth; ++x) {
            float srcX = static_cast<float>(x) * xRatio;
            float srcY = static_cast<float>(y) * yRatio;

            size_t xL = std::min(static_cast<size_t>(srcX), static_cast<size_t>(image.width - 1));
            size_t yL = std::min(static_cast<size_t>(srcY), static_cast<size_t>(image.height - 1));
            size_t xH = std::clamp(xL + 1, size_t(0), static_cast<size_t>(image.width - 1));
            size_t yH = std::clamp(yL + 1, size_t(0), static_cast<size_t>(image.height - 1));

            float xWeight = srcX - static_cast<float>(xL);
            float yWeight = srcY - static_cast<float>(yL);

            auto interpolate = [&](const std::vector<uint8_t>& channel) {
                float topLeft = static_cast<float>(channel[yL * static_cast<size_t>(image.width) + xL]);
                float topRight = static_cast<float>(channel[yL * static_cast<size_t>(image.width) + xH]);
                float bottomLeft = static_cast<float>(channel[yH * static_cast<size_t>(image.width) + xL]);
                float bottomRight = static_cast<float>(channel[yH * static_cast<size_t>(image.width) + xH]);

                return static_cast<uint8_t>(
                    (topLeft * (1.0f - xWeight) * (1.0f - yWeight)) +
                    (topRight * xWeight * (1.0f - yWeight)) +
                    (bottomLeft * (1.0f - xWeight) * yWeight) +
                    (bottomRight * xWeight * yWeight)
                );
            };

            size_t index = static_cast<size_t>(y) * static_cast<size_t>(newWidth) + static_cast<size_t>(x);

            resizedImage.redChannel[index] = interpolate(image.redChannel);
            resizedImage.greenChannel[index] = interpolate(image.greenChannel);
            resizedImage.blueChannel[index] = interpolate(image.blueChannel);
        }
    }

    return resizedImage;
}