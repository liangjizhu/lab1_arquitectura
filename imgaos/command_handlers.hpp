#ifndef COMMAND_HANDLERS_HPP
#define COMMAND_HANDLERS_HPP

#include "progargs.hpp"

// Declaraciones de funciones auxiliares
int handleInfo(const ProgramArgs& args);
int handleMaxLevel(const ProgramArgs& args);
int handleResize(const ProgramArgs& args);
int handleCutFreq(const ProgramArgs& args);
int handleCompress(const ProgramArgs& args);

#endif // COMMAND_HANDLERS_HPP
