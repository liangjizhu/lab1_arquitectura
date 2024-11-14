#include "command_handlers.hpp"
#include "binaryio.hpp"
#include "imageinfo.hpp"
#include "imgsoa/imagesoa.hpp"
#include <iostream>
#include <chrono>
void processCutfreq(const std::string& inputFile, int numColors, const std::string& outputFile);
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

  auto start = std::chrono::high_resolution_clock::now();
  const int width = args.getResizeWidth();
  const int height = args.getResizeHeight();
  std::cout << "Resizing to " << width << "x" << height << '\n';

  // Delegate the resizing to a separate function in imagesoa.cpp
  if (!imgsoa::resizeAndSaveImage(args.getInputFile(), args.getOutputFile(), width, height)) {
    std::cerr << "Error: Resizing failed.\n";
    return -1;
  }

  std::cout << "Image resized and saved to " << args.getOutputFile() << '\n';
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> const duration = end - start;
  std::cout << "Time taken: " << duration.count() << " seconds" << std::endl;
  return 0;
}

// Función para manejar el comando 'cutfreq'
int handleCutFreq(const ProgramArgs& args) {
    if (!args.validateCutFreq()) {
        std::cerr << args.getErrorMessage() << '\n';
        return -1;
    }
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

    // Medir el tiempo de compresión
    auto start = std::chrono::high_resolution_clock::now();
    compressSoA(paths.value());
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> const duration = end - start;
    std::cout << "File compressed to " << args.getOutputFile() << std::endl;
    std::cout << "Time taken: " << duration.count() << " seconds" << std::endl;
    return 0;
}
