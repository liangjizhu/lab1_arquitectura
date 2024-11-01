#include "imageinfo.hpp"
#include "binaryio.hpp"
#include <iostream>
#include <vector>
#include <sstream>  // Para std::istringstream
#include <limits>

void processInfo(const std::string& inputFile) {
    // Leer el archivo completo como un vector de bytes
    std::vector<uint8_t> fileData;
    try {
        fileData = BinaryIO::readBinaryFile(inputFile);
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return;
    }

    // Crear un stream para leer desde el vector de bytes
    std::istringstream file(std::string(fileData.begin(), fileData.end()), std::ios::binary);

    std::string format;
    int width, height, maxColorValue;

    file >> format;
    if (format != "P6") {
        std::cerr << "Error: Formato PPM no soportado. Solo se soporta P6." << std::endl;
        return;
    }

    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    file >> width >> height >> maxColorValue;

    if (width <= 0 || height <= 0 || maxColorValue <= 0 || maxColorValue > 65535) {
        std::cerr << "Error: Dimensiones o valor máximo de color no válidos." << std::endl;
        return;
    }

    file.ignore(1);
    std::cout << "Archivo: " << inputFile << std::endl;
    std::cout << "Formato: " << format << std::endl;
    std::cout << "Ancho: " << width << std::endl;
    std::cout << "Alto: " << height << std::endl;
    std::cout << "Valor Máximo de Color: " << maxColorValue << std::endl;

    // Convertir los valores a unsigned antes de multiplicar
    unsigned int uWidth = static_cast<unsigned int>(width);
    unsigned int uHeight = static_cast<unsigned int>(height);
    unsigned int uPixelSize = maxColorValue > 255 ? 6U : 3U;
    unsigned int imageSize = uWidth * uHeight * uPixelSize;

    if (fileData.size() < imageSize) {
        std::cerr << "Error: El archivo parece estar incompleto o dañado." << std::endl;
    } else {
        std::cout << "El archivo contiene suficientes datos de píxeles." << std::endl;
    }
}
