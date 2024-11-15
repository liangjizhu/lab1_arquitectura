//
// Created by paolo on 15/11/24.
//

#ifndef INTERPOLATION_HPP
#define INTERPOLATION_HPP

#include <cstdint>
#include <cmath>
#include <utility>
#include "imgaos/imageaos.hpp"

// Holds the scaling ratios for x and y axes
struct ScaleRatios {
  float xRatio;
  float yRatio;
};

// Function to calculate weights based on source coordinates
std::pair<float, float> calculateWeights(float sourceX, float sourceY, size_t lowerXCoord, size_t lowerYCoord);

// Function to compute source coordinates using target coordinates and scaling ratios
std::pair<float, float> computeSourceCoordinates(int targetXCoord, int targetYCoord, const ScaleRatios& ratios);

// Helper function to interpolate a single color channel
uint8_t interpolateChannel(uint8_t topLeft, uint8_t topRight, uint8_t bottomLeft, uint8_t bottomRight, float xWeight, float yWeight);

// Main function to interpolate the pixel
Pixel interpolatePixel(const Pixel& topLeft, const Pixel& topRight, const Pixel& bottomLeft, const Pixel& bottomRight, float xWeight, float yWeight);

// Direct interpolation function for a pixel
Pixel interpolatePixelDirect(const Pixel& topLeft, const Pixel& topRight,
                             const Pixel& bottomLeft, const Pixel& bottomRight,
                             float originalX, float originalY, size_t lowerXCoord, size_t lowerYCoord);

// Direct interpolation function for a single color channel
uint8_t interpolateChannelDirect(uint8_t topLeft, uint8_t topRight, uint8_t bottomLeft, uint8_t bottomRight,
                                 float originalX, float originalY, size_t lowerXCoord, size_t lowerYCoord);

#endif // INTERPOLATION_HPP
