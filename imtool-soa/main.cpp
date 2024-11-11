#include "progargs.hpp"
#include "imageinfo.hpp"
#include "imgsoa/imagesoa.hpp"
#include <iostream>
#include <vector>
#include <string>
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
    // TODO
    return 0;
}

// Función para manejar el comando 'resize'
int handleResize(const ProgramArgs& args) {
    if (!args.validateResize()) {
        std::cerr << args.getErrorMessage() << '\n';
        return -1;
    }
    // TODO
    return 0;
}

// Función para manejar el comando 'cutfreq'
int handleCutFreq(const ProgramArgs& args) {
    if (!args.validateCutFreq()) {
        std::cerr << args.getErrorMessage() << '\n';
        return -1;
    }
    // TODO
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

// Función principal
int main(int argc, char* argv[]) {
    std::vector<std::string> const arguments(argv, argv + argc);
    ProgramArgs const args(arguments);

    if (!args.validate()) {
        std::cerr << "Error: " << args.getErrorMessage() << '\n';
        return 1;
    }

    std::string const command = args.getOperation();

    try {
        if (command == "info") {
            return handleInfo(args);
        }
        if (command == "maxlevel") {
            return handleMaxLevel(args);
        }
        if (command == "resize") {
            return handleResize(args);
        }
        if (command == "cutfreq") {
            return handleCutFreq(args);
        }
        if (command == "compress") {
            return handleCompress(args);
        }

        std::cerr << "Error: Unknown command '" << command << "'" << '\n';
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}
