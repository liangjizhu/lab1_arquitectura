#include "progargs.hpp"
#include "binaryio.hpp"
#include "imageinfo.hpp"
#include "imageaos.hpp"

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
    processMaxLevel(BinaryIO::readBinaryFile(args.getInputFile()), args.getMaxLevel());
    return 0;
}

// TODO: clang-tidy
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

    // Leer la imagen PPM utilizando readPPM
    std::vector<uint8_t> rawData = readPPM(args.getInputFile(), originalWidth, originalHeight, maxColorValue);

    // Verificar que se haya leído correctamente la información de la imagen
    if (originalWidth == 0 || originalHeight == 0) {
        std::cerr << "Error: Could not retrieve image dimensions.\n";
        return -1;
    }

    int channels = 3;  // Asumimos formato RGB para PPM (3 canales)

    // Convertir el vector de datos en una estructura de imagen
    Image originalImage = vectorToImage(rawData, originalWidth, originalHeight, channels);

    // Redimensionar la imagen
    Image resizedImage = resizeImageAoS(originalImage, width, height);

    // Convertir la imagen redimensionada a un vector de bytes
    std::vector<uint8_t> resizedData = imageToVector(resizedImage, channels);

    // Escribir la imagen redimensionada a un archivo de salida PPM
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
    // Aquí iría la lógica para ajustar la frecuencia del archivo
    return 0;
}

// Función para manejar el comando 'compress'
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

        // Manejo de comando desconocido
        std::cerr << "Error: Unknown command '" << command << "'" << '\n';
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}
