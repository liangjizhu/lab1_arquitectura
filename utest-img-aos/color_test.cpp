#include "imgaos/color.hpp"
#include "common/imageinfo.hpp"
#include <gtest/gtest.h>
#include <vector>
#include <array>

constexpr uint16_t MAX_COLOR_8BIT = 255;
constexpr uint16_t MID_COLOR_VALUE_8BIT = 128;
constexpr uint16_t LOW_COLOR_VALUE_8BIT = 64;
constexpr int COLOR_SIZE = 6;
constexpr uint16_t MAX_COLOR_VALUE_16BIT = 65535;

// Caso de prueba para la construcción del objeto Color
TEST(ColorTest, Constructor) {
    constexpr RGBColor rgb = {MAX_COLOR_8BIT, MID_COLOR_VALUE_8BIT, LOW_COLOR_VALUE_8BIT};
    Color const color(rgb);
    EXPECT_EQ(color.rgb.red, MAX_COLOR_8BIT);
    EXPECT_EQ(color.rgb.green, MID_COLOR_VALUE_8BIT);
    EXPECT_EQ(color.rgb.blue, LOW_COLOR_VALUE_8BIT);
}

// Caso de prueba para `fromBinary` con datos de 8 bits
TEST(ColorTest, FromBinary_8Bit) {
    std::array<uint8_t, 3> data = {MAX_COLOR_8BIT, MID_COLOR_VALUE_8BIT, LOW_COLOR_VALUE_8BIT};
    constexpr PPMHeader header{4, 4, MAX_COLOR_8BIT};
    Color const color = Color::fromBinary(data.data(), header);

    EXPECT_EQ(color.rgb.red, MAX_COLOR_8BIT);
    EXPECT_EQ(color.rgb.green, MID_COLOR_VALUE_8BIT);
    EXPECT_EQ(color.rgb.blue, LOW_COLOR_VALUE_8BIT);
}

// Caso de prueba para `fromBinary` con datos de 16 bits
TEST(ColorTest, FromBinary_16Bit) {
    std::array<uint8_t, COLOR_SIZE> data = {0x01, 0x00, 0x02, 0x00, 0x03, 0x00};
    constexpr PPMHeader header{4, 4, MAX_COLOR_VALUE_16BIT};
    Color const color = Color::fromBinary(data.data(), header);

    EXPECT_EQ(color.rgb.red, 1);
    EXPECT_EQ(color.rgb.green, 2);
    EXPECT_EQ(color.rgb.blue, 3);
}

// Caso de prueba para `writeToBinary` con datos de 8 bits
TEST(ColorTest, WriteToBinary_8Bit) {
    constexpr RGBColor rgb = {MAX_COLOR_8BIT, MID_COLOR_VALUE_8BIT, LOW_COLOR_VALUE_8BIT};
    Color const color(rgb);
    constexpr PPMHeader header{4, 4, MAX_COLOR_8BIT};
    std::vector<uint8_t> buffer;

    color.writeToBinary(buffer, header);

    ASSERT_EQ(buffer.size(), 3);
    EXPECT_EQ(buffer[0], MAX_COLOR_8BIT);
    EXPECT_EQ(buffer[1], MID_COLOR_VALUE_8BIT);
    EXPECT_EQ(buffer[2], LOW_COLOR_VALUE_8BIT);
}

// Caso de prueba para `writeToBinary` con datos de 16 bits
TEST(ColorTest, WriteToBinary_16Bit) {
    constexpr RGBColor rgb = {256, 512, 1024};
    Color const color(rgb);
    constexpr PPMHeader header{4, 4, MAX_COLOR_VALUE_16BIT};
    std::vector<uint8_t> buffer;

    color.writeToBinary(buffer, header);

    ASSERT_EQ(buffer.size(), 6);
    EXPECT_EQ(buffer[0], 0);
    EXPECT_EQ(buffer[1], 1);
    EXPECT_EQ(buffer[2], 0);
    EXPECT_EQ(buffer[3], 2);
    EXPECT_EQ(buffer[4], 0);
    EXPECT_EQ(buffer[5], 4);
}

// Caso de prueba para el operador de igualdad
TEST(ColorTest, EqualityOperator) {
    RGBColor constexpr rgb1 = {MAX_COLOR_8BIT, MID_COLOR_VALUE_8BIT, LOW_COLOR_VALUE_8BIT};
    RGBColor constexpr rgb2 = {MAX_COLOR_8BIT, MID_COLOR_VALUE_8BIT, LOW_COLOR_VALUE_8BIT};
    RGBColor constexpr rgb3 = {MID_COLOR_VALUE_8BIT, MAX_COLOR_8BIT, LOW_COLOR_VALUE_8BIT};

    Color const color1(rgb1);
    Color const color2(rgb2);
    Color const color3(rgb3);

    EXPECT_TRUE(color1 == color2);
    EXPECT_FALSE(color1 == color3);
}

// Caso de prueba para la función hash
TEST(ColorTest, HashFunction) {
    constexpr RGBColor rgb = {MAX_COLOR_8BIT, MID_COLOR_VALUE_8BIT, LOW_COLOR_VALUE_8BIT};
    Color const color(rgb);

    std::hash<Color> constexpr hasher;
    size_t const hashValue = hasher(color);

    // Verificar que la función hash no sea cero
    EXPECT_NE(hashValue, 0);
}