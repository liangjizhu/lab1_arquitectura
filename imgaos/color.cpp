//
// Created by liang on 27/10/24.
//
#include "color.hpp"
#include <fstream>
#include <vector>
#include <unordered_map>
#include <iostream>

void ColorPalette::addColor(const Color& color) {
  colors.push_back(color);
}

Color ColorPalette::getColor(size_t index) const {
  if (index < colors.size()) {
    return colors[index];
  }
  throw std::out_of_range("Index out of range");
}

size_t ColorPalette::size() const {
  return colors.size();
}

void ColorPalette::printPalette() const {
  for (const auto& color : colors) {
    color.print();
  }
}

