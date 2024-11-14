#include "common/progargs.hpp"
#include <gtest/gtest.h>

// Se asume que hay una carpeta llamada in donde cogeremos el archivo lake-small.pp para realizar pruebas con ella
// Se asume que hay una carpeta llamada output-aos donde hay un archivo test.cppm que es el resultado de las operaciones

// Caso de prueba para argumentos válidos de compresión
TEST(ProgramArgsTest, Validate_ValidArguments_Compress) {
    std::vector<std::string> const args = {"imtool-aos/imtool-aos", "in/lake-small.ppm", "output-aos/test", "compress"};
    ProgramArgs const progArgs(args);
    EXPECT_TRUE(progArgs.validate());
    EXPECT_EQ(progArgs.getInputFile(), "in/lake-small.ppm");
    EXPECT_EQ(progArgs.getOutputFile(), "output-aos/test");
    EXPECT_EQ(progArgs.getOperation(), "compress");
}

// Caso de prueba para argumentos inválidos (falta de parámetros)
TEST(ProgramArgsTest, Validate_InvalidArguments_MissingParams) {
    std::vector<std::string> const args = {"imtool-aos/imtool-aos", "in.ppm"};
    ProgramArgs const progArgs(args);
    EXPECT_FALSE(progArgs.validate());
    EXPECT_EQ(progArgs.getErrorMessage(), "Error: Invalid number of arguments");
}

// Caso de prueba para operación desconocida
TEST(ProgramArgsTest, Validate_InvalidOperation) {
    std::vector<std::string> const args = {"imtool-aos/imtool-aos", "in/lake-small.ppm", "output-aos/test", "unknown"};
    ProgramArgs const progArgs(args);
    EXPECT_FALSE(progArgs.validate());
    EXPECT_EQ(progArgs.getErrorMessage(), "Error: Invalid operation: unknown");
}

// Caso de prueba para validar 'info' con argumentos válidos
TEST(ProgramArgsTest, ValidateInfo_Valid) {
    std::vector<std::string> const args = {"imtool-aos/imtool-aos", "in/lake-small.ppm", "info"};
    ProgramArgs const progArgs(args);
    EXPECT_TRUE(progArgs.validateInfo());
}

// Caso de prueba para 'compress' con argumentos válidos
TEST(ProgramArgsTest, ValidateCompress_Valid) {
    std::vector<std::string> const args = {"imtool-aos/imtool-aos", "in/lake-small.ppm", "output-aos/test", "compress"};
    ProgramArgs const progArgs(args);
    EXPECT_TRUE(progArgs.validateCompress());
}

// Caso de prueba para 'compress' con argumentos inválidos
TEST(ProgramArgsTest, ValidateCompress_Invalid) {
    std::vector<std::string> const args = {"imtool-aos/imtool-aos", "in/lake-small.ppm", "output-aos/test", "compress", "100"};
    ProgramArgs const progArgs(args);
    EXPECT_FALSE(progArgs.validateCompress());
    EXPECT_EQ(progArgs.getErrorMessage(), "Error: Invalid extra arguments for compress");
}

// Caso de prueba para obtener rutas de archivos
TEST(ProgramArgsTest, GetFilePaths) {
    std::vector<std::string> const args = {"imtool-aos/imtool-aos", "in/lake-small.ppm", "output-aos/test", "compress"};
    ProgramArgs const progArgs(args);
    EXPECT_TRUE(progArgs.validate());
    auto filePaths = progArgs.getFilePaths();
    ASSERT_TRUE(filePaths.has_value());
    EXPECT_EQ(filePaths->inputFile, "in/lake-small.ppm");
    EXPECT_EQ(filePaths->outputFile, "output-aos/test");
}

// Caso de prueba para operación no soportada en getFilePaths
TEST(ProgramArgsTest, GetFilePaths_NoValue) {
    std::vector<std::string> const args = {"imtool-aos/imtool-aos", "input.ppm", "info"};
    ProgramArgs const progArgs(args);
    EXPECT_TRUE(progArgs.validate());
    EXPECT_FALSE(progArgs.getFilePaths().has_value());
}

// TODO
// VALIDATE ARGS MAX LEVEL
// VALIDATE ARGS RESIZE
// VALIDATE ARGS CUTFREQ
