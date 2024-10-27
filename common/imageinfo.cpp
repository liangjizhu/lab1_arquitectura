//
// Created by liang on 27/10/24.
//
#include "imageinfo.hpp"
#include <iostream>
#include <fstream>
#include <limits>

void processInfo(const std::string& inputFile) {
    std::ifstream file(inputFile, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo de entrada: " << inputFile << std::endl;
        return;
    }

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

    file.seekg(0, std::ios::end);
    std::streamoff fileSize = file.tellg();
    int pixelSize = maxColorValue > 255 ? 6 : 3;
    int imageSize = width * height * pixelSize;
    file.seekg(std::ios::beg);

    if (fileSize < imageSize) {
        std::cerr << "Error: El archivo parece estar incompleto o dañado." << std::endl;
    } else {
        std::cout << "El archivo contiene suficientes datos de píxeles." << std::endl;
    }

    file.close();
}
