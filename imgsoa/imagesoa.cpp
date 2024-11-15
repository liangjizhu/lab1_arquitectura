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

#include <unordered_set>
#include <limits>
#include <cmath>
#include <memory>

// Constantes descriptivas
constexpr uint16_t BYTE_MASK = 0xFF;
constexpr uint16_t MAX_COLOR_VALUE_8BIT = 255;
constexpr uint8_t BITS_PER_BYTE = 8;
constexpr size_t COLOR_TABLE_RESERVE_SIZE = 256;

constexpr int SHIFT_RED = 16;     // Desplazamiento para el canal rojo
constexpr int SHIFT_GREEN = 8;    // Desplazamiento para el canal verde
constexpr int MASK = 0xFF;

/********************************************* MAX LEVEL SOA *********************************************/
void modifyMaxLevelInputChannels(ColorChannels imageChannels, std::vector<uint8_t>& outputBytes, PPMHeader header, uint32_t antiguoNivel){
    ColorChannels const channels(static_cast<size_t>(header.width) * static_cast<size_t>(header.height));

    // Obtener referencias directas a los canales para evitar llamadas a métodos
    auto& reds = imageChannels.getRedChannel();
    auto& greens = imageChannels.getGreenChannel();
    auto& blues = imageChannels.getBlueChannel();
    const size_t totalPixels = imageChannels.size();

    // Iterar los canales
    for (size_t i = 0; i < totalPixels; ++i) {
      reds[i] = static_cast<uint16_t>(uint32_t(reds[i]) * header.maxColorValue / antiguoNivel);
      greens[i] = static_cast<uint16_t>(uint32_t(greens[i]) * header.maxColorValue / antiguoNivel);
      blues[i] = static_cast<uint16_t>(uint32_t(blues[i]) * header.maxColorValue / antiguoNivel);
    }

    // Escribir en binario el contenido de los channels
    imageChannels.writeToBinary(outputBytes, header);
}
void processMaxLevel(const FilePaths& paths, uint16_t maxLevel){
  // Leer los datos binarios del archivo de entrada
    const std::vector<uint8_t> fileData = BinaryIO::readBinaryFile(paths.inputFile);
    if (fileData.empty()) {
      std::cerr << "Error: No se pudo abrir o leer el archivo de entrada: " << paths.inputFile << '\n';
      return;
    }
    // Leer el encabezado del archivo PPM
    PPMHeader header{};
    if (!readPPMHeader(paths.inputFile, header)) {
      std::cerr << "Error al leer el encabezado del archivo PPM." << '\n';
      return;
    }
    // Crear objeto ColorChannels para manejar los canales
    ColorChannels channels(static_cast<size_t>(header.width) * static_cast<size_t>(header.height));
    channels.extractFromBinary(fileData, header);
    uint32_t const antiguoNivel = header.maxColorValue;
    header.maxColorValue = maxLevel;
    std::vector<uint8_t> outputBytes;

    // Extraer los canales de la imagen a partir de los datos binarios
    modifyMaxLevelInputChannels(channels, outputBytes, header, antiguoNivel);

    // Escribir
    writePPM(paths.outputFile, outputBytes, header.width, header.height);
}

