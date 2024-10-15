//
// Created by liang on 4/10/24.
//

#include "progargs.hpp"
#include <stdexcept>
#include <iostream>

ProgramArgs::ProgramArgs(int argc, char* argv[]) {
    for (int i = 0; i < argc; ++i) {
        args.push_back(argv[i]);
    }
}

bool ProgramArgs::validate() {
    if (args.size() < 2) {
        errorMessage = "Error: Invalid number of arguments";
        return false;
    }

    // El primer argumento es el nombre del programa, el segundo es el comando
    operation = args[1];

    // Validar el comando
    if (operation == "info" || operation == "maxlevel" || operation == "resize" ||
        operation == "cutfreq" || operation == "compress") {

        // Validar si hay al menos un archivo de entrada
        if (args.size() < 3) {
            errorMessage = "Error: Missing input file for the operation: " + operation;
            return false;
        }
        inputFile = args[2];
        return true;
    } else {
        errorMessage = "Error: Invalid operation: " + operation;
        return false;
    }
}

bool ProgramArgs::validateInfo() {
    if (args.size() != 3) {
        errorMessage = "Error: Invalid extra arguments for info";
        return false;
    }
    return true;
}

bool ProgramArgs::validateMaxLevel() {
    if (args.size() != 4) {
        errorMessage = "Error: Invalid number of extra arguments for maxlevel";
        return false;
    }
    try {
        int maxLevel = std::stoi(args[3]);
        if (maxLevel < 0 || maxLevel > 65535) {
            errorMessage = "Error: Invalid maxlevel: " + args[3];
            return false;
        }
    } catch (const std::invalid_argument&) {
        errorMessage = "Error: Invalid maxlevel: " + args[3];
        return false;
    }
    return true;
}

bool ProgramArgs::validateResize() {
    if (args.size() != 5) {
        errorMessage = "Error: Invalid number of extra arguments for resize";
        return false;
    }
    try {
        int width = std::stoi(args[3]);
        int height = std::stoi(args[4]);
        if (width <= 0 || height <= 0) {
            errorMessage = "Error: Invalid resize dimensions";
            return false;
        }
    } catch (const std::invalid_argument&) {
        errorMessage = "Error: Invalid resize dimensions";
        return false;
    }
    return true;
}

bool ProgramArgs::validateCutFreq() {
    if (args.size() != 4) {
        errorMessage = "Error: Invalid number of extra arguments for cutfreq";
        return false;
    }
    try {
        int freq = std::stoi(args[3]);
        if (freq <= 0) {
            errorMessage = "Error: Invalid cutfreq: " + args[3];
            return false;
        }
    } catch (const std::invalid_argument&) {
        errorMessage = "Error: Invalid cutfreq: " + args[3];
        return false;
    }
    return true;
}

bool ProgramArgs::validateCompress() {
    if (args.size() != 3) {
        errorMessage = "Error: Invalid extra arguments for compress";
        return false;
    }
    return true;
}

std::string ProgramArgs::getInputFile() const {
    return args[2];  // Cambiado a args[2] para el archivo de entrada
}

std::string ProgramArgs::getOperation() const {
    return args[1];  // Cambiado a args[1] para el comando
}

std::string ProgramArgs::getErrorMessage() const {
    return errorMessage;
}

int ProgramArgs::getMaxLevel() const {
    return std::stoi(args[3]);
}

int ProgramArgs::getResizeWidth() const {
    return std::stoi(args[3]);
}

int ProgramArgs::getResizeHeight() const {
    return std::stoi(args[4]);
}

int ProgramArgs::getCutFreq() const {
    return std::stoi(args[3]);
}
