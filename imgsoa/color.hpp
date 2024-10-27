//
// Created by liang on 27/10/24.
//

#ifndef COLOR_HPP
#define COLOR_HPP

#include <vector>
#include <string>
#include <iostream>
#include <functional>  // Necesario para std::hash
#include <cstdint>

class ColorPalette {
  public:
  void addColor(uint8_t red, uint8_t green, uint8_t blue);
  std::tuple<uint8_t, uint8_t, uint8_t> getColor(size_t index) const;
  size_t size() const;
  void printPalette() const;

  private:
  std::vector<uint8_t> reds;
  std::vector<uint8_t> greens;
  std::vector<uint8_t> blues;
};

#endif // COLOR_HPP

