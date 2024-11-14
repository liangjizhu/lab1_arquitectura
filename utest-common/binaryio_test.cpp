#include "common/binaryio.hpp"
#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <fstream>
#include <cstdio> // Para std::remove

// Caso de prueba para `readBinaryFile` con un archivo válido
TEST(BinaryIOTest, ReadBinaryFile_ValidFile) {
    // Crear un archivo binario temporal
    std::string const filename = "test_read.bin";
    std::vector<uint8_t> const data = {0x01, 0x02, 0x03, 0x04};

    std::ofstream outFile(filename, std::ios::binary);
    outFile.write(std::bit_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));
    outFile.close();

    // Leer el archivo usando `readBinaryFile`
    std::vector<uint8_t> const result = BinaryIO::readBinaryFile(filename);

    // Verificar que los datos leídos sean iguales a los datos escritos
    EXPECT_EQ(result.size(), data.size());
    EXPECT_EQ(result, data);

    // Eliminar el archivo temporal
    int const removeResult = std::remove(filename.c_str());
    EXPECT_EQ(removeResult, 0) << "Error: No se pudo eliminar el archivo temporal '" << filename << "'.";
}

// Caso de prueba para `readBinaryFile` con un archivo no existente
TEST(BinaryIOTest, ReadBinaryFile_NonExistentFile) {
    std::string const filename = "nonexistent.bin";
    EXPECT_THROW(BinaryIO::readBinaryFile(filename), std::runtime_error);
}

// Caso de prueba para `writeBinaryFile`
TEST(BinaryIOTest, WriteBinaryFile_ValidData) {
    std::string const filename = "test_write.bin";
    std::vector<uint8_t> const data = {0xAA, 0xBB, 0xCC, 0xDD};

    // Escribir datos al archivo usando `writeBinaryFile`
    ASSERT_NO_THROW(BinaryIO::writeBinaryFile(filename, data));

    // Verificar que el archivo fue escrito correctamente
    std::ifstream inFile(filename, std::ios::binary);
    ASSERT_TRUE(inFile.is_open());

    std::vector<uint8_t> result(data.size());
    inFile.read(std::bit_cast<char*>(result.data()), static_cast<std::streamsize>(result.size()));
    inFile.close();

    EXPECT_EQ(result, data);

    // Eliminar el archivo temporal
    int const removeResult = std::remove(filename.c_str());
    EXPECT_EQ(removeResult, 0) << "Error: No se pudo eliminar el archivo temporal '" << filename << "'.";
}

// Caso de prueba para `writeBinaryFile` con un archivo no accesible
TEST(BinaryIOTest, WriteBinaryFile_UnwritableFile) {
    std::string const filename = "/invalid_path/test_write.bin";
    std::vector<uint8_t> const data = {0x11, 0x22, 0x33, 0x44};

    EXPECT_THROW(BinaryIO::writeBinaryFile(filename, data), std::runtime_error);
}
