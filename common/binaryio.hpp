#ifndef BINARYIO_HPP
#define BINARYIO_HPP

#include <string>
#include <vector>
#include <cstdint>
#include "imageinfo.hpp"

class BinaryIO {
    public:
    
    // Lee un archivo binario y devuelve su contenido en un vector de bytes
    static std::vector<uint8_t> readBinaryFile(const std::string& filename);
    
    // Pone un puntero al inicio del archivo
    static void readBinaryFileModified(const std::string& filename);
    //static std::istream readBinaryFileModified(const std::string& filename);

    // Escribe el contenido en un archivo binario desde un vector de bytes
    static void writeBinaryFile(const std::string& filename, const std::vector<uint8_t>& data);

};

// Lee un archivo PPM y devuelve los datos de píxeles junto con el ancho, alto y el valor máximo de color
std::vector<uint8_t> readPPM(const std::string& filename, int& width, int& height, int& maxColorValue);

void writePPM(const std::string& filename, const std::vector<uint8_t>& pixelData, int width, int height);

std::vector<uint8_t> readPPMData(const std::string& filename, PPMHeader& header);

#endif // BINARYIO_HPP