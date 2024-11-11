#ifndef IMAGEAOS_HPP
#define IMAGEAOS_HPP

#include "color.hpp"
#include "imageinfo.hpp"
#include "progargs.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>


void processMaxLevel(std::vector<uint8_t> inputFile, int maxLevel);

// COMPRESS
std::vector<Color> extractImagePixels(const std::vector<uint8_t>& fileData, const PPMHeader& header);
std::pair<std::vector<Color>, std::unordered_map<Color, int>> createColorTable(const std::vector<Color>& imagePixels);
void appendColorTable(std::vector<uint8_t>& compressedData, const std::vector<Color>& colorTable, const PPMHeader& header);
void appendPixelIndices(std::vector<uint8_t>& compressedData, const std::vector<Color>& imagePixels, const std::unordered_map<Color, int>& colorIndex);
void compressAoS(const FilePaths& paths);
// Otras funciones para 'resize', 'cutfreq', etc.
// Define Pixel and Image types here
struct Pixel {
  uint8_t r, g, b;
};
using Image = std::vector<std::vector<Pixel>>;

// Function declarations
Image vectorToImage(const std::vector<uint8_t>& data, int width, int height, int channels);
std::vector<uint8_t> imageToVector(const Image& image, int channels);
Image resizeImageAoS(const Image& image, int newWidth, int newHeight);

class ImageAOS {
    public:
    ImageAOS(int width, int height, const std::vector<uint8_t>& pixels);

    void resize(int newWidth, int newHeight);

    private:
    int width;
    int height;
    std::vector<uint8_t> pixels; // Almacena los píxeles en formato RGB
};


#endif // IMAGEAOS_HPP