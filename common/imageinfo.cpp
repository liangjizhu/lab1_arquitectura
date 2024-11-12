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
        std::cerr << "Error: No se pudo abrir el archivo de entrada: " << inputFile << "\n";
        return;
    }
    std::string format;
    int width=0;
    int height=0;
    int maxColorValue=0;
    int zero = 0;
    file >> format;
    if (format != "P6") {
        std::cerr << "Error: Formato PPM no soportado. Solo se soporta P6." << "\n";
        return;
    }
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    file >> width >> height >> maxColorValue;
    if (width <= 0 || height <= 0 || maxColorValue <= 0 || maxColorValue > 65535) {
        std::cerr << "Error: Dimensiones o valor máximo de color no válidos." << "\n";
        return;
    }
    file.ignore(1);
    std::cout << "Archivo: " << inputFile << "\n";
    std::cout << "Formato: " << format << "\n";
    std::cout << "Ancho: " << width << "\n";
    std::cout << "Alto: " << height << "\n";
    std::cout << "Valor Máximo de Color: " << maxColorValue << "\n";
    file.seekg(0, std::ios::end);
    std::streamoff fileSize = file.tellg();
    int pixelSize = maxColorValue > 255 ? 6 : 3;
    int imageSize = width * height * pixelSize;
    file.seekg(std::ios::beg);

    if (fileSize < imageSize) {
        std::cerr << "Error: El archivo parece estar incompleto o dañado." << "\n";
    } else {
        std::cout << "El archivo contiene suficientes datos de píxeles." << "\n";
    }

    file.close();
}
