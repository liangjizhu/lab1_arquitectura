/*#include "imgsoa/color.hpp"
#include "common/imageinfo.hpp"
#include <gtest/gtest.h>
#include <vector>

// Constantes
constexpr size_t TEST_CAPACITY = 10;
constexpr size_t TEST_SIZE = 5;
constexpr size_t TEST_BUFFER_SIZE_8BIT = 6;
constexpr size_t TEST_BUFFER_SIZE_16BIT = 12;
constexpr uint16_t RED_VALUE_1 = 256;
constexpr uint16_t RED_VALUE_2 = 512;
constexpr uint16_t GREEN_VALUE_1 = 1024;
constexpr uint16_t GREEN_VALUE_2 = 2048;
constexpr uint16_t BLUE_VALUE_1 = 4096;
constexpr uint16_t BLUE_VALUE_2 = 8192;
constexpr uint16_t MAX_COLOR_8BIT = 255;
constexpr uint16_t MID_COLOR_VALUE_8BIT = 128;
constexpr uint16_t LOW_COLOR_VALUE_8BIT = 64;
constexpr uint16_t EXAMPLE_RED_COLOR_VALUE = 100;
constexpr uint16_t EXAMPLE_GREEN_COLOR_VALUE = 150;
constexpr uint16_t EXAMPLE_BLUE_COLOR_VALUE = 200;
constexpr uint16_t EXAMPLE_DIFF_BLUE_COLOR_VALUE = 201;

// Caso de prueba para el constructor y el metodo reserve()
TEST(ColorChannelsTest, ConstructorAndReserve) {
    ColorChannels channels(TEST_CAPACITY);
    EXPECT_EQ(channels.getRedChannel().capacity(), TEST_CAPACITY);
    EXPECT_EQ(channels.getGreenChannel().capacity(), TEST_CAPACITY);
    EXPECT_EQ(channels.getBlueChannel().capacity(), TEST_CAPACITY);
}

// Caso de prueba para `extractFromBinary()` con datos de 8 bits
TEST(ColorChannelsTest, ExtractFromBinary_8Bit) {
    constexpr PPMHeader header{2, 2, MAX_COLOR_8BIT};
    std::vector<uint8_t> const fileData = {
        MAX_COLOR_8BIT, 0, 0, 0, MAX_COLOR_8BIT, 0,
        0, 0, MAX_COLOR_8BIT, MAX_COLOR_8BIT, MAX_COLOR_8BIT, MAX_COLOR_8BIT
    };

    ColorChannels channels(4);
    channels.extractFromBinary(fileData, header);

    EXPECT_EQ(channels.getRedChannel()[0], MAX_COLOR_8BIT);
    EXPECT_EQ(channels.getGreenChannel()[1], MAX_COLOR_8BIT);
    EXPECT_EQ(channels.getBlueChannel()[2], MAX_COLOR_8BIT);
    EXPECT_EQ(channels.getRedChannel()[3], MAX_COLOR_8BIT);
    EXPECT_EQ(channels.getGreenChannel()[3], MAX_COLOR_8BIT);
    EXPECT_EQ(channels.getBlueChannel()[3], MAX_COLOR_8BIT);
}

// Caso de prueba para `writeToBinary()` con datos de 8 bits
TEST(ColorChannelsTest, WriteToBinary_8Bit) {
    constexpr PPMHeader header{2, 2, MAX_COLOR_8BIT};
    ColorChannels channels(2);
    channels.getRedChannel() = {MAX_COLOR_8BIT, MID_COLOR_VALUE_8BIT};
    channels.getGreenChannel() = {0, MAX_COLOR_8BIT};
    channels.getBlueChannel() = {LOW_COLOR_VALUE_8BIT, MID_COLOR_VALUE_8BIT};

    std::vector<uint8_t> buffer;
    channels.writeToBinary(buffer, header);

    ASSERT_EQ(buffer.size(), TEST_BUFFER_SIZE_8BIT);
    EXPECT_EQ(buffer[0], MAX_COLOR_8BIT);
    EXPECT_EQ(buffer[1], 0);
    EXPECT_EQ(buffer[2], LOW_COLOR_VALUE_8BIT);
    EXPECT_EQ(buffer[3], MID_COLOR_VALUE_8BIT);
    EXPECT_EQ(buffer[4], MAX_COLOR_8BIT);
    EXPECT_EQ(buffer[5], MID_COLOR_VALUE_8BIT);
}

// Caso de prueba para `areEqual()`
TEST(ColorChannelsTest, AreEqual) {
    ColorChannels channels(2);
    channels.getRedChannel() = {EXAMPLE_RED_COLOR_VALUE, EXAMPLE_RED_COLOR_VALUE};
    channels.getGreenChannel() = {EXAMPLE_GREEN_COLOR_VALUE, EXAMPLE_GREEN_COLOR_VALUE};
    channels.getBlueChannel() = {EXAMPLE_BLUE_COLOR_VALUE, EXAMPLE_BLUE_COLOR_VALUE};

    EXPECT_TRUE(channels.areEqual(0, 1));
    channels.getBlueChannel()[1] = EXAMPLE_DIFF_BLUE_COLOR_VALUE;
    EXPECT_FALSE(channels.areEqual(0, 1));
}

// Caso de prueba para `size()`
TEST(ColorChannelsTest, Size) {
    ColorChannels channels(TEST_SIZE);
    channels.getRedChannel().resize(TEST_SIZE);
    channels.getGreenChannel().resize(TEST_SIZE);
    channels.getBlueChannel().resize(TEST_SIZE);

    EXPECT_EQ(channels.size(), TEST_SIZE);
}*/