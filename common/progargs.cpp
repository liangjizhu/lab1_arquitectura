//
// Created by liang on 4/10/24.
//

#include "progargs.hpp"
#include <stdexcept>
#include <iostream>

ProgramArgs::ProgramArgs(int argc, char* argv[]) {
    for (int i = 0; i < argc; ++i) {
        args.emplace_back(argv[i]);
    }
}

bool ProgramArgs::validate() const {
    if (args.size() < 3) {
        errorMessage = "Error: Invalid number of arguments";
        return false;
    }

    std::string operation = args[2];

    if (operation == "info") {
        return validateInfo();
    } else if (operation == "maxlevel") {
        return validateMaxLevel();
    } else if (operation == "resize") {
        return validateResize();
    } else if (operation == "cutfreq") {
        return validateCutFreq();
    } else if (operation == "compress") {
        return validateCompress();
    } else {
        errorMessage = "Error: Invalid operation: " + operation;
        return false;
    }
}

bool ProgramArgs::validateInfo() {
    if (args.size() != 3) {
        errorMessage = "Error: Invalid extra arguments for info";
        return false;
    }
    return true;
}

bool ProgramArgs::validateMaxLevel() {
    if (args.size() != 4) {
        errorMessage = "Error: Invalid number of extra arguments for maxlevel";
        return false;
    }
    try {
        int maxLevel = std::stoi(args[3]);
        if (maxLevel < 0 || maxLevel > 65535) {
            errorMessage = "Error: Invalid maxlevel: " + args[3];
            return false;
        }
    } catch (const std::invalid_argument&) {
        errorMessage = "Error: Invalid maxlevel: " + args[3];
        return false;
    }
    return true;
}

bool ProgramArgs::validateResize() {
    if (args.size() != 5) {
        errorMessage = "Error: Invalid number of extra arguments for resize";
        return false;
    }
    try {
        int width = std::stoi(args[3]);
        int height = std::stoi(args[4]);
        if (width <= 0 || height <= 0) {
            errorMessage = "Error: Invalid resize dimensions";
            return false;
        }
    } catch (const std::invalid_argument&) {
        errorMessage = "Error: Invalid resize dimensions";
        return false;
    }
    return true;
}

bool ProgramArgs::validateCutFreq() {
    if (args.size() != 4) {
        errorMessage = "Error: Invalid number of extra arguments for cutfreq";
        return false;
    }
    try {
        int freq = std::stoi(args[3]);
        if (freq <= 0) {
            errorMessage = "Error: Invalid cutfreq: " + args[3];
            return false;
        }
    } catch (const std::invalid_argument&) {
        errorMessage = "Error: Invalid cutfreq: " + args[3];
        return false;
    }
    return true;
}

bool ProgramArgs::validateCompress() {
    if (args.size() != 3) {
        errorMessage = "Error: Invalid extra arguments for compress";
        return false;
    }
    return true;
}

std::string ProgramArgs::getInputFile() const {
    return args[1];
}

std::string ProgramArgs::getOutputFile() const {
    return args[2];
}

std::string ProgramArgs::getOperation() const {
    return args[2];
}

int ProgramArgs::getMaxLevel() const {
    return std::stoi(args[3]);
}

int ProgramArgs::getResizeWidth() const {
    return std::stoi(args[3]);
}

int ProgramArgs::getResizeHeight() const {
    return std::stoi(args[4]);
}

int ProgramArgs::getCutFreq() const {
    return std::stoi(args[3]);
}

std::string ProgramArgs::getErrorMessage() const {
    return errorMessage;
}
