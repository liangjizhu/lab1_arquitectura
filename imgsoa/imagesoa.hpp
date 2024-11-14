#ifndef IMAGESOA_HPP
#define IMAGESOA_HPP

#include "imageinfo.hpp"
#include "color.hpp"
#include "progargs.hpp"

#include <string>
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <tuple>



/********************************************* COMPRESS SOA *********************************************/
// Función principal para comprimir una imagen en formato SOA
void compressSoA(const FilePaths& paths);

// Crea un índice de colores único a partir de los canales de color
std::unordered_map<std::string, int> buildColorIndex(const ColorChannels& channels, ColorChannels& colorTable);

// Ordenar la tabla de colores en orden RGB
std::vector<size_t> sortColorTable(ColorChannels& colorTable);

// Crea una nueva tabla de colores (sortedColorTable) siguiendo el orden de los índices proporcionados
ColorChannels createSortedColorTable(const ColorChannels& colorTable, const std::vector<size_t>& indices);

// Reconstruir el índice de colores para la tabla ordenada
std::unordered_map<std::string, int> rebuildColorIndex(const ColorChannels& sortedColorTable);

// Crear tabla de colores y mapa de índices (SoA)
std::tuple<ColorChannels, std::unordered_map<std::string, int>>
createColorTableSoA(const ColorChannels& channels);

// Añadir la tabla de colores al archivo comprimido
void appendColorTableSoA(std::vector<uint8_t>& compressedData,
                         const ColorChannels& colorTable,
                         const PPMHeader& header);

// Añadir los índices de los píxeles al archivo comprimido
void appendPixelIndicesSoA(std::vector<uint8_t>& compressedData,
                           const ColorChannels& channels,
                           const std::unordered_map<std::string, int>& colorIndex);
/********************************************************************************************************/

struct ImageSOA {
  std::vector<uint8_t> redChannel;
  std::vector<uint8_t> greenChannel;
  std::vector<uint8_t> blueChannel;
  int width;
  int height;
};

ImageSOA resizeImageSOA(const ImageSOA& image, int newWidth, int newHeight);


#endif // IMAGESOA_HPP
