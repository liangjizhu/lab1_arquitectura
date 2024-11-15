//
// Created by paolo on 15/11/24.
//

#include "interpolation.hpp"
#include <algorithm>

std::pair<float, float> calculateWeights(float srcX, float srcY, size_t lowerX, size_t lowerY) {
  float xWeight = srcX - static_cast<float>(lowerX);
  float yWeight = srcY - static_cast<float>(lowerY);
  // Clamp only if weights fall outside the normal range (for stability)
  xWeight = std::clamp(xWeight, 0.0F, 1.0F);
  yWeight = std::clamp(yWeight, 0.0F, 1.0F);
  return {xWeight, yWeight};
}


std::pair<float, float> computeSourceCoordinates(int targetX, int targetY, const ScaleRatios& ratios) {
  float srcX = (static_cast<float>(targetX) + 0.5f) * ratios.xRatio - 0.5f;
  float srcY = (static_cast<float>(targetY) + 0.5f) * ratios.yRatio - 0.5f;
  return {srcX, srcY};
}


uint8_t interpolateChannel(uint8_t topLeft, uint8_t topRight, uint8_t bottomLeft, uint8_t bottomRight, float xWeight, float yWeight) {
  // Calcular c1 y c2 usando la fórmula especificada
  float c1 = topLeft + xWeight * (topRight - topLeft);
  float c2 = bottomLeft + xWeight * (bottomRight - bottomLeft);

  // Interpolación vertical
  return static_cast<uint8_t>(c1 + yWeight * (c2 - c1));
}

Pixel interpolatePixel(const Pixel& topLeft, const Pixel& topRight, const Pixel& bottomLeft, const Pixel& bottomRight, float xWeight, float yWeight) {
  return Pixel{
    .r = interpolateChannel(topLeft.r, topRight.r, bottomLeft.r, bottomRight.r, xWeight, yWeight),
    .g = interpolateChannel(topLeft.g, topRight.g, bottomLeft.g, bottomRight.g, xWeight, yWeight),
    .b = interpolateChannel(topLeft.b, topRight.b, bottomLeft.b, bottomRight.b, xWeight, yWeight)
};
}

uint8_t interpolateChannelDirect(uint8_t topLeft, uint8_t topRight, uint8_t bottomLeft, uint8_t bottomRight, float origX, float origY, size_t xl, size_t yl) {
  // Interpolación horizontal en la fila superior
  float c1 = topLeft + (origX - static_cast<float>(xl)) * (topRight - topLeft);
  // Interpolación horizontal en la fila inferior
  float c2 = bottomLeft + (origX - static_cast<float>(xl)) * (bottomRight - bottomLeft);
  // Interpolación vertical entre c1 y c2
  return static_cast<uint8_t>(c1 + (origY - static_cast<float>(yl)) * (c2 - c1));
}

Pixel interpolatePixelDirect(const Pixel& topLeft, const Pixel& topRight, const Pixel& bottomLeft, const Pixel& bottomRight, float origX, float origY, size_t xl, size_t yl) {
  return Pixel{
    .r = interpolateChannelDirect(topLeft.r, topRight.r, bottomLeft.r, bottomRight.r, origX, origY, xl, yl),
    .g = interpolateChannelDirect(topLeft.g, topRight.g, bottomLeft.g, bottomRight.g, origX, origY, xl, yl),
    .b = interpolateChannelDirect(topLeft.b, topRight.b, bottomLeft.b, bottomRight.b, origX, origY, xl, yl)
};
}
