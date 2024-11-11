#include "imgaos/imageaos.hpp"
#include "common/imageinfo.hpp"
#include "imgaos/color.hpp"
#include <gtest/gtest.h>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <cstdio>

// Constantes descriptivas
constexpr uint16_t MAX_COLOR_VALUE_8BIT = 255;  // Valor máximo para un color de 8 bits

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

    ASSERT_NO_THROW(compressAoS(paths));

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
