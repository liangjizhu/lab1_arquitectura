//
// Created by paolo on 15/11/24.
//

#include "interpolation.hpp"
#include <algorithm>






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
