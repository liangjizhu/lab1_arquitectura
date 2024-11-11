#include "imgsoa/imagesoa.hpp"
#include "common/imageinfo.hpp"
#include "imgsoa/color.hpp"
#include "common/binaryio.hpp"
#include <gtest/gtest.h>
#include <unordered_map>
#include <vector>
#include <string>

// Constantes
constexpr size_t TEST_SIZE = 4;
constexpr uint16_t MAX_COLOR_8BIT = 255;

// COMPRESS
// Caso de prueba para `buildColorIndex()`
TEST(ImageSoATest, BuildColorIndex) {
    ColorChannels channels(TEST_SIZE);
    channels.getRedChannel() = {255, 0, 255, 128};
    channels.getGreenChannel() = {0, 255, 128, 255};
    channels.getBlueChannel() = {128, 128, 0, 255};

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
    colorTable.getRedChannel() = {255, 128, 0, 255};
    colorTable.getGreenChannel() = {0, 255, 128, 255};
    colorTable.getBlueChannel() = {128, 0, 255, 128};

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
    colorTable.getRedChannel() = {255, 128, 0, 255};
    colorTable.getGreenChannel() = {0, 255, 128, 255};
    colorTable.getBlueChannel() = {128, 0, 255, 128};

    std::vector<size_t> indices = {2, 1, 3, 0};
    auto sortedColorTable = createSortedColorTable(colorTable, indices);

    EXPECT_EQ(sortedColorTable.getRedChannel()[0], 0);
    EXPECT_EQ(sortedColorTable.getGreenChannel()[0], 128);
    EXPECT_EQ(sortedColorTable.getBlueChannel()[0], 255);

    EXPECT_EQ(sortedColorTable.getRedChannel()[3], 255);
    EXPECT_EQ(sortedColorTable.getGreenChannel()[3], 0);
    EXPECT_EQ(sortedColorTable.getBlueChannel()[3], 128);
}

// Caso de prueba para `rebuildColorIndex()`
TEST(ImageSoATest, RebuildColorIndex) {
    ColorChannels sortedColorTable(TEST_SIZE);
    sortedColorTable.getRedChannel() = {0, 128, 255, 255};
    sortedColorTable.getGreenChannel() = {128, 255, 255, 0};
    sortedColorTable.getBlueChannel() = {255, 0, 128, 128};

    auto colorIndex = rebuildColorIndex(sortedColorTable);

    EXPECT_EQ(colorIndex["0,128,255"], 0);
    EXPECT_EQ(colorIndex["128,255,0"], 1);
    EXPECT_EQ(colorIndex["255,255,128"], 2);
    EXPECT_EQ(colorIndex["255,0,128"], 3);
}

// Caso de prueba para `createColorTableSoA()`
TEST(ImageSoATest, CreateColorTableSoA) {
    ColorChannels channels(TEST_SIZE);
    channels.getRedChannel() = {255, 0, 255, 128};
    channels.getGreenChannel() = {0, 255, 128, 255};
    channels.getBlueChannel() = {128, 128, 0, 255};

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
    colorTable.getRedChannel() = {255, 128};
    colorTable.getGreenChannel() = {0, 255};
    colorTable.getBlueChannel() = {128, 64};

    std::vector<uint8_t> compressedData;
    PPMHeader header{2, 2, MAX_COLOR_8BIT};
    appendColorTableSoA(compressedData, colorTable, header);

    ASSERT_EQ(compressedData.size(), 6);
    EXPECT_EQ(compressedData[0], 255);
    EXPECT_EQ(compressedData[1], 0);
    EXPECT_EQ(compressedData[2], 128);
    EXPECT_EQ(compressedData[3], 128);
    EXPECT_EQ(compressedData[4], 255);
    EXPECT_EQ(compressedData[5], 64);
}

// Caso de prueba para `appendPixelIndicesSoA()`
TEST(ImageSoATest, AppendPixelIndicesSoA) {
    ColorChannels channels(2);
    channels.getRedChannel() = {255, 128};
    channels.getGreenChannel() = {0, 255};
    channels.getBlueChannel() = {128, 64};

    std::unordered_map<std::string, int> colorIndex = {
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

// TODO
// MAX LEVEL
// ARGS RESIZE
// ARGS CUTFREQ
