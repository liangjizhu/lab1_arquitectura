#include "command_handlers.hpp"
#include "binaryio.hpp"
#include "imageinfo.hpp"
#include "imageaos.hpp"
#include <iostream>
#include <chrono>
void processCutfreq(const std::string& inputFile, int frequency, const std::string& outputFile);
int handleInfo(const ProgramArgs& args) {
    if (!args.validateInfo()) {
        std::cerr << args.getErrorMessage() << '\n';
        return -1;
    }
    processInfo(args.getInputFile());
    return 0;
}

int handleMaxLevel(const ProgramArgs& args) {
    if (!args.validateMaxLevel()) {
        std::cerr << args.getErrorMessage() << '\n';
        return -1;
    }
    processMaxLevel(BinaryIO::readBinaryFile(args.getInputFile()), args.getMaxLevel());
    return 0;
}

int handleResize(const ProgramArgs& args) {
    if (!args.validateResize()) {
        std::cerr << args.getErrorMessage() << '\n';
        return -1;
    }
    int const width = args.getResizeWidth();
    int const height = args.getResizeHeight();
    std::cout << "Resizing to " << width << "x" << height << '\n';
    // Aquí iría la lógica para redimensionar el archivo
    return 0;
}

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

int handleCompress(const ProgramArgs& args) {
    if (!args.validateCompress()) {
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

    // Llamar a la función de compresión
    compressAoS(paths.value());

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> const duration = end - start;

    std::cout << "File compressed to " << paths->outputFile << '\n';
    std::cout << "Time taken: " << duration.count() << " seconds" << '\n';
    return 0;
}
