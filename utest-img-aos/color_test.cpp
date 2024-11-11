#include "imgaos/color.hpp"
#include "common/imageinfo.hpp"
#include <gtest/gtest.h>
#include <vector>
#include <array>

// Caso de prueba para la construcción del objeto Color
TEST(ColorTest, Constructor) {
    constexpr RGBColor rgb = {255, 128, 64};
    Color const color(rgb);
    EXPECT_EQ(color.rgb.red, 255);
    EXPECT_EQ(color.rgb.green, 128);
    EXPECT_EQ(color.rgb.blue, 64);
}

// Caso de prueba para `fromBinary` con datos de 8 bits
TEST(ColorTest, FromBinary_8Bit) {
    std::array<uint8_t, 3> data = {255, 128, 64};
    PPMHeader header{4, 4, 255};
    Color color = Color::fromBinary(data.data(), header);

    EXPECT_EQ(color.rgb.red, 255);
    EXPECT_EQ(color.rgb.green, 128);
    EXPECT_EQ(color.rgb.blue, 64);
}

// Caso de prueba para `fromBinary` con datos de 16 bits
TEST(ColorTest, FromBinary_16Bit) {
    std::array<uint8_t, 6> data = {0x01, 0x00, 0x02, 0x00, 0x03, 0x00};
    PPMHeader header{4, 4, 65535};
    Color color = Color::fromBinary(data.data(), header);

    EXPECT_EQ(color.rgb.red, 1);
    EXPECT_EQ(color.rgb.green, 2);
    EXPECT_EQ(color.rgb.blue, 3);
}

// Caso de prueba para `writeToBinary` con datos de 8 bits
TEST(ColorTest, WriteToBinary_8Bit) {
    RGBColor rgb = {255, 128, 64};
    Color color(rgb);
    PPMHeader header{4, 4, 255};
    std::vector<uint8_t> buffer;

    color.writeToBinary(buffer, header);

    ASSERT_EQ(buffer.size(), 3);
    EXPECT_EQ(buffer[0], 255);
    EXPECT_EQ(buffer[1], 128);
    EXPECT_EQ(buffer[2], 64);
}

// Caso de prueba para `writeToBinary` con datos de 16 bits
TEST(ColorTest, WriteToBinary_16Bit) {
    RGBColor rgb = {256, 512, 1024};
    Color color(rgb);
    PPMHeader header{4, 4, 65535};
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
    RGBColor rgb1 = {255, 128, 64};
    RGBColor rgb2 = {255, 128, 64};
    RGBColor rgb3 = {128, 255, 64};

    Color color1(rgb1);
    Color color2(rgb2);
    Color color3(rgb3);

    EXPECT_TRUE(color1 == color2);
    EXPECT_FALSE(color1 == color3);
}

// Caso de prueba para la función hash
TEST(ColorTest, HashFunction) {
    RGBColor rgb = {255, 128, 64};
    Color color(rgb);

    std::hash<Color> hasher;
    size_t hashValue = hasher(color);

    // Verificar que la función hash no sea cero
    EXPECT_NE(hashValue, 0);
}
