#include "imageinfo.hpp"
#include "binaryio.hpp"
#include <iostream>
#include <vector>
#include <sstream>
#include <stdexcept>

bool readPPMHeader(const std::string& inputFile, PPMHeader& header) {
    std::vector<uint8_t> fileData;
    try {
        fileData = BinaryIO::readBinaryFile(inputFile);
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << '\n';
        return false;
    }

    std::istringstream fileStream(std::string(fileData.begin(), fileData.end()), std::ios::binary);

    std::string format;
    fileStream >> format;
    if (format != "P6") {
        std::cerr << "Error: Formato PPM no soportado. Solo se soporta P6." << '\n';
        return false;
    }

    fileStream >> header.width >> header.height >> header.maxColorValue;
    if (header.width <= 0 || header.height <= 0) {
        std::cerr << "Error: Dimensiones o valor máximo de color no válidos." << '\n';
        return false;
    }

    fileStream.ignore(1);  // Ignorar el salto de línea después del encabezado
    return true;
}

void processInfo(const std::string& inputFile) {
    PPMHeader header{};
    if (!readPPMHeader(inputFile, header)) {
        std::cerr << "Error al leer el encabezado del archivo PPM." << '\n';
        return;
    }

    std::cout << "Archivo: " << inputFile << '\n';
    std::cout << "Formato: P6" << '\n';
    std::cout << "Ancho: " << header.width << '\n';
    std::cout << "Alto: " << header.height << '\n';
    std::cout << "Valor Máximo de Color: " << header.maxColorValue << '\n';

    unsigned int const uPixelSize = header.maxColorValue > 255 ? 6U : 3U;
    unsigned int const imageSize = static_cast<unsigned int>(header.width) * static_cast<unsigned int>(header.height) * uPixelSize;

    std::vector<uint8_t> const fileData = BinaryIO::readBinaryFile(inputFile);
    if (fileData.size() < imageSize) {
        std::cerr << "Error: El archivo parece estar incompleto o dañado." << '\n';
    } else {
        std::cout << "El archivo contiene suficientes datos de píxeles." << '\n';
    }
}
