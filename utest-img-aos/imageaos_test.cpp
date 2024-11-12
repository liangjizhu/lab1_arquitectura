/*#include "imgaos/imageaos.hpp"
#include "common/imageinfo.hpp"
#include "imgaos/color.hpp"
#include <gtest/gtest.h>
#include <vector>
#include "common/binaryio.hpp"
#include <common/progargs.hpp>
#include <unordered_map>
#include <fstream>
#include <cstdio>


// Function declaration
std::vector<Color> encontrar_colores_menos_frecuentes(const std::unordered_map<Color, int, HashColor>& colorFrequency, int numColors);

// Constantes descriptivas
constexpr uint16_t MAX_COLOR_VALUE_8BIT = 255;  // Valor máximo para un color de 8 bits

// COMPRESS
// Caso de prueba para `extractImagePixels`
TEST(ImageAosTest, ExtractImagePixels_8Bit) {
    constexpr PPMHeader header{2, 2, MAX_COLOR_VALUE_8BIT};
    std::vector<uint8_t> const fileData = {
        MAX_COLOR_VALUE_8BIT, 0, 0, 0, MAX_COLOR_VALUE_8BIT, 0,
        0, 0, MAX_COLOR_VALUE_8BIT, MAX_COLOR_VALUE_8BIT, MAX_COLOR_VALUE_8BIT, MAX_COLOR_VALUE_8BIT
    };

    auto pixels = extractImagePixels(fileData, header);

    ASSERT_EQ(pixels.size(), 4);
    EXPECT_EQ(pixels[0].rgb.red, MAX_COLOR_VALUE_8BIT);
    EXPECT_EQ(pixels[1].rgb.green, MAX_COLOR_VALUE_8BIT);
    EXPECT_EQ(pixels[2].rgb.blue, MAX_COLOR_VALUE_8BIT);
    EXPECT_EQ(pixels[3].rgb.red, MAX_COLOR_VALUE_8BIT);
    EXPECT_EQ(pixels[3].rgb.green, MAX_COLOR_VALUE_8BIT);
    EXPECT_EQ(pixels[3].rgb.blue, MAX_COLOR_VALUE_8BIT);
}

// Caso de prueba para `createColorTable`
TEST(ImageAosTest, CreateColorTable) {
    std::vector<Color> const imagePixels = {
        Color({MAX_COLOR_VALUE_8BIT, 0, 0}),
        Color({0, MAX_COLOR_VALUE_8BIT, 0}),
        Color({0, 0, MAX_COLOR_VALUE_8BIT}),
        Color({MAX_COLOR_VALUE_8BIT, MAX_COLOR_VALUE_8BIT, MAX_COLOR_VALUE_8BIT}),
        Color({MAX_COLOR_VALUE_8BIT, 0, 0}) // Duplicado para probar la tabla de colores
    };

    auto [colorTable, colorIndex] = createColorTable(imagePixels);

    ASSERT_EQ(colorTable.size(), 4);
    EXPECT_EQ(colorIndex.size(), 4);
    EXPECT_EQ(colorIndex.at(Color({MAX_COLOR_VALUE_8BIT, 0, 0})), 2);
    EXPECT_EQ(colorIndex.at(Color({0, MAX_COLOR_VALUE_8BIT, 0})), 1);
    EXPECT_EQ(colorIndex.at(Color({0, 0, MAX_COLOR_VALUE_8BIT})), 0);
    EXPECT_EQ(colorIndex.at(Color({MAX_COLOR_VALUE_8BIT, MAX_COLOR_VALUE_8BIT, MAX_COLOR_VALUE_8BIT})), 3);
}

// Caso de prueba para `appendColorTable`
TEST(ImageAosTest, AppendColorTable) {
    std::vector<Color> const colorTable = {
        Color({MAX_COLOR_VALUE_8BIT, 0, 0}),
        Color({0, MAX_COLOR_VALUE_8BIT, 0}),
        Color({0, 0, MAX_COLOR_VALUE_8BIT})
    };
    constexpr PPMHeader header{2, 2, MAX_COLOR_VALUE_8BIT};
    std::vector<uint8_t> compressedData;

    appendColorTable(compressedData, colorTable, header);

    ASSERT_EQ(compressedData.size(), 9);
    EXPECT_EQ(compressedData[0], MAX_COLOR_VALUE_8BIT);
    EXPECT_EQ(compressedData[1], 0);
    EXPECT_EQ(compressedData[2], 0);
    EXPECT_EQ(compressedData[3], 0);
    EXPECT_EQ(compressedData[4], MAX_COLOR_VALUE_8BIT);
    EXPECT_EQ(compressedData[5], 0);
    EXPECT_EQ(compressedData[6], 0);
    EXPECT_EQ(compressedData[7], 0);
    EXPECT_EQ(compressedData[8], MAX_COLOR_VALUE_8BIT);
}

// Caso de prueba para `appendPixelIndices`
TEST(ImageAosTest, AppendPixelIndices) {
    std::vector<Color> const imagePixels = {
        Color({MAX_COLOR_VALUE_8BIT, 0, 0}),
        Color({0, MAX_COLOR_VALUE_8BIT, 0}),
        Color({0, 0, MAX_COLOR_VALUE_8BIT}),
        Color({MAX_COLOR_VALUE_8BIT, MAX_COLOR_VALUE_8BIT, MAX_COLOR_VALUE_8BIT})
    };
    std::unordered_map<Color, int> const colorIndex = {
        {Color({MAX_COLOR_VALUE_8BIT, 0, 0}), 0},
        {Color({0, MAX_COLOR_VALUE_8BIT, 0}), 1},
        {Color({0, 0, MAX_COLOR_VALUE_8BIT}), 2},
        {Color({MAX_COLOR_VALUE_8BIT, MAX_COLOR_VALUE_8BIT, MAX_COLOR_VALUE_8BIT}), 3}
    };
    std::vector<uint8_t> compressedData;

    appendPixelIndices(compressedData, imagePixels, colorIndex);

    ASSERT_EQ(compressedData.size(), 4);
    EXPECT_EQ(compressedData[0], 0);
    EXPECT_EQ(compressedData[1], 1);
    EXPECT_EQ(compressedData[2], 2);
    EXPECT_EQ(compressedData[3], 3);
}

// Caso de prueba para `compressAoS`
TEST(ImageAosTest, CompressAoS) {
    std::string const inputFilename = "test_input.ppm";
    std::string const outputFilename = "test_output.cppm";

    // Crear archivo PPM de prueba
    std::ofstream outFile(inputFilename, std::ios::binary);
    ASSERT_TRUE(outFile.is_open());
    outFile << "P6\n2 2\n" << MAX_COLOR_VALUE_8BIT << "\n";
    outFile << static_cast<char>(MAX_COLOR_VALUE_8BIT) << static_cast<char>(0) << static_cast<char>(0)
            << static_cast<char>(0) << static_cast<char>(MAX_COLOR_VALUE_8BIT) << static_cast<char>(0)
            << static_cast<char>(0) << static_cast<char>(0) << static_cast<char>(MAX_COLOR_VALUE_8BIT)
            << static_cast<char>(MAX_COLOR_VALUE_8BIT) << static_cast<char>(MAX_COLOR_VALUE_8BIT) << static_cast<char>(MAX_COLOR_VALUE_8BIT);
    outFile.close();

    FilePaths const paths{inputFilename, outputFilename};

    try {
        compressAoS(paths);
    } catch (const std::exception& e) {
        FAIL() << "Expected no exception, but got: " << e.what();
    }

    // Verificar que el archivo comprimido se ha creado
    std::ifstream inFile(outputFilename, std::ios::binary);
    ASSERT_TRUE(inFile.is_open());
    inFile.close();

    // Eliminar archivos temporales y verificar el resultado de `std::remove()`
    int const removeInput = std::remove(inputFilename.c_str());
    EXPECT_EQ(removeInput, 0);

    int const removeOutput = std::remove(outputFilename.c_str());
    EXPECT_EQ(removeOutput, 0);
}
// TODO
// MAX LEVEL
// ARGS RESIZE
// ARGS CUTFREQ



void readImageAndStoreColors(const std::string& inputFile, std::vector<Color>& pixelList, std::unordered_map<Color, int, HashColor>& colorFrequency);
void processCutfreq(const std::string& inputFilename, int numColors, const std::string& outputFilename);
// Caso de prueba para `encontrar_colores_menos_frecuentes`
TEST(ImageAosTest, EncontrarColoresMenosFrecuentes) {
    const std::unordered_map<Color, int, HashColor> colorFrequency = {
        {Color({MAX_COLOR_VALUE_8BIT, 0, 0}), 5},
        {Color({0, MAX_COLOR_VALUE_8BIT, 0}), 3},
        {Color({0, 0, MAX_COLOR_VALUE_8BIT}), 8},
        {Color({MAX_COLOR_VALUE_8BIT, MAX_COLOR_VALUE_8BIT, 0}), 1},
        {Color({0, MAX_COLOR_VALUE_8BIT, MAX_COLOR_VALUE_8BIT}), 2}};
    const int numColors = 2;
    std::vector<Color> menosFrecuentes = encontrar_colores_menos_frecuentes(colorFrequency, numColors);

    ASSERT_EQ(menosFrecuentes.size(), 2);
    EXPECT_EQ(menosFrecuentes[0].rgb.red, 255);
    EXPECT_EQ(menosFrecuentes[0].rgb.green, 255);
    EXPECT_EQ(menosFrecuentes[0].rgb.blue, 0);
    EXPECT_EQ(menosFrecuentes[1].rgb.red, 0);
    EXPECT_EQ(menosFrecuentes[1].rgb.green, 255);
    EXPECT_EQ(menosFrecuentes[1].rgb.blue, 255);
}

// Caso de prueba para `readImageAndStoreColors`
TEST(ImageAosTest, ReadImageAndStoreColors) {
    std::string const inputFilename = "test_input.ppm";
    std::ofstream outFile(inputFilename, std::ios::binary);
    ASSERT_TRUE(outFile.is_open());
    outFile << "P6\n2 2\n" << MAX_COLOR_VALUE_8BIT << "\n";
    outFile << static_cast<char>(MAX_COLOR_VALUE_8BIT) << static_cast<char>(0) << static_cast<char>(0)
            << static_cast<char>(0) << static_cast<char>(MAX_COLOR_VALUE_8BIT) << static_cast<char>(0)
            << static_cast<char>(0) << static_cast<char>(0) << static_cast<char>(MAX_COLOR_VALUE_8BIT)
            << static_cast<char>(MAX_COLOR_VALUE_8BIT) << static_cast<char>(MAX_COLOR_VALUE_8BIT) << static_cast<char>(0);
    outFile.close();

    std::vector<Color> pixelList;
    std::unordered_map<Color, int, HashColor> colorFrequency;
    readImageAndStoreColors(inputFilename, pixelList, colorFrequency);

    ASSERT_EQ(pixelList.size(), 4);
    EXPECT_EQ(pixelList[0].rgb.red, 255);
    EXPECT_EQ(pixelList[0].rgb.green, 0);
    EXPECT_EQ(pixelList[0].rgb.blue, 0);
    EXPECT_EQ(pixelList[1].rgb.red, 0);
    EXPECT_EQ(pixelList[1].rgb.green, 255);
    EXPECT_EQ(pixelList[1].rgb.blue, 0);
    EXPECT_EQ(pixelList[2].rgb.red, 0);
    EXPECT_EQ(pixelList[2].rgb.green, 0);
    EXPECT_EQ(pixelList[2].rgb.blue, 255);
    EXPECT_EQ(pixelList[3].rgb.red, 255);
    EXPECT_EQ(pixelList[3].rgb.green, 255);
    EXPECT_EQ(pixelList[3].rgb.blue, 0);

    ASSERT_EQ(colorFrequency.size(), 4);
    EXPECT_EQ(colorFrequency[Color({255, 0, 0})], 1);
    EXPECT_EQ(colorFrequency[Color({0, 255, 0})], 1);
    EXPECT_EQ(colorFrequency[Color({0, 0, 255})], 1);
    EXPECT_EQ(colorFrequency[Color({255, 255, 0})], 1);

    int const removeInput = std::remove(inputFilename.c_str());
    EXPECT_EQ(removeInput, 0);
}

// Caso de prueba para `processCutfreq`
TEST(ImageAosTest, ProcessCutfreq) {
    std::string const inputFilename = "test_input.ppm";
    std::string const outputFilename = "test_output.ppm";

    // Crear archivo PPM de prueba
    std::ofstream outFile(inputFilename, std::ios::binary);
    ASSERT_TRUE(outFile.is_open());
    outFile << "P6\n2 2\n" << MAX_COLOR_VALUE_8BIT << "\n";
    outFile << static_cast<char>(MAX_COLOR_VALUE_8BIT) << static_cast<char>(0) << static_cast<char>(0)
            << static_cast<char>(0) << static_cast<char>(MAX_COLOR_VALUE_8BIT) << static_cast<char>(0)
            << static_cast<char>(0) << static_cast<char>(0) << static_cast<char>(MAX_COLOR_VALUE_8BIT)
            << static_cast<char>(MAX_COLOR_VALUE_8BIT) << static_cast<char>(MAX_COLOR_VALUE_8BIT) << static_cast<char>(0);
    outFile.close();

    const int numColors = 2;
    try {
        processCutfreq(inputFilename, numColors, outputFilename);
    } catch (const std::exception& e) {
        FAIL() << "Expected no exception, but got: " << e.what();
    }

    // Verificar que el archivo de salida se ha creado
    std::ifstream inFile(outputFilename, std::ios::binary);
    ASSERT_TRUE(inFile.is_open());
    inFile.close();

    // Eliminar archivos temporales y verificar el resultado de `std::remove()`
    int const removeInput = std::remove(inputFilename.c_str());
    EXPECT_EQ(removeInput, 0);

    int const removeOutput = std::remove(outputFilename.c_str());
    EXPECT_EQ(removeOutput, 0);
}

*/