#include "imgaos/imageaos.hpp"
#include "common/imageinfo.hpp"
#include "common/interpolation.hpp"
#include "imgaos/color.hpp"
#include <gtest/gtest.h>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <cstdio>

// Constantes descriptivas
constexpr uint16_t MAX_COLOR_VALUE_8BIT = 255;  // Valor máximo para un color de 8 bits

/********************************************* COMPRESS AOS *********************************************/
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
/********************************************************************************************************/

/********************************************* CUTFREQ AOS *********************************************/
// Prueba para `encontrar_colores_menos_frecuentes`
TEST(CutFreqTests, TestMenosFrecuentes) {
    std::unordered_map<Color, int, HashColor> colorFrequency;
    colorFrequency[Color(RGBColor{10, 20, 30})] = 5; // NOLINT(*-avoid-magic-numbers)
    colorFrequency[Color(RGBColor{40, 50, 60})] = 10; // NOLINT(*-avoid-magic-numbers)
    colorFrequency[Color(RGBColor{70, 80, 90})] = 1; // NOLINT(*-avoid-magic-numbers)

    auto menosFrecuentes = encontrar_colores_menos_frecuentes(colorFrequency, 2);

    ASSERT_EQ(menosFrecuentes.size(), 2);
    EXPECT_EQ(menosFrecuentes[0], Color(RGBColor{70, 80, 90}));
    EXPECT_EQ(menosFrecuentes[1], Color(RGBColor{10, 20, 30}));
}

// Prueba para `construirKDTree`
TEST(CutFreqTests, TestConstruirKDTree) {
    std::vector<Color> colores = {
        Color(RGBColor{10, 20, 30}), // NOLINT(*-avoid-magic-numbers)
        Color(RGBColor{40, 50, 60}), // NOLINT(*-avoid-magic-numbers)
        Color(RGBColor{70, 80, 90}) // NOLINT(*-avoid-magic-numbers)
    };

    auto kdTreeRoot = construirKDTree(colores, 0);

    ASSERT_NE(kdTreeRoot, nullptr);
    EXPECT_EQ(kdTreeRoot->color, Color(RGBColor{40, 50, 60}));
    EXPECT_EQ(kdTreeRoot->left->color, Color(RGBColor{10, 20, 30}));
    EXPECT_EQ(kdTreeRoot->right->color, Color(RGBColor{70, 80, 90}));
}

// Prueba para `buscarVecinoMasCercano`
TEST(CutFreqTests, TestBuscarVecinoMasCercano) {
    std::vector<Color> colores = {
        Color(RGBColor{10, 20, 30}), // NOLINT(*-avoid-magic-numbers)
        Color(RGBColor{40, 50, 60}), // NOLINT(*-avoid-magic-numbers)
        Color(RGBColor{70, 80, 90}) // NOLINT(*-avoid-magic-numbers)
    };
    auto kdTreeRoot = construirKDTree(colores, 0);
    Color const target(RGBColor{15, 25, 35});

    auto [mejorColor, distancia] = buscarVecinoMasCercano(kdTreeRoot, target, 0);

    EXPECT_EQ(mejorColor, Color(RGBColor{10, 20, 30}));
}

// Prueba para `substituteColors`
TEST(CutFreqTests, TestSubstituteColors) {
    std::vector<Color> pixels = {
        Color(RGBColor{10, 20, 30}), // NOLINT(*-avoid-magic-numbers)
        Color(RGBColor{40, 50, 60}), // NOLINT(*-avoid-magic-numbers)
        Color(RGBColor{70, 80, 90}), // NOLINT(*-avoid-magic-numbers)
        Color(RGBColor{10, 20, 30}) // NOLINT(*-avoid-magic-numbers)
    };
    std::vector<Color> const menosFrecuentes = {Color(RGBColor{10, 20, 30})};

    std::vector<Color> remainingColors = {
        Color(RGBColor{40, 50, 60}), // NOLINT(*-avoid-magic-numbers)
        Color(RGBColor{70, 80, 90}) // NOLINT(*-avoid-magic-numbers)
    };
    auto kdTreeRoot = construirKDTree(remainingColors, 0);

    substituteColors(pixels, menosFrecuentes, kdTreeRoot);

    EXPECT_EQ(pixels[0], Color(RGBColor{40, 50, 60}));
    EXPECT_EQ(pixels[1], Color(RGBColor{40, 50, 60}));
    EXPECT_EQ(pixels[2], Color(RGBColor{70, 80, 90}));
    EXPECT_EQ(pixels[3], Color(RGBColor{40, 50, 60}));
}

