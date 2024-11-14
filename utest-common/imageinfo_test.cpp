#include "common/imageinfo.hpp"
#include <gtest/gtest.h>
#include <string>
#include <fstream>

// Caso de prueba para `readPPMHeader` con archivo PPM válido
TEST(ImageInfoTest, ReadPPMHeader_Valid) {
    std::string const ppmData = "P6\n4 4\n255\n";
    std::ofstream outFile("test_valid.ppm", std::ios::binary);
    outFile << ppmData;
    outFile.close();

    PPMHeader header{};
    EXPECT_TRUE(readPPMHeader("test_valid.ppm", header));
    EXPECT_EQ(header.width, 4);
    EXPECT_EQ(header.height, 4);
    EXPECT_EQ(header.maxColorValue, 255);

    int const removeResult = std::remove("test_valid.ppm");
    EXPECT_EQ(removeResult, 0) << "Error: No se pudo eliminar el archivo temporal 'test_valid.ppm'.";
}

// Caso de prueba para `readPPMHeader` con formato no soportado
TEST(ImageInfoTest, ReadPPMHeader_InvalidFormat) {
    std::string const ppmData = "P3\n4 4\n255\n";
    std::ofstream outFile("test_invalid.ppm", std::ios::binary);
    outFile << ppmData;
    outFile.close();

    PPMHeader header{};
    EXPECT_FALSE(readPPMHeader("test_invalid.ppm", header));

    int const removeResult = std::remove("test_invalid.ppm");
    EXPECT_EQ(removeResult, 0) << "Error: No se pudo eliminar el archivo temporal 'test_invalid.ppm'.";
}

// Caso de prueba para `readPPMHeader` con dimensiones inválidas
TEST(ImageInfoTest, ReadPPMHeader_InvalidDimensions) {
    std::string const ppmData = "P6\n-4 4\n255\n";
    std::ofstream outFile("test_invalid_dims.ppm", std::ios::binary);
    outFile << ppmData;
    outFile.close();

    PPMHeader header{};
    EXPECT_FALSE(readPPMHeader("test_invalid_dims.ppm", header));

    int const removeResult = std::remove("test_invalid_dims.ppm");
    EXPECT_EQ(removeResult, 0) << "Error: No se pudo eliminar el archivo temporal 'test_invalid_dims.ppm'.";
}

// Caso de prueba para `generateHeader`
TEST(ImageInfoTest, GenerateHeader) {
    constexpr PPMHeader header{4, 4, 255};
    constexpr std::string_view expected = "C6 4 4 255 10\n";
    std::string const result = generateHeader(header, 10);
    EXPECT_EQ(result, expected);
}

// Caso de prueba para `ensureCppmExtension` con nombre de archivo sin extensión
TEST(ImageInfoTest, EnsureCppmExtension_NoExtension) {
    std::string const filename = "output/test";
    std::string const result = ensureCppmExtension(filename);
    EXPECT_EQ(result, "output/test.cppm");
}

// Caso de prueba para `ensureCppmExtension` con nombre de archivo con extensión `.cppm`
TEST(ImageInfoTest, EnsureCppmExtension_WithExtension) {
    std::string const filename = "output/test.cppm";
    std::string const result = ensureCppmExtension(filename);
    EXPECT_EQ(result, "output/test.cppm");
}

// Caso de prueba para `processInfo` (integración con `readPPMHeader`)
TEST(ImageInfoTest, ProcessInfo_ValidFile) {
    std::string const ppmData = "P6\n4 4\n255\n";
    std::ofstream outFile("test_process.ppm", std::ios::binary);
    outFile << ppmData;
    outFile.close();

    testing::internal::CaptureStdout();
    processInfo("test_process.ppm");
    std::string const output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("Ancho: 4"), std::string::npos);
    EXPECT_NE(output.find("Alto: 4"), std::string::npos);
    EXPECT_NE(output.find("Valor Máximo de Color: 255"), std::string::npos);

    int const removeResult = std::remove("test_process.ppm");
    EXPECT_EQ(removeResult, 0) << "Error: No se pudo eliminar el archivo temporal 'test_process.ppm'.";
}
