#include "imgsoa/imagesoa.hpp"
#include "common/imageinfo.hpp"
#include "imgsoa/color.hpp"
#include <gtest/gtest.h>
#include <unordered_map>
#include <vector>
#include <string>

// Constantes
constexpr size_t TEST_SIZE = 4;
constexpr uint16_t MAX_COLOR_8BIT = 255;
constexpr uint16_t MID_COLOR_VALUE_8BIT = 128;
constexpr uint16_t LOW_COLOR_VALUE_8BIT = 64;

/********************************************* COMPRESS AOS *********************************************/
// Caso de prueba para `buildColorIndex()`
TEST(ImageSoATest, BuildColorIndex) {
    ColorChannels channels(TEST_SIZE);
    channels.getRedChannel() = {MAX_COLOR_8BIT, 0, MAX_COLOR_8BIT, MID_COLOR_VALUE_8BIT};
    channels.getGreenChannel() = {0, MAX_COLOR_8BIT, MID_COLOR_VALUE_8BIT, MAX_COLOR_8BIT};
    channels.getBlueChannel() = {MID_COLOR_VALUE_8BIT, MID_COLOR_VALUE_8BIT, 0, MAX_COLOR_8BIT};

    ColorChannels colorTable(TEST_SIZE);
    auto colorIndex = buildColorIndex(channels, colorTable);

    ASSERT_EQ(colorTable.size(), 4);
    EXPECT_EQ(colorIndex["255,0,128"], 0);
    EXPECT_EQ(colorIndex["0,255,128"], 1);
    EXPECT_EQ(colorIndex["255,128,0"], 2);
    EXPECT_EQ(colorIndex["128,255,255"], 3);
}

// Caso de prueba para `sortColorTable()`
TEST(ImageSoATest, SortColorTable) {
    ColorChannels colorTable(TEST_SIZE);
    colorTable.getRedChannel() = {MAX_COLOR_8BIT, MID_COLOR_VALUE_8BIT, 0, MAX_COLOR_8BIT};
    colorTable.getGreenChannel() = {0, MAX_COLOR_8BIT, MID_COLOR_VALUE_8BIT, MAX_COLOR_8BIT};
    colorTable.getBlueChannel() = {MID_COLOR_VALUE_8BIT, 0, MAX_COLOR_8BIT, MID_COLOR_VALUE_8BIT};

    auto indices = sortColorTable(colorTable);

    ASSERT_EQ(indices.size(), 4);
    EXPECT_EQ(indices[0], 2); // (0, 128, 255)
    EXPECT_EQ(indices[1], 1); // (128, 255, 0)
    EXPECT_EQ(indices[2], 0); // (255, 0, 128)
    EXPECT_EQ(indices[3], 3); // (255, 255, 128)
}

// Caso de prueba para `createSortedColorTable()`
TEST(ImageSoATest, CreateSortedColorTable) {
    ColorChannels colorTable(TEST_SIZE);
    colorTable.getRedChannel() = {MAX_COLOR_8BIT, MID_COLOR_VALUE_8BIT, 0, MAX_COLOR_8BIT};
    colorTable.getGreenChannel() = {0, MAX_COLOR_8BIT, MID_COLOR_VALUE_8BIT, MAX_COLOR_8BIT};
    colorTable.getBlueChannel() = {MID_COLOR_VALUE_8BIT, 0, MAX_COLOR_8BIT, MID_COLOR_VALUE_8BIT};

    std::vector<size_t> const indices = {2, 1, 3, 0};
    auto sortedColorTable = createSortedColorTable(colorTable, indices);

    EXPECT_EQ(sortedColorTable.getRedChannel()[0], 0);
    EXPECT_EQ(sortedColorTable.getGreenChannel()[0], MID_COLOR_VALUE_8BIT);
    EXPECT_EQ(sortedColorTable.getBlueChannel()[0], MAX_COLOR_8BIT);

    EXPECT_EQ(sortedColorTable.getRedChannel()[3], MAX_COLOR_8BIT);
    EXPECT_EQ(sortedColorTable.getGreenChannel()[3], 0);
    EXPECT_EQ(sortedColorTable.getBlueChannel()[3], MID_COLOR_VALUE_8BIT);
}

