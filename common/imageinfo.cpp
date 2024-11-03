#include "imageinfo.hpp"
#include "binaryio.hpp"
#include <iostream>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <limits>

bool readPPMHeader(const std::string& inputFile, PPMHeader& header) {
    std::vector<uint8_t> fileData;
    try {
        fileData = BinaryIO::readBinaryFile(inputFile);
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }

    std::istringstream fileStream(std::string(fileData.begin(), fileData.end()), std::ios::binary);

    std::string format;
    fileStream >> format;
    if (format != "P6") {
        std::cerr << "Error: Formato PPM no soportado. Solo se soporta P6." << std::endl;
        return false;
    }

    fileStream >> header.width >> header.height >> header.maxColorValue;
    if (header.width <= 0 || header.height <= 0) {
        std::cerr << "Error: Dimensiones o valor máximo de color no válidos." << std::endl;
        return false;
    }

    fileStream.ignore(1);  // Ignorar el salto de línea después del encabezado
    return true;
}

void processInfo(const std::string& inputFile) {
    PPMHeader header;
    if (!readPPMHeader(inputFile, header)) {
        std::cerr << "Error al leer el encabezado del archivo PPM." << std::endl;
        return;
    }

    std::cout << "Archivo: " << inputFile << std::endl;
    std::cout << "Formato: P6" << std::endl;
    std::cout << "Ancho: " << header.width << std::endl;
    std::cout << "Alto: " << header.height << std::endl;
    std::cout << "Valor Máximo de Color: " << header.maxColorValue << std::endl;

    unsigned int uPixelSize = header.maxColorValue > 255 ? 6U : 3U;
    unsigned int imageSize = static_cast<unsigned int>(header.width) * static_cast<unsigned int>(header.height) * uPixelSize;

    std::vector<uint8_t> fileData = BinaryIO::readBinaryFile(inputFile);
    if (fileData.size() < imageSize) {
        std::cerr << "Error: El archivo parece estar incompleto o dañado." << std::endl;
    } else {
        std::cout << "El archivo contiene suficientes datos de píxeles." << std::endl;
    }
}
