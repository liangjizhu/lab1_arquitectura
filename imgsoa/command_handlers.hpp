#ifndef COMMAND_HANDLERS_HPP
#define COMMAND_HANDLERS_HPP

#include "progargs.hpp"

// Declaraciones de funciones para manejar cada comando
int handleInfo(const ProgramArgs& args);
int handleMaxLevel(const ProgramArgs& args);
int handleResize(const ProgramArgs& args);
int handleCutFreq(const ProgramArgs& args);
int handleCompress(const ProgramArgs& args);

namespace imgsoa {
  bool resizeAndSaveImage(const std::string& inputFile, const std::string& outputFile, int newWidth, int newHeight);
}

#endif // COMMAND_HANDLERS_HPP