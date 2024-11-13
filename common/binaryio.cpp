#include "binaryio.hpp"
#include <fstream>
#include <stdexcept>
#include <cstdint>
#include <bit>
#include <limits>

// Lee un archivo binario y devuelve su contenido como un vector de bytes
std::vector<uint8_t> BinaryIO::readBinaryFile(const std::string& filename) {
  std::ifstream file(filename, std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("Error: Unable to open file " + filename);
  }

  file.seekg(0, std::ios::end);
  std::streamsize const size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<uint8_t> buffer(static_cast<std::vector<uint8_t>::size_type>(size));

  if (!file.read(std::bit_cast<char*>(buffer.data()), size)) {
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

  if (!file.write(std::bit_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()))) {
    throw std::runtime_error("Error: Failed to write to file " + filename);
  }
}

std::vector<uint8_t> readPPMData(const std::string& filename, PPMHeader& header) {
  std::ifstream file(filename, std::ios::binary);
  if (!file.is_open()) {
    throw std::runtime_error("Error: Unable to open file " + filename);
  }

  // Read header
  std::string format;
  file >> format;
  if (format != "P6") {
    throw std::runtime_error("Error: Unsupported PPM format (only P6 is supported).");
  }

  file >> header.width >> header.height >> header.maxColorValue;
  if (header.maxColorValue != 255) {
    throw std::runtime_error("Error: Unsupported max color value (only 255 is supported).");
  }

  file.ignore(1);  // Skip newline

  // Ensure both width and height are cast to size_t for dataSize computation
  const size_t dataSize = static_cast<size_t>(header.width) * static_cast<size_t>(header.height) * 3;
  std::vector<uint8_t> data(dataSize);

  // Only cast dataSize if it's within the range of std::streamsize
  if (dataSize > static_cast<size_t>(std::numeric_limits<std::streamsize>::max())) {
    throw std::overflow_error("Error: Image data size exceeds allowable file read size.");
  }

  // Perform file read with safe conversion
  file.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(dataSize));
  if (!file) {
    throw std::runtime_error("Error: Failed to read pixel data.");
  }

  return data;
}

std::vector<uint8_t> readPPM(const std::string& filename, int& width, int& height, int& maxColorValue) {
  std::ifstream file(filename, std::ios::binary);
  if (!file) {
    throw std::runtime_error("Error: Unable to open file " + filename);
  }

  std::string format;
  file >> format;
  if (format != "P6") {
    throw std::runtime_error("Error: Unsupported PPM format. Expected P6.");
  }

  file >> width >> height >> maxColorValue;
  file.ignore(1); // Ignorar el carácter de nueva línea después de maxColorValue

  // Verificación del valor máximo de color
  if (maxColorValue != 255) {
    throw std::runtime_error("Error: Unsupported max color value. Expected 255.");
  }

  // Calcular el tamaño esperado de los datos
  size_t dataSize = static_cast<size_t>(width) * static_cast<size_t>(height) * 3;
  std::vector<uint8_t> data(dataSize);

  // Leer los datos de píxeles, convertiendo dataSize a streamsize
  file.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(dataSize));
  if (!file) {
    throw std::runtime_error("Error: Failed to read pixel data.");
  }

  return data;
}

void writePPM(const std::string& filename, const std::vector<uint8_t>& pixelData, int width, int height) {
  std::ofstream file(filename, std::ios::binary);
  if (!file.is_open()) {
    throw std::runtime_error("Error: Unable to open file " + filename + " for writing");
  }

  file << "P6\n" << width << " " << height << "\n255\n";
  file.write(reinterpret_cast<const char*>(pixelData.data()), static_cast<std::streamsize>(pixelData.size()));

  if (!file) {
    throw std::runtime_error("Error: Failed to write to file " + filename);
  }
}

