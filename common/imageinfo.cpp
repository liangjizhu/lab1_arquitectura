#include "imageinfo.hpp"
#include "binaryio.hpp"
#include <iostream>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <fstream>

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

// COMPRESS
// Reutilizar: Generar encabezado del archivo comprimido
std::string generateHeader(const PPMHeader& header, int colorTableSize) {
    std::ostringstream headerStream;
    headerStream << "C6 " << header.width << " " << header.height << " " << header.maxColorValue << " " << colorTableSize << "\n";
    return headerStream.str();
}

// Reutilizar: Función para asegurar que el archivo tenga la extensión ".cppm"
std::string ensureCppmExtension(const std::string& filename) {
    if (filename.find(".cppm") == std::string::npos) {
        return filename + ".cppm";
    }
    return filename;
}
// Define getImageDimensions to read image width and height
std::pair<int, int> getImageDimensions(const std::string& filename) {
  std::ifstream file(filename, std::ios::binary);
  if (!file.is_open()) {
    throw std::runtime_error("Error: Unable to open file " + filename);
  }

  std::string format;
  int width, height, maxColorValue;

  // Read the format identifier
  file >> format;
  std::cout << "Format: " << format << std::endl;
  if (format != "P6") {
    throw std::runtime_error("Error: Unsupported PPM format (must be P6)");
  }

  // Read width, height, and max color value
  file >> width >> height >> maxColorValue;
  std::cout << "Width: " << width << ", Height: " << height << ", Max Color Value: " << maxColorValue << std::endl;

  if (width <= 0 || height <= 0 || maxColorValue != 255) {
    throw std::runtime_error("Error: Dimensiones o valor máximo de color no válidos.");
  }

  // Skip the newline after the header
  file.ignore(256, '\n');

  return {width, height};
}


