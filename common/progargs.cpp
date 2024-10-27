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
  if (args.size() < 3) {
    errorMessage = "Error: Invalid number of arguments";
    return false;
  }

  // `info` tiene tres argumentos: nombre del programa, archivo de entrada, y comando.
  if (args[2] == "info") {
    if (args.size() != 3) {
      errorMessage = "Error: Invalid number of arguments for 'info'";
      return false;
    }
    operation = args[2];
    inputFile = args[1];
    return true;
  }

  // Las demás operaciones tienen cuatro argumentos: nombre del programa, archivo de entrada, operación y archivo de salida.
  if (args.size() < 4) {
    errorMessage = "Error: Invalid number of arguments";
    return false;
  }

  operation = args[3];

  // Validar el comando
  if (operation == "maxlevel" || operation == "resize" || operation == "cutfreq" || operation == "compress") {
    inputFile = args[1];
    outputFile = args[2];  // Para las operaciones que requieren archivo de salida

    if (operation == "compress" && args.size() != 4) {
      errorMessage = "Error: Invalid arguments for 'compress'";
      return false;
    }

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
    if (args.size() != 4) {  // Verifica que haya cuatro argumentos en total para `compress`
        errorMessage = "Error: Invalid extra arguments for compress";
        return false;
    }
    return true;
}

std::string ProgramArgs::getInputFile() const {
    return inputFile;
}

std::string ProgramArgs::getOutputFile() const {
  return outputFile;
}

std::string ProgramArgs::getOperation() const {
    return operation;
}

std::string ProgramArgs::getErrorMessage() const {
    return errorMessage;
}

int ProgramArgs::getMaxLevel() const {
    try {
        return std::stoi(args[3]);
    } catch (const std::exception& e) {
        std::cerr << "Error: Invalid max level value: " << e.what() << std::endl;
        return -1;
    }
}

int ProgramArgs::getResizeWidth() const {
    try {
        return std::stoi(args[3]);
    } catch (const std::exception& e) {
        std::cerr << "Error: Invalid width value: " << e.what() << std::endl;
        return -1;
    }
}

int ProgramArgs::getResizeHeight() const {
    try {
        return std::stoi(args[4]);
    } catch (const std::exception& e) {
        std::cerr << "Error: Invalid height value: " << e.what() << std::endl;
        return -1;
    }
}

int ProgramArgs::getCutFreq() const {
    try {
        return std::stoi(args[3]);
    } catch (const std::exception& e) {
        std::cerr << "Error: Invalid cut frequency value: " << e.what() << std::endl;
        return -1;
    }
}
