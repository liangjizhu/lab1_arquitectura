//
// Created by liang on 4/10/24.
//
#include "progargs.hpp"
#include <iostream>

// Constructor que procesa los argumentos
ProgramArgs::ProgramArgs(int argc, char* argv[]) {
  if (argc < 3) {
    valid = false;
    return;
  }

  inputFile = argv[1];
  outputFile = argv[2];
  operation = argv[3];

  for (int i = 4; i < argc; ++i) {
    additionalParams.push_back(argv[i]);
  }

  valid = validate();
}

// Obtener el archivo de entrada
std::string ProgramArgs::getInputFile() const {
  return inputFile;
}

// Obtener el archivo de salida
std::string ProgramArgs::getOutputFile() const {
  return outputFile;
}

// Obtener la operación
std::string ProgramArgs::getOperation() const {
  return operation;
}

// Obtener los parámetros adicionales
std::vector<std::string> ProgramArgs::getAdditionalParams() const {
  return additionalParams;
}

// Validar los argumentos
bool ProgramArgs::validate() const {
  // Comprobamos que la operación sea válida
  if (operation != "info" && operation != "maxlevel" &&
      operation != "resize" && operation != "cutfreq" &&
      operation != "compress") {
    return false;
      }

  // Validaciones específicas por operación
  if (operation == "maxlevel" && additionalParams.size() != 1) {
    return false;
  } else if (operation == "resize" && additionalParams.size() != 2) {
    return false;
  } else if (operation == "cutfreq" && additionalParams.size() != 1) {
    return false;
  }

  return true;
}

// Mostrar mensaje de uso
void ProgramArgs::printUsage() {
  std::cout << "Usage: imtool <input_file> <output_file> <operation> [additional_params]\n";
  std::cout << "Operations:\n";
  std::cout << "  info: Display image metadata\n";
  std::cout << "  maxlevel <new_max>: Scale intensity levels\n";
  std::cout << "  resize <new_width> <new_height>: Resize image\n";
  std::cout << "  cutfreq <n>: Remove n least frequent colors\n";
  std::cout << "  compress: Compress the image\n";
}
