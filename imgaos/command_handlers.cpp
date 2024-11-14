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
    processMaxLevel(BinaryIO::readBinaryFile(args.getInputFile()), args.getMaxLevel());
    return 0;
}

// Función para manejar el comando 'resize'
int handleResize(const ProgramArgs& args) {
    if (!args.validateResize()) {
        std::cerr << args.getErrorMessage() << '\n';
        return -1;
    }

    int const width = args.getResizeWidth();
    int const height = args.getResizeHeight();
    std::cout << "Resizing to " << width << "x" << height << '\n';

    int originalWidth, originalHeight, maxColorValue;
    std::vector<uint8_t> rawData = readPPM(args.getInputFile(), originalWidth, originalHeight, maxColorValue);

    if (originalWidth == 0 || originalHeight == 0) {
        std::cerr << "Error: Could not retrieve image dimensions.\n";
        return -1;
    }

    int channels = 3;
    Image originalImage = vectorToImage(rawData, originalWidth, originalHeight, channels);
    Image resizedImage = resizeImageAoS(originalImage, width, height);
    std::vector<uint8_t> resizedData = imageToVector(resizedImage, channels);
    writePPM(args.getOutputFile(), resizedData, width, height);

    std::cout << "Image resized and saved to " << args.getOutputFile() << '\n';
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