TEST(CutFreqTests, TestPrepareRemainingColors) {
    ImageData data;
    data.colorCount.resize(256 * 256 * 256, 0); // Inicializar el tamaño para colores de 8 bits NOLINT(*-implicit-widening-of-multiplication-result)

    data.uniqueColors = {
      Color(RGBColor{10, 20, 30}), // NOLINT(*-avoid-magic-numbers)
      Color(RGBColor{40, 50, 60}), // NOLINT(*-avoid-magic-numbers)
      Color(RGBColor{70, 80, 90}) // NOLINT(*-avoid-magic-numbers)
    };
    std::vector<Color> const menosFrecuentes = {Color(RGBColor{10, 20, 30})};

    auto remainingColors = prepareRemainingColors(data, menosFrecuentes);

    ASSERT_EQ(remainingColors.size(), 2);
    EXPECT_EQ(remainingColors[0], Color(RGBColor{40, 50, 60}));
    EXPECT_EQ(remainingColors[1], Color(RGBColor{70, 80, 90}));
}
/********************************************************************************************************/

/********************************************* MAXLEVEL AOS *********************************************/
//TEST(MaxLevelTests, TestCambioDeNivel) {
    //std::vector<Color> colores;
    //colores.push_back(Color(RGBColor{17, 12, 7})); // NOLINT(*-avoid-magic-numbers)
    //constexpr PPMHeader header{2, 2, MAX_COLOR_VALUE_8BIT};
    //std::vector<Color> result = modifyMaxLevelInputPixels(colores, header.maxColorValue, static_cast<uint16_t>(100));
    //std::vector<Color> target;
    //target.push_back(Color(RGBColor{5, 4, 2}));
    //EXPECT_EQ(result, target);
//}

/********************************************************************************************************/

/********************************************* CUTFREQ AOS *********************************************/

#include <gtest/gtest.h>

TEST(VectorToImageTest, ConvertsDataToImageSuccessfully) {
    // Sample 3x3 image data, RGB channels
    std::vector<uint8_t> data = {255, 0, 0, 0, 255, 0, 0, 0, 255,
                                 255, 255, 0, 0, 255, 255, 255, 0, 255,
                                 100, 150, 200, 50, 25, 75, 125, 175, 50};
    int width = 3;
    int height = 3;
    int channels = 3;

    Image image = vectorToImage(data, width, height, channels);

    // Check if dimensions match
    ASSERT_EQ(image.size(), height);
    ASSERT_EQ(image[0].size(), width);

    // Check specific pixel values
    EXPECT_EQ(image[0][0].r, 255);
    EXPECT_EQ(image[0][1].g, 255);
    EXPECT_EQ(image[2][2].b, 50);
}

TEST(ImageToVectorTest, ConvertsImageToVectorSuccessfully) {
    // Create a 2x2 image
    Image image = {
        {{255, 0, 0}, {0, 255, 0}},
        {{0, 0, 255}, {100, 150, 200}}
    };
    int channels = 3;

    std::vector<uint8_t> data = imageToVector(image, channels);

    // Expected flat data representation
    std::vector<uint8_t> expectedData = {255, 0, 0, 0, 255, 0, 0, 0, 255, 100, 150, 200};
    ASSERT_EQ(data, expectedData);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
// ARGS CUTFREQ
