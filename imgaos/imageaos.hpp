//
// Created by liang on 4/10/24.
//


#ifndef IMAGEAOS_HPP
#define IMAGEAOS_HPP

#include "color.hpp"
#include "imageinfo.hpp"
#include <string>
#include <vector>
#include <unordered_map>

#include <string>

void processMaxLevel(std::vector<uint8_t> inputFile, int maxLevel);

std::vector<Color> extractImagePixels(const std::vector<uint8_t>& fileData, const PPMHeader& header);
std::pair<std::vector<Color>, std::unordered_map<Color, int>> createColorTable(const std::vector<Color>& imagePixels);
std::string generateHeader(const PPMHeader& header, int colorTableSize);
void appendColorTable(std::vector<uint8_t>& compressedData, const std::vector<Color>& colorTable, const PPMHeader& header);
void appendPixelIndices(std::vector<uint8_t>& compressedData, const std::vector<Color>& imagePixels, const std::unordered_map<Color, int>& colorIndex);
void compressAoS(const std::string& inputFile, std::string outputFile);
// Otras funciones para 'resize', 'cutfreq', etc.

#endif // IMAGEAOS_HPP
