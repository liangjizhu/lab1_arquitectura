#include "progargs.hpp"
#include "binaryio.hpp"
#include "imageinfo.hpp"
#include "imgsoa/imagesoa.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
  std::vector<std::string> arguments(argv, argv + argc);
  ProgramArgs args(arguments);

  if (!args.validate()) {
    std::cerr << "Error: " << args.getErrorMessage() << '\n';
    return 1;
  }

  std::string const command = args.getOperation();

  try {
    if (command == "info") {
      if (!args.validateInfo()) {
        std::cerr << args.getErrorMessage() << '\n';
        return -1;
      }
      processInfo(args.getInputFile());
      return 0;
    } else if (command == "compress") {
      if (!args.validateCompress()) {
        std::cerr << args.getErrorMessage() << '\n';
        return -1;
      }
      compressSoA(args.getInputFile(), args.getOutputFile());
      return 0;
    } else {
      std::cerr << "Error: Unknown command '" << command << "'" << '\n';
      return 1;
    }
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << '\n';
    return 1;
  }

  return 0;
}
