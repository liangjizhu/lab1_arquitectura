#include "progargs.hpp"
#include "command_handlers.hpp"
#include <iostream>
#include <vector>
#include <string>

// El MAIN solo invoca funciones
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
