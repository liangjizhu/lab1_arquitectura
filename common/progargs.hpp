//
// Created by liang on 4/10/24.
//

#ifndef PROGARGS_HPP
#define PROGARGS_HPP

#include <string>
#include <vector>

class ProgramArgs {
  public:
  // Constructor que recibe los argumentos de la línea de comandos
  ProgramArgs(int argc, char* argv[]);

  // Funciones para obtener cada uno de los argumentos
  std::string getInputFile() const;
  std::string getOutputFile() const;
  std::string getOperation() const;
  std::vector<std::string> getAdditionalParams() const;

  // Validar los argumentos
  bool validate() const;

  // Mostrar un mensaje de uso cuando los argumentos no son válidos
  static void printUsage();

  private:
  std::string inputFile;
  std::string outputFile;
  std::string operation;
  std::vector<std::string> additionalParams;

  bool valid;  // Bandera para verificar si los argumentos son válidos
};

#endif // PROGARGS_HPP

