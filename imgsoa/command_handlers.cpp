#include "command_handlers.hpp"
#include "imageinfo.hpp"
#include "imgsoa/imagesoa.hpp"
#include <iostream>
#include <chrono>

// Función para manejar el comando 'info'
int handleInfo(const ProgramArgs& args) {
  if (!args.validateInfo()) {
    std::cerr << args.getErrorMessage() << '\n';
    return -1;
  }
  processInfo(args.getInputFile());
  return 0;
}

// Función para manejar el comando 'maxlevel'
int handleMaxLevel(const ProgramArgs& args) {
  if (!args.validateMaxLevel()) {
    std::cerr << args.getErrorMessage() << '\n';
    return -1;
  }
  // TODO: Implementar maxlevel
  return 0;
}

// Función para manejar el comando 'resize'
int handleResize(const ProgramArgs& args) {
  if (!args.validateResize()) {
    std::cerr << args.getErrorMessage() << '\n';
    return -1;
  }
  // TODO: Implementar resize
  return 0;
}

// Función para manejar el comando 'cutfreq'
int handleCutFreq(const ProgramArgs& args) {
  if (!args.validateCutFreq()) {
    std::cerr << args.getErrorMessage() << '\n';
    return -1;
  }
  // TODO: Implementar cutfreq
  return 0;
}

// Función para manejar el comando 'compress'
int handleCompress(const ProgramArgs& args) {
  auto paths = args.getFilePaths();
  if (!args.validateCompress()) {
    std::cerr << args.getErrorMessage() << '\n';
    return -1;
  }

  std::cout << "Compressing file..." << '\n';
  auto start = std::chrono::high_resolution_clock::now();
  compressSoA(paths.value());
  auto end = std::chrono::high_resolution_clock::now();

  std::chrono::duration<double> const duration = end - start;
  std::cout << "File compressed to " << args.getOutputFile() << '\n';
  std::cout << "Time taken: " << duration.count() << " seconds" << '\n';
  return 0;
}
