#include <gtest/gtest.h>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cstdio>
#include <memory>

constexpr int BUFFER_SIZE = 128;
// Helper para leer la salida del comando
using PcloseFunc = int (*)(FILE*);

std::string getCommandOutput(const std::string& command) {
    std::string const commandWithRedirect = command + " 2>&1";
    std::string buffer(BUFFER_SIZE, '\0');
    std::string result;

    std::unique_ptr<FILE, PcloseFunc> const pipe(popen(commandWithRedirect.c_str(), "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("Error al ejecutar el comando");
    }

    while (fgets(buffer.data(), BUFFER_SIZE, pipe.get()) != nullptr) {
        result += buffer;
    }

    return result;
}

// Test para el comando 'info' con un archivo válido
TEST(CommandHandlersFTest, HandleInfo_Valid) {
    std::string const command = "./imtool-aos/imtool-aos ../in/lake-small.ppm info";
    std::string const output = getCommandOutput(command);

    // Verificar que la salida contiene información básica de la imagen
    EXPECT_NE(output.find("Ancho:"), std::string::npos);
    EXPECT_NE(output.find("Alto:"), std::string::npos);
    EXPECT_NE(output.find("Valor Máximo de Color:"), std::string::npos);
}

// Test para el comando 'info' con un archivo inválido
TEST(CommandHandlersFTest, HandleInfo_InvalidFile) {
    std::string const command = "./imtool-aos/imtool-aos ../in/nonexistent.ppm info";
    std::string const output = getCommandOutput(command);

    // Verificar que se muestra un error
    EXPECT_NE(output.find("Unable to open file"), std::string::npos);
    EXPECT_NE(output.find("Error al leer el encabezado del archivo PPM"), std::string::npos);
}

// Test para el comando 'compress' con un archivo válido
TEST(CommandHandlersFTest, HandleCompress_Valid) {
    std::string const command = "./imtool-aos/imtool-aos ../in/lake-small.ppm ../output-aos/test compress";
    std::string const output = getCommandOutput(command);

    // Verificar que la salida indica compresión exitosa
    EXPECT_NE(output.find("Compressing file..."), std::string::npos);
    EXPECT_NE(output.find("File compressed to"), std::string::npos);

    // Verificar que el archivo comprimido se creó
    std::ifstream outFile("../output-aos/test.cppm");
    EXPECT_TRUE(outFile.is_open());
    outFile.close();
}

// Test para el comando 'compress' con un archivo inválido
TEST(CommandHandlersFTest, HandleCompress_InvalidFile) {
    std::string const command = "./imtool-aos/imtool-aos ../in/nonexistent.ppm ../output-aos/test compress";
    std::string const output = getCommandOutput(command);

    // Verificar que se muestra un error
    EXPECT_NE(output.find("Compressing file..."), std::string::npos);
    EXPECT_NE(output.find("Unable to open file"), std::string::npos);
}

// TODO