// Caso de prueba para `rebuildColorIndex()`
TEST(ImageSoATest, RebuildColorIndex) {
    ColorChannels sortedColorTable(TEST_SIZE);
    sortedColorTable.getRedChannel() = {0, MID_COLOR_VALUE_8BIT, MAX_COLOR_8BIT, MAX_COLOR_8BIT};
    sortedColorTable.getGreenChannel() = {MID_COLOR_VALUE_8BIT, MAX_COLOR_8BIT, MAX_COLOR_8BIT, 0};
    sortedColorTable.getBlueChannel() = {MAX_COLOR_8BIT, 0, MID_COLOR_VALUE_8BIT, MID_COLOR_VALUE_8BIT};

    auto colorIndex = rebuildColorIndex(sortedColorTable);

    EXPECT_EQ(colorIndex["0,128,255"], 0);
    EXPECT_EQ(colorIndex["128,255,0"], 1);
    EXPECT_EQ(colorIndex["255,255,128"], 2);
    EXPECT_EQ(colorIndex["255,0,128"], 3);
}

// Caso de prueba para `createColorTableSoA()`
TEST(ImageSoATest, CreateColorTableSoA) {
    ColorChannels channels(TEST_SIZE);
    channels.getRedChannel() = {MAX_COLOR_8BIT, 0, MAX_COLOR_8BIT, MID_COLOR_VALUE_8BIT};
    channels.getGreenChannel() = {0, MAX_COLOR_8BIT, MID_COLOR_VALUE_8BIT, MAX_COLOR_8BIT};
    channels.getBlueChannel() = {MID_COLOR_VALUE_8BIT, MID_COLOR_VALUE_8BIT, 0, MAX_COLOR_8BIT};

    auto [sortedColorTable, colorIndex] = createColorTableSoA(channels);

    ASSERT_EQ(sortedColorTable.size(), 4);
    EXPECT_EQ(colorIndex["0,255,128"], 0);
    EXPECT_EQ(colorIndex["128,255,255"], 1);
    EXPECT_EQ(colorIndex["255,0,128"], 2);
    EXPECT_EQ(colorIndex["255,128,0"], 3);
}

// Caso de prueba para `appendColorTableSoA()`
TEST(ImageSoATest, AppendColorTableSoA) {
    ColorChannels colorTable(2);
    colorTable.getRedChannel() = {MAX_COLOR_8BIT, MID_COLOR_VALUE_8BIT};
    colorTable.getGreenChannel() = {0, MAX_COLOR_8BIT};
    colorTable.getBlueChannel() = {MID_COLOR_VALUE_8BIT, LOW_COLOR_VALUE_8BIT};

    std::vector<uint8_t> compressedData;
    constexpr PPMHeader header{2, 2, MAX_COLOR_8BIT};
    appendColorTableSoA(compressedData, colorTable, header);

    ASSERT_EQ(compressedData.size(), 6);
    EXPECT_EQ(compressedData[0], MAX_COLOR_8BIT);
    EXPECT_EQ(compressedData[1], 0);
    EXPECT_EQ(compressedData[2], MID_COLOR_VALUE_8BIT);
    EXPECT_EQ(compressedData[3], MID_COLOR_VALUE_8BIT);
    EXPECT_EQ(compressedData[4], MAX_COLOR_8BIT);
    EXPECT_EQ(compressedData[5], LOW_COLOR_VALUE_8BIT);
}

// Caso de prueba para `appendPixelIndicesSoA()`
TEST(ImageSoATest, AppendPixelIndicesSoA) {
    ColorChannels channels(2);
    channels.getRedChannel() = {MAX_COLOR_8BIT, MID_COLOR_VALUE_8BIT};
    channels.getGreenChannel() = {0, MAX_COLOR_8BIT};
    channels.getBlueChannel() = {MID_COLOR_VALUE_8BIT, LOW_COLOR_VALUE_8BIT};

    std::unordered_map<std::string, int> const colorIndex = {
        {"255,0,128", 0},
        {"128,255,64", 1}
    };

    std::vector<uint8_t> compressedData;
    appendPixelIndicesSoA(compressedData, channels, colorIndex);

    ASSERT_EQ(compressedData.size(), 4);
    EXPECT_EQ(compressedData[0], 0);
    EXPECT_EQ(compressedData[1], 0);
    EXPECT_EQ(compressedData[2], 1);
    EXPECT_EQ(compressedData[3], 0);
}
/********************************************************************************************************/

