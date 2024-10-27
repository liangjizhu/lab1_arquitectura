//
// Created by liang on 4/10/24.
//

#ifndef PROGARGS_HPP
#define PROGARGS_HPP

#include <string>
#include <vector>

class ProgramArgs {
    public:
    ProgramArgs(int argc, char* argv[]);

    bool validate();
    std::string getInputFile() const;
    std::string getOperation() const;
    int getMaxLevel() const;
    int getResizeWidth() const;
    int getResizeHeight() const;
    int getCutFreq() const;

    std::string getErrorMessage() const;

    // Funciones de validación
    bool validateInfo();
    bool validateMaxLevel();
    bool validateResize();
    bool validateCutFreq();
    bool validateCompress();

    private:
    std::vector<std::string> args;
    std::string errorMessage;

    // Agregar las variables faltantes
    std::string operation;
    std::string inputFile;
};

#endif // PROGARGS_HPP
