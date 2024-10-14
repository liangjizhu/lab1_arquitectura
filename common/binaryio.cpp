//
// Created by liang on 4/10/24.
//

#include "binaryio.hpp"
#include <fstream>
#include <stdexcept>

// Lee un archivo binario y devuelve su contenido como un vector de bytes
std::vector<uint8_t> BinaryIO::readBinaryFile(const std::string& filename) {
  std::ifstream file(filename, std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("Error: Unable to open file " + filename);
  }

  file.seekg(0, std::ios::end);
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  // Convertimos size a un tipo sin signo compatible con std::vector
  std::vector<uint8_t> buffer(static_cast<std::vector<uint8_t>::size_type>(size));

  if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
    throw std::runtime_error("Error: Failed to read the file " + filename);
  }

  return buffer;
}

// Escribe el contenido de un vector de bytes en un archivo binario
void BinaryIO::writeBinaryFile(const std::string& filename, const std::vector<uint8_t>& data) {
  std::ofstream file(filename, std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("Error: Unable to open file " + filename + " for writing");
  }

  // Convertimos data.size() a std::streamsize de manera segura
  if (!file.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()))) {
    throw std::runtime_error("Error: Failed to write to file " + filename);
  }
}
