//
// Created by liang on 4/10/24.
//

#include "progargs.hpp"    // Incluir el manejo de argumentos
#include "binaryio.hpp"    // Incluir el manejo de archivos binarios
#include "imageinfo.hpp"   // Incluir la lógica de processInfo desde common
#include "imgsoa/imagesoa.hpp"
#include "imgsoa/imagesoa.cpp"
#include <iostream>        // Para la salida estándar
#include "imgsoa/color.hpp"

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
            // processMaxLevel(args.getInputFile(), args.getMaxLevel());
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
            std::string inputFile = args.getInputFile();       // Obtener el archivo de entrada
            std::string outputFile = args.getOutputFile();     // Obtener el archivo de salida (asegúrate de que esto exista en ProgramArgs)
    
            std::cout << "Cutting frequency to: " << frequency << std::endl;

            // Llama a la función processCutfreq con los argumentos correctos
            processCutfreq(inputFile, frequency, outputFile);
            return 0;
            // Aquí iría la lógica para ajustar la frecuencia del archivo

        } else if (command == "compress") {
            // Comando 'compress': comprimir el archivo
            std::cout << "Compressing file..." << std::endl;
            // Aquí iría la lógica para comprimir el archivo

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
