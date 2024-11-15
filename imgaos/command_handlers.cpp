#include "command_handlers.hpp"
#include "binaryio.hpp"
#include "imageinfo.hpp"
#include "imageaos.hpp"
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
    auto paths = args.getFilePaths();
    if (!paths.has_value()) {
        std::cerr << "Error: No se pudo obtener los archivos de entrada y salida." << '\n';
        return -1;
    }

    std::cout << "Cambiando intensidad de imagen..." << '\n';
    auto start = std::chrono::high_resolution_clock::now();
    processMaxLevel(paths.value(), static_cast<uint16_t>(args.getMaxLevel()));
    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "Imagen cambiada de intensidad y guardada " << args.getOutputFile() << '\n';
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> const duration = end - start;
    std::cout << "Time taken: " << duration.count() << " seconds\n";

    return 0;
}

int handleResize(const ProgramArgs& args) {
  if (!args.validateResize()) {
    std::cerr << args.getErrorMessage() << '\n';
    return -1;
  }

  auto const start = std::chrono::high_resolution_clock::now();
  int const width = args.getResizeWidth();
  int const height = args.getResizeHeight();
  std::cout << "Resizing to " << width << "x" << height << '\n';

  // Initialize variables individually
  int originalWidth = 0;
  int originalHeight = 0;
  int maxColorValue = 0;

  const std::vector<uint8_t> rawData = readPPM(args.getInputFile(), originalWidth, originalHeight, maxColorValue);

  if (originalWidth == 0 || originalHeight == 0) {
    std::cerr << "Error: Could not retrieve image dimensions.\n";
    return -1;
  }

  constexpr int channels = 3;
  const Image originalImage = vectorToImage(rawData, originalWidth, originalHeight, channels);
  const Image resizedImage = resizeImageAoS(originalImage, width, height);
  const std::vector<uint8_t> resizedData = imageToVector(resizedImage, channels);
  writePPM(args.getOutputFile(), resizedData, width, height);

  std::cout << "Image resized and saved to " << args.getOutputFile() << '\n';
  auto const end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> const duration = end - start;
  std::cout << "Time taken: " << duration.count() << " seconds\n";

  return 0;
}


// Función para manejar el comando 'cutfreq'
int handleCutFreq(const ProgramArgs& args) {
    if (!args.validateCutFreq()) {
        std::cerr << args.getErrorMessage() << '\n';
        return -1;
    }
    int const frequency = args.getCutFreq();
    std::cout << "Cutting frequency to: " << frequency << '\n';
    processCutfreq(args.getInputFile(), frequency, args.getOutputFile());

    // Aquí iría la lógica para ajustar la frecuencia del archivo
    return 0;
}

// Función para manejar el comando 'compress'
int handleCompress(const ProgramArgs& args) {
    if (!args.validate()) {
        std::cerr << args.getErrorMessage() << '\n';
        return -1;
    }

    auto paths = args.getFilePaths();
    if (!paths.has_value()) {
        std::cerr << "Error: No se pudo obtener los archivos de entrada y salida." << '\n';
        return -1;
    }

    std::cout << "Compressing file..." << '\n';
    auto start = std::chrono::high_resolution_clock::now();
    compressAoS(paths.value());
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> const duration = end - start;

    std::cout << "File compressed to " << paths->outputFile << '\n';
    std::cout << "Time taken: " << duration.count() << " seconds" << '\n';
    return 0;
}
