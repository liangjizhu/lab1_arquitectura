#ifndef PROGARGS_HPP
#define PROGARGS_HPP

#include <string>
#include <vector>
#include <optional>

struct FilePaths {
    std::string inputFile;
    std::string outputFile;
};

class ProgramArgs {
    public:
    ProgramArgs(const std::vector<std::string>& arguments);

    bool validate() const;
    [[nodiscard]] std::string getInputFile() const;
    [[nodiscard]] std::string getOutputFile() const;
    [[nodiscard]] std::string getOperation() const;
    [[nodiscard]] int getMaxLevel() const;
    [[nodiscard]] int getResizeWidth() const;
    [[nodiscard]] int getResizeHeight() const;
    [[nodiscard]] int getCutFreq() const;
    [[nodiscard]] std::optional<FilePaths> getFilePaths() const;

    [[nodiscard]] std::string getErrorMessage() const;

    // Funciones de validación marcadas como const
    [[nodiscard]] bool validateInfo() const;
    [[nodiscard]] bool validateMaxLevel() const;
    [[nodiscard]] bool validateResize() const;
    [[nodiscard]] bool validateCutFreq() const;

    private:
    std::vector<std::string> args;
    mutable std::string errorMessage;
    mutable std::string operation;
    mutable std::string inputFile;
    mutable std::string outputFile;
};

#endif // PROGARGS_HPP
