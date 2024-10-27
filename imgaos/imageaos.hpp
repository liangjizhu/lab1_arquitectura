//
// Created by liang on 4/10/24.
//


#ifndef IMAGEAOS_HPP
#define IMAGEAOS_HPP

#include "binaryio.hpp"
#include "color.hpp"

#include <string>

void processMaxLevel(std::vector<uint8_t> inputFile, int maxLevel);
void compressAoS(const std::string& inputFile, const std::string& outputFile);
// Otras funciones para 'resize', 'cutfreq', etc.

#endif // IMAGEAOS_HPP
