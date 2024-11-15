#include "binaryio.hpp"
#include <fstream>
#include <stdexcept>
#include <cstdint>
#include <bit>
#include <limits>

static constexpr size_t COLOR_TABLE_RESERVE_SIZE = 256;

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

//QUIERO TERMINAR ESTO
void BinaryIO::readBinaryFileModified(const std::string& filename) {
//std::istream BinaryIO::readBinaryFileModified(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    
    if (!file.is_open()) {
        throw std::runtime_error("Error: Unable to open file " + filename);
    }

    //return inputStream;
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

  // Leer el formato
  std::string format;
  file >> format;
  if (format != "P6") {
    throw std::runtime_error("Error: Unsupported PPM format (only P6 is supported).");
  }

  // Leer el ancho, alto y valor máximo de color
  file >> header.width >> header.height >> header.maxColorValue;

  // Ignorar el carácter de nueva línea después del encabezado
  file.ignore(1);

  // Verificar que el valor máximo de color sea válido
  if (header.maxColorValue != COLOR_TABLE_RESERVE_SIZE -1) {
    throw std::runtime_error("Error: Unsupported max color value (only 255 is supported).");
  }

  // Calcular el tamaño esperado de los datos
  const size_t expected_size = static_cast<size_t>(header.width) * static_cast<size_t>(header.height) * 3;  // P6 es RGB, 3 canales
  std::vector<char> temp_data(expected_size);

  // Leer los datos binarios de la imagen, convertiendo expected_size a streamsize
  file.read(temp_data.data(), static_cast<std::streamsize>(expected_size));

  // Verificar que se haya leído la cantidad exacta de datos
  if (file.gcount() != static_cast<std::streamsize>(expected_size)) {
    throw std::runtime_error("Error: Data size in file does not match width, height, and channels.");
  }

  std::vector<uint8_t> data(temp_data.begin(), temp_data.end());
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
  if (maxColorValue != COLOR_TABLE_RESERVE_SIZE - 1) {
    throw std::runtime_error("Error: Unsupported max color value. Expected 255.");
  }

  // Calcular el tamaño esperado de los datos
  const size_t dataSize = static_cast<size_t>(width) * static_cast<size_t>(height) * 3;
  std::vector<char> tempData(dataSize);

  // Leer los datos de píxeles directamente en tempData
  file.read(tempData.data(), static_cast<std::streamsize>(dataSize));
  if (file.gcount() != static_cast<std::streamsize>(dataSize)) {
    throw std::runtime_error("Error: Failed to read pixel data.");
  }

  // Convertir tempData a std::vector<uint8_t>
  std::vector<uint8_t> data(tempData.begin(), tempData.end());
  return data;
}

void writePPM(const std::string& filename, const std::vector<uint8_t>& pixelData, int width, int height) {
  std::ofstream file(filename, std::ios::binary);
  if (!file.is_open()) {
    throw std::runtime_error("Error: Unable to open file " + filename + " for writing");
  }

  // Write the PPM header
  file << "P6\n" << width << " " << height << "\n255\n";

  // Convert pixelData to a std::vector<char> for writing
  const std::vector<char> pixelDataChar(pixelData.begin(), pixelData.end());

  // Write the pixel data as binary
  std::vector<char> charData(pixelData.begin(), pixelData.end());
  file.write(charData.data(), static_cast<std::streamsize>(charData.size()));


  if (!file) {
    throw std::runtime_error("Error: Failed to write to file " + filename);
  }
}

