//
// Created by paolo on 15/11/24.
//

#include "interpolation.hpp"
#include <algorithm>

std::pair<float, float> calculateWeights(float sourceX, float sourceY, size_t lowerXCoord, size_t lowerYCoord) {
    float xWeight = sourceX - static_cast<float>(lowerXCoord);
    float yWeight = sourceY - static_cast<float>(lowerYCoord);
    // Clamp only if weights fall outside the normal range (for stability)
    xWeight = std::clamp(xWeight, 0.0F, 1.0F);
    yWeight = std::clamp(yWeight, 0.0F, 1.0F);
    return {xWeight, yWeight};
}

std::pair<float, float> computeSourceCoordinates(int targetXCoord, int targetYCoord, const ScaleRatios& ratios) {
    float const sourceX = (static_cast<float>(targetXCoord) + 0.5F) * ratios.xRatio - 0.5F;
    float const sourceY = (static_cast<float>(targetYCoord) + 0.5F) * ratios.yRatio - 0.5F;
    return {sourceX, sourceY};
}

uint8_t interpolateChannel(uint8_t topLeft, uint8_t topRight, uint8_t bottomLeft, uint8_t bottomRight, float xWeight, float yWeight) {
    // Calcular interpolaciones horizontales
    float const topInterpolation = topLeft + xWeight * (topRight - topLeft);
    float const bottomInterpolation = bottomLeft + xWeight * (bottomRight - bottomLeft);

    // Interpolación vertical
    return static_cast<uint8_t>(topInterpolation + yWeight * (bottomInterpolation - topInterpolation));
}

Pixel interpolatePixel(const Pixel& topLeft, const Pixel& topRight, const Pixel& bottomLeft, const Pixel& bottomRight, float xWeight, float yWeight) {
    return Pixel{
        .r = interpolateChannel(topLeft.r, topRight.r, bottomLeft.r, bottomRight.r, xWeight, yWeight),
        .g = interpolateChannel(topLeft.g, topRight.g, bottomLeft.g, bottomRight.g, xWeight, yWeight),
        .b = interpolateChannel(topLeft.b, topRight.b, bottomLeft.b, bottomRight.b, xWeight, yWeight)
    };
}

uint8_t interpolateChannelDirect(uint8_t topLeft, uint8_t topRight, uint8_t bottomLeft, uint8_t bottomRight, float originalX, float originalY, size_t lowerXCoord, size_t lowerYCoord) {
    // Interpolación horizontal en la fila superior
    float const topInterpolation = topLeft + (originalX - static_cast<float>(lowerXCoord)) * (topRight - topLeft);
    // Interpolación horizontal en la fila inferior
    float const bottomInterpolation = bottomLeft + (originalX - static_cast<float>(lowerXCoord)) * (bottomRight - bottomLeft);
    // Interpolación vertical entre las dos interpolaciones horizontales
    return static_cast<uint8_t>(topInterpolation + (originalY - static_cast<float>(lowerYCoord)) * (bottomInterpolation - topInterpolation));
}

Pixel interpolatePixelDirect(const Pixel& topLeft, const Pixel& topRight, const Pixel& bottomLeft, const Pixel& bottomRight, float originalX, float originalY, size_t lowerXCoord, size_t lowerYCoord) {
    return Pixel{
        .r = interpolateChannelDirect(topLeft.r, topRight.r, bottomLeft.r, bottomRight.r, originalX, originalY, lowerXCoord, lowerYCoord),
        .g = interpolateChannelDirect(topLeft.g, topRight.g, bottomLeft.g, bottomRight.g, originalX, originalY, lowerXCoord, lowerYCoord),
        .b = interpolateChannelDirect(topLeft.b, topRight.b, bottomLeft.b, bottomRight.b, originalX, originalY, lowerXCoord, lowerYCoord)
    };
}
