#include "progargs.hpp"

#include <cstdint>
#include <stdexcept>
#include <iostream>

constexpr uint16_t MAX_COLOR_VALUE_16BIT = 65535;
constexpr size_t RESIZE_ARGUMENT_COUNT = 5;
constexpr size_t MAX_LEVEL_ARGUMENT_COUNT = 5;

// Constructor para inicializar los argumentos usando std::vector<std::string>
ProgramArgs::ProgramArgs(const std::vector<std::string>& arguments) : args(arguments) {}

bool ProgramArgs::validate() const{
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
    }
        errorMessage = "Error: Invalid operation: " + operation;
        return false;

}

std::string ProgramArgs::getOutputFile() const {
  return outputFile;
}


bool ProgramArgs::validateInfo() const{
    if (args.size() != 3) {
        errorMessage = "Error: Invalid extra arguments for info";
        return false;
    }
    return true;
}

bool ProgramArgs::validateMaxLevel() const {
    if (args.size() != MAX_LEVEL_ARGUMENT_COUNT) {
        errorMessage = "Error: Invalid number of arguments for 'maxlevel'. Expected: 5 (imtool in output-aos maxlevel xxx)";
        return false;
    }
    try {
        int const maxLevel = getMaxLevel();
        if (maxLevel < 0 || maxLevel > MAX_COLOR_VALUE_16BIT) {
            errorMessage = "Error: Invalid maxlevel value: " + std::to_string(maxLevel) + ". It must be between 0 and " + std::to_string(MAX_COLOR_VALUE_16BIT);
            return false;
        }
    } catch (const std::invalid_argument&) {
        errorMessage = "Error: Invalid maxlevel value. It must be an integer.";
        return false;
    } catch (const std::out_of_range&) {
        errorMessage = "Error: Maxlevel value is out of range.";
        return false;
    }
    return true;
}

bool ProgramArgs::validateResize() const{
    if (args.size() != 6) {
        errorMessage = "Error: Invalid number of extra arguments for resize";
        return false;
    }
    try {
        int const width = std::stoi(args[4]);
        int const height = std::stoi(args[5]);
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

bool ProgramArgs::validateCutFreq() const{
    if (args.size() != 4) {
        errorMessage = "Error: Invalid number of extra arguments for cutfreq";
        return false;
    }
    try {
        int const freq = std::stoi(args[3]);
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

bool ProgramArgs::validateCompress() const{
    if (args.size() != 4) {  // Verifica que haya cuatro argumentos en total para `compress`
        errorMessage = "Error: Invalid extra arguments for compress";
        return false;
    }
    return true;
}

std::string ProgramArgs::getInputFile() const {
    return inputFile;
}

std::optional<FilePaths> ProgramArgs::getFilePaths() const {
    if (operation == "maxlevel" || operation == "resize" || operation == "cutfreq" || operation == "compress") {
        return FilePaths{inputFile, outputFile};
    }
    return std::nullopt;
}

std::string ProgramArgs::getOperation() const {
    return operation;
}

std::string ProgramArgs::getErrorMessage() const {
    return errorMessage;
}

int ProgramArgs::getMaxLevel() const {
    try {
        return std::stoi(args[4]);
    } catch (const std::exception& e) {
        std::cerr << "Error: Invalid max level value: " << e.what() << '\n';
        return -1;
    }
}

int ProgramArgs::getResizeWidth() const {
  try {
    int width = std::stoi(args[4]);
    if (width <= 0) {
      std::cerr << "Error: Width must be positive.\n";
      return -1;
    }
    std::cout << "Parsed width: " << width << std::endl;
    return width;
  } catch (const std::exception& e) {
    std::cerr << "Error: Invalid width value: " << e.what() << '\n';
    return -1;
  }
}

int ProgramArgs::getResizeHeight() const {
  try {
    int height = std::stoi(args[5]);
    if (height <= 0) {
      std::cerr << "Error: Height must be positive.\n";
      return -1;
    }
    std::cout << "Parsed height: " << height << std::endl;
    return height;
  } catch (const std::exception& e) {
    std::cerr << "Error: Invalid height value: " << e.what() << '\n';
    return -1;
  }
}


int ProgramArgs::getCutFreq() const {
    try {
        return std::stoi(args[3]);
    } catch (const std::exception& e) {
        std::cerr << "Error: Invalid cut frequency value: " << e.what() << '\n';
        return -1;
    }
}
