//
// Created by liang on 4/10/24.
//

#include "progargs.hpp"    // Incluir el manejo de argumentos
#include "binaryio.hpp"    // Incluir el manejo de archivos binarios
#include "imageinfo.hpp"   // Incluir la lógica de processInfo desde common
#include "imageaos.hpp"    // Incluir la lógica de imgaos

#include <iostream>        // Para la salida estándar

int main(int argc, char* argv[]) {
    // Crear una instancia de ProgramArgs para gestionar los argumentos de línea de comandos
    ProgramArgs args(argc, argv);

    // Validar los argumentos proporcionados
    if (!args.validate()) {
        std::cerr << "Error: " << args.getErrorMessage() << std::endl;
        return 1;  // Salir con error si la validación falla
    }

    // Obtener el comando (info, maxlevel, resize, cutfreq, compress)
    std::string command = args.getOperation();

    try {
        if (command == "info") {
            // Validar argumentos para 'info'
            if (!args.validateInfo()) {
                std::cerr << args.getErrorMessage() << std::endl;
                return -1;  // Error en los argumentos para 'info'
            }
            // Llamada a processInfo desde common
            processInfo(args.getInputFile());
            return 0;

        } else if (command == "maxlevel") {
            // Validar y procesar 'maxlevel'
            if (!args.validateMaxLevel()) {
                std::cerr << args.getErrorMessage() << std::endl;
                return -1;
            }
            processMaxLevel(BinaryIO::readBinaryFile(args.getInputFile()), args.getMaxLevel());
            return 0;
        } else if (command == "resize") {
            // Comando 'resize': redimensionar el archivo
            int width = args.getResizeWidth();
            int height = args.getResizeHeight();
            std::cout << "Resizing to " << width << "x" << height << std::endl;
            // Aquí iría la lógica para redimensionar el archivo

        } else if (command == "cutfreq") {
            // Comando 'cutfreq': cortar frecuencia
            int frequency = args.getCutFreq();
            std::cout << "Cutting frequency to: " << frequency << std::endl;
            // Aquí iría la lógica para ajustar la frecuencia del archivo

        } else if (command == "compress") {
            // Comando 'compress': comprimir el archivo
            if (!args.validateCompress()) {
              std::cerr << args.getErrorMessage() << std::endl;
              return -1;  // Error en los argumentos para 'info'
            }
            std::cout << "Compressing file..." << std::endl;
            compressAoS(args.getInputFile(), args.getOutputFile());
            std::cout << "File compressed to " << args.getOutputFile() << std::endl;
            return 0;

        } else {
            // Si el comando no es válido, mostrar mensaje de error
            std::cerr << "Error: Unknown command '" << command << "'" << std::endl;
            return 1;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
