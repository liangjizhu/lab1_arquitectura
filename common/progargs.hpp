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

    private:
    std::vector<std::string> args;
    std::string operation;
    std::string inputFile;
    std::string errorMessage;

    bool validateInfo();
    bool validateMaxLevel();
    bool validateResize();
    bool validateCutFreq();
    bool validateCompress();
};

#endif // PROGARGS_HPP
