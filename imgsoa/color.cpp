//
// Created by liang on 27/10/24.
//
#include "color.hpp"

void ColorPalette::addColor(uint8_t red, uint8_t green, uint8_t blue) {
  reds.push_back(red);
  greens.push_back(green);
  blues.push_back(blue);
}

std::tuple<uint8_t, uint8_t, uint8_t> ColorPalette::getColor(size_t index) const {
  return {reds[index], greens[index], blues[index]};
}

size_t ColorPalette::size() const {
  return reds.size();
}

void ColorPalette::printPalette() const {
  for (size_t i = 0; i < size(); ++i) {
    std::cout << "Color " << i << " -> R: " << static_cast<int>(reds[i])
              << " G: " << static_cast<int>(greens[i])
              << " B: " << static_cast<int>(blues[i]) << '\n';
  }
}
