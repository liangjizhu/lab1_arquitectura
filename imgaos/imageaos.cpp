//
// Created by liang on 4/10/24.
//

#include "imageaos.hpp"
#include <iostream>
#include <fstream>

void processInfo(const std::string& inputFile) {
    // Lógica para el comando 'info'
    std::ifstream file(inputFile);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open input file: " << inputFile << std::endl;
        return;
    }

    // Aquí procesas el archivo PPM para extraer información
    std::cout << "Processing 'info' for file: " << inputFile << std::endl;
    // Ejemplo de lo que podrías imprimir (análisis del archivo PPM)
}

void processMaxLevel(const std::string& inputFile, int maxLevel) {
    // Lógica para el comando 'maxlevel'
    std::cout << "Processing 'maxlevel' for file: " << inputFile << " with max level: " << maxLevel << std::endl;
    // Aquí iría la lógica para modificar el nivel máximo del archivo
}

// TODO
