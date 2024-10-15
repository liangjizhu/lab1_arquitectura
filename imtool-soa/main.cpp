//
// Created by liang on 4/10/24.
//

#include "progargs.hpp"  // Incluir el manejo de argumentos
#include "binaryio.hpp"  // Incluir el manejo de archivos binarios
#include <iostream>      // Para la salida estándar

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
            // Comando 'info': leer archivo y mostrar información
            std::vector<uint8_t> data = BinaryIO::readBinaryFile(args.getInputFile());
            std::cout << "File info: Size = " << data.size() << " bytes" << std::endl;

        } else if (command == "maxlevel") {
            // Comando 'maxlevel': establecer el nivel máximo
            int maxLevel = args.getMaxLevel();
            std::cout << "Setting max level to: " << maxLevel << std::endl;
            // Aquí iría la lógica para aplicar el maxLevel al archivo

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

    return 0;  // Todo se ejecutó correctamente
}
