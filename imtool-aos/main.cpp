//
// Created by liang on 4/10/24.
//

#include <iostream>
#include <string>
#include <cstdlib>

void printErrorAndExit(const std::string& message, int errorCode) {
    std::cerr << "Error: " << message << std::endl;
    std::exit(errorCode);
}

bool isInteger(const std::string& str) {
    for (char c : str) {
        if (!std::isdigit(c) && c != '-') return false;
    }
    return true;
}

int main(int argc, char* argv[]) {
    // Validación de número de argumentos mínimo
    if (argc < 4) {
        printErrorAndExit("Invalid number of arguments: " + std::to_string(argc - 1), -1);
    }

    std::string option = argv[3];

    if (option == "info" || option == "compress") {
        // 'info' y 'compress' requieren exactamente tres argumentos
        if (argc != 4) printErrorAndExit("Invalid extra arguments for " + option, -1);
    } else if (option == "maxlevel") {
        // 'maxlevel' requiere exactamente cuatro argumentos
        if (argc != 5) {
            printErrorAndExit("Invalid number of extra arguments for maxlevel: " + std::to_string(argc - 3), -1);
        }
        if (!isInteger(argv[4])) {
            printErrorAndExit("Invalid maxlevel: " + std::string(argv[4]), -1);
        }
        int maxlevel = std::stoi(argv[4]);
        if (maxlevel < 0 || maxlevel > 65535) {
            printErrorAndExit("Invalid maxlevel: " + std::to_string(maxlevel), -1);
        }
    } else if (option == "resize") {
        // 'resize' requiere exactamente cinco argumentos
        if (argc != 6) {
            printErrorAndExit("Invalid number of extra arguments for resize: " + std::to_string(argc - 3), -1);
        }
        if (!isInteger(argv[4]) || std::stoi(argv[4]) <= 0) {
            printErrorAndExit("Invalid resize width: " + std::string(argv[4]), -1);
        }
        if (!isInteger(argv[5]) || std::stoi(argv[5]) <= 0) {
            printErrorAndExit("Invalid resize height: " + std::string(argv[5]), -1);
        }
    } else if (option == "cutfreq") {
        // 'cutfreq' requiere exactamente cuatro argumentos
        if (argc != 5) {
            printErrorAndExit("Invalid number of extra arguments for cutfreq: " + std::to_string(argc - 3), -1);
        }
        if (!isInteger(argv[4]) || std::stoi(argv[4]) <= 0) {
            printErrorAndExit("Invalid cutfreq: " + std::string(argv[4]), -1);
        }
    } else {
        printErrorAndExit("Invalid option: " + option, -1);
    }

    std::cout << "Arguments processed successfully." << std::endl;
    return 0;
}
