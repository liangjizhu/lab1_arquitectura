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
std::pair<float, float> calculateWeights(float srcX, float srcY, size_t lowerX, size_t lowerY);

// Function to compute source coordinates using target coordinates and scaling ratios
std::pair<float, float> computeSourceCoordinates(int targetX, int targetY, const ScaleRatios& ratios);

// Helper function to interpolate a single color channel
uint8_t interpolateChannel(uint8_t topLeft, uint8_t topRight, uint8_t bottomLeft, uint8_t bottomRight, float xWeight, float yWeight);

// Main function to interpolate the pixel
Pixel interpolatePixel(const Pixel& topLeft, const Pixel& topRight, const Pixel& bottomLeft, const Pixel& bottomRight, float xWeight, float yWeight);

// interpolation.hpp
Pixel interpolatePixelDirect(const Pixel& topLeft, const Pixel& topRight,
                             const Pixel& bottomLeft, const Pixel& bottomRight,
                             float origX, float origY, size_t xl, size_t yl);


// interpolation.hpp
uint8_t interpolateChannelDirect(uint8_t p1, uint8_t p2, uint8_t p3, uint8_t p4,
                                 float origX, float origY, size_t xl, size_t yl);


#endif // INTERPOLATION_HPP