/********************************************* CUTFREQ SOA *********************************************/
// Prueba para readImageAndStoreChannels
TEST(KDTreeTests, TestReadImageAndStoreChannels) {
    const std::string inputFile = "../in/lake-large.ppm";
    ColorChannels colorChannels(100);
    std::unordered_map<uint32_t, int, HashColor> colorFrequency;

    readImageAndStoreChannels(inputFile, colorChannels, colorFrequency);

    ASSERT_GT(colorChannels.size(), 0);
    ASSERT_GT(colorFrequency.size(), 0);
}

// Prueba para encontrar_colores_menos_frecuentes_2
TEST(KDTreeTests, TestEncontrarColoresMenosFrecuentes2) {
    std::unordered_map<uint32_t, int, HashColor> colorFrequency;
    colorFrequency[0x123456] = 10;
    colorFrequency[0x654321] = 5;
    colorFrequency[0xABCDEF] = 1;

    auto coloresMenosFrecuentes = encontrar_colores_menos_frecuentes_2(colorFrequency, 2);

    ASSERT_EQ(coloresMenosFrecuentes.size(), 2);
    EXPECT_TRUE(coloresMenosFrecuentes.find(std::make_tuple(0xAB, 0xCD, 0xEF)) != coloresMenosFrecuentes.end());
    EXPECT_TRUE(coloresMenosFrecuentes.find(std::make_tuple(0x65, 0x43, 0x21)) != coloresMenosFrecuentes.end());
}

// Prueba para construirKDTree
TEST(KDTreeTests, TestConstruirKDTree) {
    std::vector<std::tuple<uint16_t, uint16_t, uint16_t>> colors = {
        {10, 20, 30}, {40, 50, 60}, {70, 80, 90}
    };

    auto kdTreeRoot = construirKDTree(colors, 0);

    ASSERT_NE(kdTreeRoot, nullptr);
    EXPECT_EQ(kdTreeRoot->color, std::make_tuple(40, 50, 60));
}

// Prueba para buscarVecinoMasCercanoOptimizado
TEST(KDTreeTests, TestBuscarVecinoMasCercanoOptimizado) {
    std::vector<std::tuple<uint16_t, uint16_t, uint16_t>> colors = {
        {10, 20, 30}, {40, 50, 60}, {70, 80, 90}
    };
    auto kdTreeRoot = construirKDTree(colors, 0);
    BusquedaVecino busqueda({15, 25, 35});

    buscarVecinoMasCercanoOptimizado(kdTreeRoot.get(), busqueda, 0);

    EXPECT_EQ(busqueda.mejorColor, std::make_tuple(10, 20, 30));
}

// Prueba para sustituirColoresEnImagen
TEST(KDTreeTests, TestSustituirColoresEnImagen) {
    ColorChannels colorChannels(3);
    colorChannels.getRedChannel() = {10, 40, 70};
    colorChannels.getGreenChannel() = {20, 50, 80};
    colorChannels.getBlueChannel() = {30, 60, 90};

    std::unordered_map<std::tuple<uint16_t, uint16_t, uint16_t>, std::tuple<uint16_t, uint16_t, uint16_t>, HashTuple> replacementMap;
    replacementMap[{10, 20, 30}] = {40, 50, 60};

    sustituirColoresEnImagen(colorChannels, replacementMap);

    EXPECT_EQ(colorChannels.getRedChannel()[0], 40);
    EXPECT_EQ(colorChannels.getGreenChannel()[0], 50);
    EXPECT_EQ(colorChannels.getBlueChannel()[0], 60);
}
/********************************************************************************************************/

// TODO
// MAX LEVEL
// ARGS RESIZE