/********************************************* COMPRESS SOA *********************************************/
// Crea un índice de colores único a partir de los canales de color
std::unordered_map<std::string, int> buildColorIndex(const ColorChannels& channels, ColorChannels& colorTable) {

    std::unordered_map<std::string, int> colorIndex;
    colorIndex.reserve(COLOR_TABLE_RESERVE_SIZE);

    size_t tableIndex = 0;

    for (size_t i = 0; i < channels.size(); ++i) {
        // Genera una clave de color como cadena ("R, G, B") para cada píxel
        std::string const colorKey = std::to_string(channels.getRedChannel()[i]) + "," +
                                     std::to_string(channels.getGreenChannel()[i]) + "," +
                                     std::to_string(channels.getBlueChannel()[i]);
        // Si el color es nuevo (no está en el índice), lo agrega a la colorTable
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

// Ordenar la tabla de colores en orden RGB
std::vector<size_t> sortColorTable(ColorChannels& colorTable) {
    std::vector<size_t> indices(colorTable.getRedChannel().size());
    std::iota(indices.begin(), indices.end(), 0);
    // std::ranges::sort -> ordenar contenedores con una sintaxis más concisa y segura
    // Lambda [&colorTable](size_t indexA, size_t indexB) -> [&colorTable] captura la referencia a colorTable, permitiendo acceder a sus metodos
    // std::tie crea una tupla temporal con los valores (R, G, B) del color en la posición indexA y lo compara con la tupla del color en indexB
    // (R1, G1, B1) < (R2, G2, B2) si:
    // - R1 < R2
    // - o si R1 == R2 y G1 < G2
    // - o si R1 == R2 y G1 == G2 y B1 < B2
    std::ranges::sort(indices.begin(), indices.end(), [&colorTable](size_t indexA, size_t indexB) {
        return std::tie(colorTable.getRedChannel()[indexA], colorTable.getGreenChannel()[indexA], colorTable.getBlueChannel()[indexA]) <
               std::tie(colorTable.getRedChannel()[indexB], colorTable.getGreenChannel()[indexB], colorTable.getBlueChannel()[indexB]);
    });

    return indices;
}

// Crea una nueva tabla de colores (sortedColorTable) siguiendo el orden de los índices proporcionados
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

// Función principal para crear la tabla de colores
std::tuple<ColorChannels, std::unordered_map<std::string, int>>
createColorTableSoA(const ColorChannels& channels) {
    // Crear la tabla de colores y el índice
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
    // Asegurarse de que la extensión del archivo de salida sea .cppm
    std::string const outputFile = ensureCppmExtension(paths.outputFile);
    // Leer los datos binarios del archivo de entrada
    const std::vector<uint8_t> fileData = BinaryIO::readBinaryFile(paths.inputFile);
    if (fileData.empty()) {
      std::cerr << "Error: No se pudo abrir o leer el archivo de entrada: " << paths.inputFile << '\n';
      return;
    }
    // Leer el encabezado del archivo PPM
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
/********************************************************************************************************/

namespace imgsoa {

  namespace {

    bool readImageData(const std::string& inputFile, PPMHeader& header, std::vector<uint8_t>& data) {
      try {
        data = readPPMData(inputFile, header);
      } catch (const std::runtime_error& e) {
        std::cerr << "Error: Failed to read image data - " << e.what() << '\n';
        return false;
      }
      return true;
    }

    namespace {
      ImageSOA convertToSOA(const std::vector<uint8_t>& data, const PPMHeader& header) {
        ImageSOA image;
        image.width = header.width;
        image.height = header.height;
        const size_t pixelCount = static_cast<size_t>(image.width) * static_cast<size_t>(image.height);

        image.redChannel.resize(pixelCount);
        image.greenChannel.resize(pixelCount);
        image.blueChannel.resize(pixelCount);

        for (size_t i = 0, j = 0; i < data.size(); i += 3, ++j) {
          image.redChannel[j] = data[i];
          image.greenChannel[j] = data[i + 1];
          image.blueChannel[j] = data[i + 2];
        }
        return image;
      }
    }



    std::vector<uint8_t> convertToFlatData(const ImageSOA& image, int newWidth, int newHeight) {
      std::vector<uint8_t> resizedData(static_cast<size_t>(newWidth) * static_cast<size_t>(newHeight) * 3);
      for (size_t j = 0, k = 0; j < resizedData.size(); j += 3, ++k) {
        resizedData[j] = image.redChannel[k];
        resizedData[j + 1] = image.greenChannel[k];
        resizedData[j + 2] = image.blueChannel[k];
      }
      return resizedData;
    }

    bool writeImageData(const std::string& outputFile, const std::vector<uint8_t>& data, int width, int height) {
      try {
        writePPM(outputFile, data, width, height);
      } catch (const std::runtime_error& e) {
        std::cerr << "Error: Failed to write resized image - " << e.what() << '\n';
        return false;
      }
      return true;
    }

  }  // namespace

    bool resizeAndSaveImage(const FilePaths& filePaths, int newWidth, int newHeight) {
    PPMHeader header{};
    std::vector<uint8_t> data;

    // Step 1: Read the image data
    if (!readImageData(filePaths.inputFile, header, data)) {
      return false;
    }

    // Step 2: Convert to SOA format
    const ImageSOA image = convertToSOA(data, header);

    // Step 3: Resize the image
    const ImageSOA resizedImage = resizeImageSOA(image, newWidth, newHeight);

    // Step 4: Convert resized image to flat data format
    const std::vector<uint8_t> resizedData = convertToFlatData(resizedImage, newWidth, newHeight);

    // Step 5: Write resized image to output file
    return writeImageData(filePaths.outputFile, resizedData, newWidth, newHeight);
  }

  }  // namespace imgsoa


  namespace {

    struct SourceCoordinates {
        float x;
        float y;
      };

    struct ImageDimensions {
      size_t width;
      size_t height;
    };

    std::tuple<size_t, size_t, size_t, size_t, float, float> calculateSourceCoordinatesAndWeights(
        const SourceCoordinates& sourceCoords, const ImageDimensions& originalDims) {

      const auto xLeft = static_cast<size_t>(std::floor(sourceCoords.x));
      const auto yLeft = static_cast<size_t>(std::floor(sourceCoords.y));
      const auto xRight = std::clamp(xLeft + 1, static_cast<size_t>(0), originalDims.width - 1);
      const auto yRight = std::clamp(yLeft + 1, static_cast<size_t>(0), originalDims.height - 1);


      const float xWeight = sourceCoords.x - static_cast<float>(xLeft);
      const float yWeight = sourceCoords.y - static_cast<float>(yLeft);

      return {xLeft, yLeft, xRight, yRight, xWeight, yWeight};
    }

  namespace {

struct RowInterpolationParams {
    const std::vector<uint8_t>& channel; //NOLINT
    size_t imageWidth;
    size_t xLow;
    size_t xHigh;
    size_t y;
};

// Function to calculate the value at a specific position
float getValueAtPosition(const std::vector<uint8_t>& channel, size_t index) {
    return static_cast<float>(channel[index]);
}

// Function to get the indices for a specific row
std::pair<size_t, size_t> getRowIndices(const RowInterpolationParams& params) {
    const size_t leftIndex = (params.y * params.imageWidth) + params.xLow;
    const size_t rightIndex = (params.y * params.imageWidth) + params.xHigh;
    return {leftIndex, rightIndex};
}

// Function to interpolate between two channel values
float interpolate(float lowValue, float highValue, float weight) { //NOLINT
    return lowValue + (weight * (highValue - lowValue));
}

// Extract top and bottom interpolations
float interpolateRow(const RowInterpolationParams& params) {
    const auto [leftIndex, rightIndex] = getRowIndices(params);
    const float left = getValueAtPosition(params.channel, leftIndex);
    const float right = getValueAtPosition(params.channel, rightIndex);
    return interpolate(left, right, 0.0F);  // No weight here, will be added in main function
}

uint8_t bilinearInterpolateChannel(const BilinearInterpolationParams& params) {
  const RowInterpolationParams topParams = {
    .channel = params.channel,
    .imageWidth = params.imageWidth,
    .xLow = params.xLow,
    .xHigh = params.xHigh,
    .y = params.yLow
};
  const RowInterpolationParams bottomParams = {
    .channel = params.channel,
    .imageWidth = params.imageWidth,
    .xLow = params.xLow,
    .xHigh = params.xHigh,
    .y = params.yHigh
};

  const float topInterpolation = interpolateRow(topParams) * params.xWeight;
  const float bottomInterpolation = interpolateRow(bottomParams) * (1 - params.xWeight);

  // Perform vertical interpolation between the two rows
  return static_cast<uint8_t>(topInterpolation + (params.yWeight * (bottomInterpolation - topInterpolation)));
}


} // end of anonymous namespace


  } // namespace

  namespace {

    // Updated helper function to calculate scaling ratios with ImageDimensions structs
    std::pair<float, float> calculateScalingRatios(
        const ImageDimensions& originalDims, const ImageDimensions& newDims) {

      const float xRatio = static_cast<float>(originalDims.width - 1) / static_cast<float>(newDims.width - 1);
      const float yRatio = static_cast<float>(originalDims.height - 1) / static_cast<float>(newDims.height - 1);

      return {xRatio, yRatio};
    }

  } // end of anonymous namespace


  ImageSOA resizeImageSOA(const ImageSOA& image, int newWidth, int newHeight) { //NOLINT
    ImageSOA resizedImage;
    resizedImage.width = newWidth;
    resizedImage.height = newHeight;

    const size_t pixelCount = static_cast<size_t>(newWidth) * static_cast<size_t>(newHeight);
    resizedImage.redChannel.resize(pixelCount);
    resizedImage.greenChannel.resize(pixelCount);
    resizedImage.blueChannel.resize(pixelCount);

    // Define original and new dimensions with designated initializers
    const ImageDimensions originalDims = {.width = static_cast<size_t>(image.width), .height = static_cast<size_t>(image.height)};
    const ImageDimensions newDims = {.width = static_cast<size_t>(newWidth), .height = static_cast<size_t>(newHeight)};

    // Calculate scaling ratios
    auto [xRatio, yRatio] = calculateScalingRatios(originalDims, newDims);

    for (int newY = 0; newY < newHeight; ++newY) {
        for (int newX = 0; newX < newWidth; ++newX) {
            const SourceCoordinates sourceCoords = {.x = static_cast<float>(newX) * xRatio, .y = static_cast<float>(newY) * yRatio};

            // Calculate source coordinates and weights
            auto [xLow, yLow, xHigh, yHigh, xWeight, yWeight] = calculateSourceCoordinatesAndWeights(sourceCoords, originalDims);

            const size_t index = (static_cast<size_t>(newY) * static_cast<size_t>(newWidth)) + static_cast<size_t>(newX);

            // Interpolate and set pixel for red channel
            resizedImage.redChannel[index] = bilinearInterpolateChannel({
                .channel = image.redChannel,
                .imageWidth = originalDims.width,
                .xLow = xLow,
                .yLow = yLow,
                .xHigh = xHigh,
                .yHigh = yHigh,
                .xWeight = xWeight,
                .yWeight = yWeight
            });

            // Interpolate and set pixel for green channel
            resizedImage.greenChannel[index] = bilinearInterpolateChannel({
                .channel = image.greenChannel,
                .imageWidth = originalDims.width,
                .xLow = xLow,
                .yLow = yLow,
                .xHigh = xHigh,
                .yHigh = yHigh,
                .xWeight = xWeight,
                .yWeight = yWeight
            });

            // Interpolate and set pixel for blue channel
            resizedImage.blueChannel[index] = bilinearInterpolateChannel({
                .channel = image.blueChannel,
                .imageWidth = originalDims.width,
                .xLow = xLow,
                .yLow = yLow,
                .xHigh = xHigh,
                .yHigh = yHigh,
                .xWeight = xWeight,
                .yWeight = yWeight
            });
        }
    }

    return resizedImage;
}


/********************************************* CUTFREQ SOA *********************************************/
//NOLINTBEGIN(misc-no-recursion)
void readImageAndStoreChannels(const std::string& inputFile, ColorChannels& colorChannels, std::unordered_map<uint32_t, int, HashColor>& colorFrequency) {
    PPMHeader header{};
    if (!readPPMHeader(inputFile, header)) {
      std::cerr << "Error al leer el encabezado del archivo PPM." << '\n';
      return;
    }
    // Abrir el archivo en modo binario
    std::ifstream file(inputFile, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo de entrada: " << inputFile << '\n';
        return;
    }
    // Leer el encabezado del archivo PPM
    std::string format;
    file >> format >> header.width >> header.height >> header.maxColorValue;
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignorar el salto de línea después del encabezado
    // Verificar formato y valor máximo de color
    if (format != "P6" || header.maxColorValue != MAX_COLOR_VALUE_8BIT) {
        std::cerr << "Error: Formato PPM no soportado o valor máximo de color inválido en " << inputFile << '\n';
        return;
    }
    // Calcular el número total de píxeles
    const size_t totalPixels = static_cast<size_t>(header.width) * static_cast<size_t>(header.height);
    // Reservar espacio en los canales de color
    colorChannels.reserve(totalPixels);
    // Leer todos los datos de píxeles en un solo bloque
    std::vector<char> buffer(totalPixels * 3); // Cada píxel tiene 3 componentes (RGB)
    file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
    // Extraer los datos a los canales y calcular frecuencias
    const std::vector<uint8_t> uint8Buffer(buffer.begin(), buffer.end());
    colorChannels.extractFromBinaryWithFrequency(uint8Buffer, header, colorFrequency);
    file.close();
}

std::unordered_set<std::tuple<uint16_t, uint16_t, uint16_t>, HashTuple> encontrar_colores_menos_frecuentes_2(
    const std::unordered_map<uint32_t, int, HashColor>& frecuencia, int n) {
    // Vector para almacenar colores con sus frecuencias y ordenar
    std::vector<std::pair<std::tuple<uint16_t, uint16_t, uint16_t>, int>> colores_frecuentes;
    // Convertimos cada color en una tupla RGB y lo almacenamos junto con su frecuencia
    for (const auto& [color, freq] : frecuencia) {
        const uint16_t red = (color >> 16) & 0xFF;
        const uint16_t green = (color >> 8) & 0xFF;
        const uint16_t blue = color & 0xFF;
        colores_frecuentes.emplace_back(std::make_tuple(red, green, blue), freq);
    }
    // Ordenar el vector por frecuencia en orden ascendente
      // En caso de empate, ordenar por azul (descendente), luego verde (descendente), y finalmente rojo (descendente)
    std::sort(colores_frecuentes.begin(), colores_frecuentes.end(), [](const auto& colora, const auto& colorb) {
        if (colora.second != colorb.second) {
            return colora.second < colorb.second;
        }
        // Comparar componentes b, luego g, luego r en orden descendente para los empates
        const auto& [ra, ga, ba] = colora.first;
        const auto& [rb, gb, bb] = colorb.first;
        if (ba != bb) {
            return ba > bb;
        }
        if (ga != gb) {
            return ga > gb;
        }
        return ra > rb;
    });
    // Crear un conjunto para los colores menos frecuentes
    std::unordered_set<std::tuple<uint16_t, uint16_t, uint16_t>, HashTuple> colores_menos_frecuentes;

    // Agregar los primeros n colores menos frecuentes al conjunto
    for (size_t i = 0; i < std::min(static_cast<size_t>(n), colores_frecuentes.size()); ++i) {
        colores_menos_frecuentes.insert(colores_frecuentes[i].first);
    }

    return colores_menos_frecuentes;
}


void sustituirColoresEnImagen(ColorChannels& colorChannels, const std::unordered_map<std::tuple<uint16_t, uint16_t, uint16_t>, std::tuple<uint16_t, uint16_t, uint16_t>, HashTuple>& replacementMap) {
    // Crear un mapa de búsqueda más eficiente usando un único entero como clave
    std::unordered_map<uint32_t, uint32_t> fastReplacementMap;
    fastReplacementMap.reserve(replacementMap.size());  // Pre-reservar espacio
    // Convertir el mapa de tuplas a un mapa de enteros
    for (const auto& [oldColor, newColor] : replacementMap) {
        const uint32_t oldPacked = (static_cast<uint32_t>(std::get<0>(oldColor)) << SHIFT_RED) |
                            (static_cast<uint32_t>(std::get<1>(oldColor)) << SHIFT_GREEN) |
                            (static_cast<uint32_t>(std::get<2>(oldColor)));

        const uint32_t newPacked = (static_cast<uint32_t>(std::get<0>(newColor)) << SHIFT_RED) |
                            (static_cast<uint32_t>(std::get<1>(newColor)) << SHIFT_GREEN) |
                            (static_cast<uint32_t>(std::get<2>(newColor)));

        fastReplacementMap[oldPacked] = newPacked;
    }
    // Obtener referencias directas a los canales para evitar llamadas a métodos
    auto& reds = colorChannels.getRedChannel();
    auto& greens = colorChannels.getGreenChannel();
    auto& blues = colorChannels.getBlueChannel();
    // Procesar los píxeles en bloques para mejor uso de la caché
    constexpr size_t BLOCK_SIZE = 1024;
    const size_t totalPixels = colorChannels.size();
    for (size_t blockStart = 0; blockStart < totalPixels; blockStart += BLOCK_SIZE) {
        const size_t blockEnd = std::min(blockStart + BLOCK_SIZE, totalPixels);
        for (size_t i = blockStart; i < blockEnd; ++i) {
            // Empaquetar el color actual en un único entero
            const uint32_t currentPacked = (static_cast<uint32_t>(reds[i]) << SHIFT_RED) |
                                   (static_cast<uint32_t>(greens[i]) << SHIFT_GREEN) |
                                   (static_cast<uint32_t>(blues[i]));
            // Buscar si hay que reemplazar el color
            auto its = fastReplacementMap.find(currentPacked);
            if (its != fastReplacementMap.end()) {
                // Desempaquetar el nuevo color directamente a los canales
                reds[i] = (its->second >> SHIFT_RED) & MASK;
                greens[i] = (its->second >> SHIFT_GREEN) & MASK;
                blues[i] = its->second & MASK;
            }
        }
    }
}



// Construcción balanceada del árbol KD
std::unique_ptr<KDNode> construirKDTree(std::vector<std::tuple<uint16_t, uint16_t, uint16_t>>& colors, int depth = 0) {
    if (colors.empty()) {
        return nullptr;
    }
    const int axis = depth % 3;
    std::sort(colors.begin(), colors.end(), [axis](const auto& sidea, const auto& sideb) {
        if (axis == 0) {
            return std::get<0>(sidea) < std::get<0>(sideb);
        }
        if (axis == 1) {
            return std::get<1>(sidea) < std::get<1>(sideb);
        }
        return std::get<2>(sidea) < std::get<2>(sideb);
        if (axis == 1){
            return std::get<1>(sidea) < std::get<1>(sideb);
        }
        return std::get<2>(sidea) < std::get<2>(sideb);
    });

    const size_t medianIndex = colors.size() / 2;
    //auto* root = new KDNode(colors[medianIndex]);
    std::unique_ptr<KDNode> root = std::make_unique<KDNode>(colors[medianIndex]);
    std::vector<std::tuple<uint16_t, uint16_t, uint16_t>> leftColors(colors.begin(), colors.begin() + static_cast<std::vector<std::tuple<uint16_t, uint16_t, uint16_t>>::difference_type>(medianIndex));
    std::vector<std::tuple<uint16_t, uint16_t, uint16_t>> rightColors(colors.begin() + static_cast<std::vector<std::tuple<uint16_t, uint16_t, uint16_t>>::difference_type>(medianIndex) + 1, colors.end());

    root->left = construirKDTree(leftColors, depth + 1);
    root->right = construirKDTree(rightColors, depth + 1);

    return root;
}

// Calcular la distancia al cuadrado entre dos colores
double calcularDistanciaCuadrada(const std::tuple<uint16_t, uint16_t, uint16_t>& color1,
                                 const std::tuple<uint16_t, uint16_t, uint16_t>& color2) {
    return std::pow(std::get<0>(color1) - std::get<0>(color2), 2) +
           std::pow(std::get<1>(color1) - std::get<1>(color2), 2) +
           std::pow(std::get<2>(color1) - std::get<2>(color2), 2);
}

void buscarVecinoMasCercanoOptimizado(KDNode* root, BusquedaVecino& busqueda, int depth = 0) {
    if (root == nullptr){
        return;
    }
    // Calculamos la distancia al cuadrado entre el color buscado y el color del nodo actual
    const double distancia = calcularDistanciaCuadrada(busqueda.color, root->color);
    // Si encontramos un color más cercano, lo actualizamos
    if (distancia < busqueda.minDistanciaCuadrada) {
        busqueda.minDistanciaCuadrada = distancia;
        busqueda.mejorColor = root->color;
    }
    const int axis = depth % 3;
    KDNode* primerSubArbol = nullptr;
    KDNode* segundoSubArbol = nullptr;
    // Decidimos qué subárbol explorar primero
    if ((axis == 0 && std::get<0>(busqueda.color) < std::get<0>(root->color)) ||
        (axis == 1 && std::get<1>(busqueda.color) < std::get<1>(root->color)) ||
        (axis == 2 && std::get<2>(busqueda.color) < std::get<2>(root->color))) {
            primerSubArbol = root->left.get();
            segundoSubArbol = root->right.get();
        } else {
            primerSubArbol = root->right.get();
            segundoSubArbol = root->left.get();
    }
    // Recursión en el primer subárbol
    buscarVecinoMasCercanoOptimizado(primerSubArbol, busqueda, depth + 1);
    // Calculamos la distancia al eje para ver si necesitamos explorar el otro subárbol
    double distanciaAlEje = 0.0;
    if (axis == 0) {
        distanciaAlEje = std::pow(std::get<0>(busqueda.color) - std::get<0>(root->color), 2);
    } else if (axis == 1) {
        distanciaAlEje = std::pow(std::get<1>(busqueda.color) - std::get<1>(root->color), 2);
    } else {
        distanciaAlEje = std::pow(std::get<2>(busqueda.color) - std::get<2>(root->color), 2);
    }
    // Si la distancia al eje es menor que la mínima encontrada, exploramos el otro subárbol
    if (distanciaAlEje < busqueda.minDistanciaCuadrada) {
        buscarVecinoMasCercanoOptimizado(segundoSubArbol, busqueda, depth + 1);
    }
}

void writePPM(const std::string& outputFile, const PPMHeader& header, const ColorChannels& colorChannels) {
    std::ofstream outFile(outputFile, std::ios::binary);
    if (!outFile.is_open()) {
        std::cerr << "Error al abrir el archivo de salida: " << outputFile << '\n';
        return;
    }
    // Configurar buffer de salida para mejor rendimiento
    constexpr size_t BUFFER_SIZE = static_cast<size_t>(1024) * 1024; // 1MB buffer
    std::vector<char> writeBuffer(BUFFER_SIZE);
    outFile.rdbuf()->pubsetbuf(writeBuffer.data(), BUFFER_SIZE);
    // Pre-calcular el tamaño del encabezado y reservar espacio
    const std::string headerStr = "P6\n" + std::to_string(header.width) + " " +
                           std::to_string(header.height) + "\n255\n";
    // Escribir encabezado con conversión segura de tipo
    outFile.write(headerStr.c_str(), static_cast<std::streamsize>(headerStr.size()));
    // Obtener referencias directas a los canales
    const auto& reds = colorChannels.getRedChannel();
    const auto& greens = colorChannels.getGreenChannel();
    const auto& blues = colorChannels.getBlueChannel();
    // Calcular el tamaño total de los datos de píxeles
    const size_t totalPixels = colorChannels.size();
    const size_t bufferSize = totalPixels * 3; // 3 bytes por píxel (RGB)
    // Crear un buffer temporal para los datos de píxeles
    std::vector<uint8_t> pixelBuffer(bufferSize);
    // Llenar el buffer con los datos RGB de manera eficiente
    for (size_t i = 0, j = 0; i < totalPixels; ++i) {
        pixelBuffer[j++] = static_cast<uint8_t>(reds[i]);
        pixelBuffer[j++] = static_cast<uint8_t>(greens[i]);
        pixelBuffer[j++] = static_cast<uint8_t>(blues[i]);
    }
    // Escribir todos los datos de píxeles de una sola vez con conversión segura de tipo
    //NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
    outFile.write(reinterpret_cast<const char*>(pixelBuffer.data()),
              static_cast<std::streamsize>(bufferSize));
    //NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
    // Forzar la escritura de cualquier dato en el buffer
    outFile.flush();
    std::cout << "Imagen procesada escrita en: " << outputFile << '\n';
}

std::unordered_map<std::tuple<uint16_t, uint16_t, uint16_t>, std::tuple<uint16_t, uint16_t, uint16_t>, HashTuple> encontrarColoresReemplazo(
    const std::unordered_set<std::tuple<uint16_t, uint16_t, uint16_t>, HashTuple>& colorsToRemoveSet,
    const ColorChannels& colorChannels) {

    std::unordered_map<std::tuple<uint16_t, uint16_t, uint16_t>, std::tuple<uint16_t, uint16_t, uint16_t>, HashTuple> replacementMap;

    // Filtramos los colores candidatos para reemplazo (solo aquellos que no están en la lista de eliminados).
    //la lista de candidatos es candidateColors
    //colorsToRemoveSet tiene que ser la lista de n colores menos frecuentes
    std::vector<std::tuple<uint16_t, uint16_t, uint16_t>> candidateColors;
    for (size_t i = 0; i < colorChannels.size(); ++i) {
        auto candidateColor = std::make_tuple(colorChannels.getRedChannel()[i], colorChannels.getGreenChannel()[i], colorChannels.getBlueChannel()[i]);
        if (colorsToRemoveSet.find(candidateColor) == colorsToRemoveSet.end()) {
            candidateColors.push_back(candidateColor); // Guardamos solo los colores que no han sido eliminados.
        }
    }

    // Buscamos el color más cercano de los candidatos para cada color eliminado.
    for (const auto& colorToRemove : colorsToRemoveSet) {
        std::tuple<uint16_t, uint16_t, uint16_t> closestColor;
        double minDistance = std::numeric_limits<double>::max(); // Inicializamos la distancia más corta como un valor muy alto.

        // Recorremos los candidatos y calculamos la distancia euclidiana en el espacio RGB.
        for (const auto& candidateColor : candidateColors) {
            const int redDiff = static_cast<int>(std::get<0>(colorToRemove)) - static_cast<int>(std::get<0>(candidateColor));
            const int greenDiff = static_cast<int>(std::get<1>(colorToRemove)) - static_cast<int>(std::get<1>(candidateColor));
            const int blueDiff = static_cast<int>(std::get<2>(colorToRemove)) - static_cast<int>(std::get<2>(candidateColor));
            const double distance = (redDiff * redDiff) + (greenDiff * greenDiff) + (blueDiff * blueDiff);

            // Si encontramos una distancia menor, actualizamos el color más cercano.
            if (distance < minDistance) {
                minDistance = distance;
                closestColor = candidateColor;
            }
        }

        // Guardamos en el mapa de reemplazo el color que debe sustituir al eliminado.
        replacementMap[colorToRemove] = closestColor;
    }

    return replacementMap; // Devolvemos el mapa de reemplazo de colores.
}

void processCutfreq(const std::string& inputFile, int numColors, const std::string& outputFile) {
    PPMHeader header{};
    if (!readPPMHeader(inputFile, header)) {
        std::cerr << "Error al leer el encabezado del archivo PPM.\n";
        return;
    }
    const size_t totalPixels = static_cast<size_t>(header.width) * static_cast<size_t>(header.height);
    ColorChannels colorChannels(totalPixels);
    std::unordered_map<uint32_t, int, HashColor> colorFrequency;
    readImageAndStoreChannels(inputFile, colorChannels, colorFrequency);
    std::cout << "Colores únicos al principio: " << colorFrequency.size() << '\n';
    auto coloresMenosFrecuentes = encontrar_colores_menos_frecuentes_2(colorFrequency, numColors);
    std::vector<std::tuple<uint16_t, uint16_t, uint16_t>> coloresRestantes;
    for (const auto& [color, _] : colorFrequency) {
        const uint16_t red = (color >> 16) & 0xFF;
        const uint16_t green = (color >> 8) & 0xFF;
        const uint16_t blue = color & 0xFF;
        auto colorTupla = std::make_tuple(red, green, blue);
        if (coloresMenosFrecuentes.find(colorTupla) == coloresMenosFrecuentes.end()) {
            coloresRestantes.push_back(colorTupla);
        }
    }
    const std::unique_ptr<KDNode> kdTreeRoot = construirKDTree(coloresRestantes);
    std::unordered_map<std::tuple<uint16_t, uint16_t, uint16_t>, std::tuple<uint16_t, uint16_t, uint16_t>, HashTuple> replacementMap;
    // Modificamos la forma en que se pasan los parámetros a la búsqueda
    for (const auto& colorMenosFrecuente : coloresMenosFrecuentes) {
        // Creamos la estructura de búsqueda
        BusquedaVecino busqueda{colorMenosFrecuente};
        // Llamamos a la función con la estructura
        buscarVecinoMasCercanoOptimizado(kdTreeRoot.get(), busqueda);
        // Usamos el resultado almacenado en la estructura
        replacementMap[colorMenosFrecuente] = busqueda.mejorColor;
    }
    sustituirColoresEnImagen(colorChannels, replacementMap);
    writePPM(outputFile, header, colorChannels);
    ColorChannels colorChannels2(totalPixels);
    std::unordered_map<uint32_t, int, HashColor> colorFrequency2;
    readImageAndStoreChannels(outputFile, colorChannels2, colorFrequency2);
    std::cout << "Colores únicos al final: " << colorFrequency2.size() << '\n';
}
//NOLINTEND(misc-no-recursion)
/********************************************************************************************************/

