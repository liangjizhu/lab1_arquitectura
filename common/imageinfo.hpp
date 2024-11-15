#ifndef IMAGEINFO_HPP
#define IMAGEINFO_HPP

#include <string>
#include <cstdint>

struct PPMHeader {
    int width;
    int height;
    uint32_t maxColorValue;
};

bool readPPMHeader(const std::string& inputFile, PPMHeader& header);
void processInfo(const std::string& inputFile);
std::pair<int, int> getImageDimensions(const std::string& filename);
// Declaración de la función para generar encabezado
std::string generateHeaderMaxlevel(const PPMHeader& header, int maxlevel);
std::string generateHeader(const PPMHeader& header, int colorTableSize);
std::string ensureCppmExtension(const std::string& filename);

#endif // IMAGEINFO_HPP

