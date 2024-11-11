#include "imgaos/command_handlers.hpp"
#include "common/progargs.hpp"
#include "common/binaryio.hpp"
#include "common/imageinfo.hpp"
#include <gtest/gtest.h>
#include <fstream>
#include <cstdio>

// Caso de prueba para `handleInfo` con argumentos válidos
TEST(CommandHandlersTest, HandleInfo_Valid) {
    std::vector<std::string> args = {"imtool-aos/imtool-aos", "/home/liang/CLionProjects/lab1_arquitectura/input/lake-small.ppm", "info"};
    ProgramArgs progArgs(args);

    EXPECT_EQ(handleInfo(progArgs), 0);
}

// Caso de prueba para `handleInfo` con argumentos inválidos
TEST(CommandHandlersTest, HandleInfo_Invalid) {
    std::vector<std::string> args = {"imtool-aos/imtool-aos", "input/lake-small.ppm", "invalid"};
    ProgramArgs progArgs(args);

    EXPECT_EQ(handleInfo(progArgs), -1);
}

// Caso de prueba para `handleCompress` con argumentos válidos
TEST(CommandHandlersTest, HandleCompress_Valid) {
    std::string const inputFilename = "test_input.ppm";
    std::string const outputFilename = "test_output.cppm";

    // Crear archivo PPM de prueba
    std::ofstream outFile(inputFilename, std::ios::binary);
    ASSERT_TRUE(outFile.is_open());
    outFile << "P6\n2 2\n255\n";
    outFile << static_cast<char>(255) << static_cast<char>(0) << static_cast<char>(0)
            << static_cast<char>(0) << static_cast<char>(255) << static_cast<char>(0)
            << static_cast<char>(0) << static_cast<char>(0) << static_cast<char>(255)
            << static_cast<char>(255) << static_cast<char>(255) << static_cast<char>(255);
    outFile.close();

    std::vector<std::string> args = {"imtool-aos", inputFilename, outputFilename, "compress"};
    ProgramArgs progArgs(args);

    EXPECT_EQ(handleCompress(progArgs), 0);

    // Verificar que el archivo comprimido se ha creado
    std::ifstream inFile(outputFilename, std::ios::binary);
    ASSERT_TRUE(inFile.is_open());
    inFile.close();

    // Eliminar archivos temporales
    std::remove(inputFilename.c_str());
    std::remove(outputFilename.c_str());
}

// Caso de prueba para `handleCompress` con argumentos inválidos
TEST(CommandHandlersTest, HandleCompress_Invalid) {
    std::vector<std::string> args = {"imtool-aos", "input/lake-small.ppm", "output/test.cppm", "compress", "extra_arg"};
    ProgramArgs progArgs(args);

    EXPECT_EQ(handleCompress(progArgs), -1);
}
