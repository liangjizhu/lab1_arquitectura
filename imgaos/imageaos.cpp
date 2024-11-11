#include "imageaos.hpp"
#include "binaryio.hpp"
#include "progargs.hpp"
#include "imageinfo.hpp"
#include "color.hpp"

#include <iostream>
#include <fstream>
#include <cmath>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
#include <algorithm>

// Constante descriptiva para el tamaño de reserva inicial de la tabla de colores
constexpr size_t COLOR_TABLE_RESERVE_SIZE = 256;
constexpr uint16_t BYTE_MASK = 0xFF;
constexpr uint16_t MAX_COLOR_VALUE_8BIT = 255;
constexpr uint16_t MAX_COLOR_VALUE_16BIT = 65535;
constexpr uint8_t BITS_PER_BYTE = 8;

// TODO:
//  - PARA ESTA OPERACIÓN CREO QUE SE PODRÍA OPTIMIZAR SI SE EJECUTA AL MISMO TIEMPO QUE SE LEA EL ARCHIVO
void processMaxLevel(std::vector<uint8_t> inputFile, int maxLevel) {
    int contadorDeLineas = 0;
    std::string maxIntensidadStr;
    int maxIntensidadInt = 0;
    //bool altaIntensidad;

    //int contador = 0;
    //int contador_aux = 0;
    //uint8_t pet[2];
    //uint16_t caster;

    //Leer archivo
    for (unsigned char & i : inputFile){
        if (contadorDeLineas < 3){
            std::cout << i;
        }

        //Tener en cuenta las líneas (usando los newlines)
        if (i == '\n'){
            //Ubicación de máxima intensidad en el archivo ppm
            if (contadorDeLineas == 2){
                std::cout << "Máxima intensidad: " << maxIntensidadStr << "\n";
                maxIntensidadInt  = std::stoi(maxIntensidadStr);

                if (maxIntensidadInt < 256){
                    std::cout << "Intensidad baja";
                }
            }
            contadorDeLineas++;
        } else if ((i == ' ') || (i == '\t')){
            std::cout << "\ndo nothing\n";
        } else {
            if (contadorDeLineas == 2) {
                maxIntensidadStr.append(1, static_cast<char>(i));
            }
            //pet[contador] = *i;
            //if (contadorDeLineas >= 3){
                //if (contador >= 1){
                    //caster = (static_cast<u_int16_t>(pet[0]) << 8) | pet[1];
                    //contador = 0;
                    //std::cout << "casted: " << caster << "\n";
                //} else {
                    //contador++;
                //}
            //}
        }
        //contador_aux++;
    }

    // Lógica para el comando 'maxlevel'
    std::cout << "\nProcessing 'maxlevel' for file: " << inputFile.size() << " with max level: " << maxLevel << '\n';

    // Aquí iría la lógica para modificar el nivel máximo del archivo
}

// TODO
//ELIMINACIÓN DE COLORES POCO FRECUENTES

    //ESTAMOS EN AOS -> ARRAY OF STRUCTS
    // HABRÁ UN ARRAY QUE CONTENGA TODOS LOS PÍXELES, CADA POSICIÓN DEL ARRAY TENDRÁ TRES CAMPOS (R G Y B)
    //PSEUDOCODE
    //RECIBE UN ARGUMENTO:
        //Número entero positivo -> Número de colores que hay que eliminar
    //PASOS A SEGUIR:
        //1.- DETERMINAR LA FRECUENCIA ABSOLUTA DE CADA COLOR
        //2.- ELIGE LOS QUE APAREZCAN MENOS VECES
            //Si hay empate
                //Eliminar primero valor mayor en componente b
                //Luego los de mayor valor en componente g
                //Finalmente los de mayor valor en componente r
        //3.- CALCULAR DISTANCIA EUCLÍDEA CON LOS DEMÁS COLORES
        //4.- SUSTITUCIÓN

//VOY A IMAGINAR QUE HAY UN STRUCT PARA CADA COLOR (lo voy a llamar Color)
//Y UN VECTOR LLAMADO ArrayOfColors QUE CONTIENE TODOS LOS COLORES DE LA IMAGEN


// ******* COLOR IMPLEMENTADO EN COLOR.HPP *******
//FUNCIÓN PARA CALCULAR LA DISTANCIA EUCLÍDEA
// double distancia_euclidiana(const Color& c1, const Color& c2) {
//     return std::sqrt(std::pow(c1.red - c2.red, 2) +
//                      std::pow(c1.green - c2.green, 2) +
//                      std::pow(c1.blue - c2.blue, 2));
// }


//FUNCIÓN PARA CONTAR LA FRECUENCIA DE CADA COLOR
//Un unordered_map es como un diccionario pero su tiempo de búsqueda es O(1), y si se repite la clave, se sobreescribe
//Paso el vector por referencia para no hacer una copia y así ser más eficiente
// std::unordered_map<Color, int> contar_frecuencia(const std::vector<Color>& pixeles) {
//
//     std::unordered_map<Color, int> frecuencia;
//
//     for (const auto& pixel : pixeles) {
//         frecuencia[pixel]++;
//     }
//
//     return frecuencia;
// }

// FUNCIÓN PARA ENCONTRAR LOS COLORES MENOS FRECUENTES
// Para encontrar los colores menos frecuentes necesitamos que el unordered map pase a ser un vector de pares, ya que los vectores son ordenables
// Ordenaremos el vector de menor a mayor frecuencia y seleccionaremos los primeros 'n' colores

//Toma como parámetro de entrada el unordered map (llamado frecuencia) y el número de colores a seleccionar, y retorna el vector de los n colores menos frecuentes

// std::vector<Color> encontrar_colores_menos_frecuentes(const std::unordered_map<Color, int>& frecuencia, long unsigned int n) {
//
//     // Convertir el unordered_map en un vector de pares (color, frecuencia)
//     std::vector<std::pair<Color, int>> colores_frecuentes(frecuencia.begin(), frecuencia.end());
//
//     // Ordenar por frecuencia ascendente
//     //sort es parte de <include algorithm>
//     std::sort(colores_frecuentes.begin(), colores_frecuentes.end(),
//         [](const auto& a, const auto& b) {
//             return a.second < b.second; // Comparar por la frecuencia (segundo elemento del par)
//         }
//     );
//
//     // Seleccionar los primeros 'n' colores menos frecuentes
//     //Creamos un vector vacío de colores
//     //Metemos en el vector vacío los 'n' primeros colores menos frecuentes
//     std::vector<Color> menos_frecuentes;
//     for (size_t i = 0; i < static_cast<size_t>(n) && i < colores_frecuentes.size(); ++i) {
//       menos_frecuentes.push_back(colores_frecuentes[i].first);
//     }
//
//
//     return menos_frecuentes;
// }

// void distancias_euclideas(std::vector<Color>& menos_frecuentes, std::vector<Color>& pixeles) {
    //Calcular la distancia euclídea con los demás colores
    // for (const auto& pixel : pixeles) {
    //    for (const auto& color : menos_frecuentes) {
            // double distancia = distancia_euclidiana(pixel, color);
            // Hacer algo con la distancia
    //    }
    // }
// }

//void processCutfreq(int numColors, std::vector<Color>& pixeles) {
    ////Determinar la frecuencia absoluta de cada color
    //std::unordered_map<Color, int> frecuencia;
    //frecuencia = contar_frecuencia(pixeles);

    ////Encontrar los colores menos frecuentes y ordenarlos
    //std::vector<Color> menos_frecuentes;
    //menos_frecuentes = encontrar_colores_menos_frecuentes(frecuencia, numColors);

    ////Calcular la distancia euclídea con los demás colores
//}

// COMPRESS AOS
std::vector<Color> extractImagePixels(const std::vector<uint8_t>& fileData, const PPMHeader& header) {
    const size_t numPixels = static_cast<size_t>(header.width) * static_cast<size_t>(header.height);
    const size_t pixelSize = (header.maxColorValue > MAX_COLOR_VALUE_8BIT) ? 6 : 3;
    const size_t pixelStart = fileData.size() - numPixels * pixelSize;

    std::vector<Color> imagePixels;
    imagePixels.reserve(numPixels);

    for (size_t i = pixelStart; i < fileData.size(); i += pixelSize) {
        imagePixels.push_back(Color::fromBinary(&fileData[i], header));
    }

    return imagePixels;
}

std::pair<std::vector<Color>, std::unordered_map<Color, int>> createColorTable(const std::vector<Color>& imagePixels) {
    std::vector<Color> colorTable;
    colorTable.reserve(COLOR_TABLE_RESERVE_SIZE);

    std::unordered_map<Color, int> colorIndex;
    colorIndex.reserve(COLOR_TABLE_RESERVE_SIZE);

    // Crear la tabla de colores y actualizar el mapa de índices
    for (const auto& pixel : imagePixels) {
        auto [it, inserted] = colorIndex.emplace(pixel, static_cast<int>(colorTable.size()));
        if (inserted) {
            colorTable.push_back(pixel);
        }
    }

    // Ordenar la tabla de colores (orden lexicográfico RGB)
    std::ranges::sort(colorTable.begin(), colorTable.end(), [](const Color& colorA, const Color& colorB) {
        return std::tie(colorA.rgb.red, colorA.rgb.green, colorA.rgb.blue) <
               std::tie(colorB.rgb.red, colorB.rgb.green, colorB.rgb.blue);
    });

    // Actualizar el mapa de índices después de ordenar
    colorIndex.clear();
    colorIndex.reserve(colorTable.size());
    for (size_t i = 0; i < colorTable.size(); ++i) {
        colorIndex[colorTable[i]] = static_cast<int>(i);
    }

    return {colorTable, colorIndex};
}

void appendColorTable(std::vector<uint8_t>& compressedData, const std::vector<Color>& colorTable, const PPMHeader& header) {
    for (const auto& color : colorTable) {
        color.writeToBinary(compressedData, header);
    }
}

void appendPixelIndices(std::vector<uint8_t>& compressedData, const std::vector<Color>& imagePixels, const std::unordered_map<Color, int>& colorIndex) {
    size_t colorIndexSize = 1;
    if (colorIndex.size() > COLOR_TABLE_RESERVE_SIZE && colorIndex.size() <= MAX_COLOR_VALUE_16BIT) {
        colorIndexSize = 2;
    } else if (colorIndex.size() > MAX_COLOR_VALUE_16BIT) {
        colorIndexSize = 4;
    }

    compressedData.reserve(compressedData.size() + imagePixels.size() * colorIndexSize);

    for (const auto& pixel : imagePixels) {
        const int index = colorIndex.at(pixel);  // Declarar 'index' como const

        auto appendIndex = [&compressedData](const int index, const size_t size) {
            for (size_t i = 0; i < size; ++i) {
                compressedData.push_back(static_cast<uint8_t>((index >> (BITS_PER_BYTE * i)) & BYTE_MASK));
            }
        };
        appendIndex(index, colorIndexSize);
    }
}

void compressAoS(const FilePaths& paths) {
    std::string outputFile = ensureCppmExtension(paths.outputFile);

    const std::vector<uint8_t> fileData = BinaryIO::readBinaryFile(paths.inputFile);
    if (fileData.empty()) {
        std::cerr << "Error: No se pudo abrir o leer el archivo de entrada: " << paths.inputFile << '\n';
        return;
    }

    PPMHeader header{};
    if (!readPPMHeader(paths.inputFile, header)) {
        std::cerr << "Error al leer el encabezado del archivo PPM." << '\n';
        return;
    }

    auto imagePixels = extractImagePixels(fileData, header);
    auto [colorTable, colorIndex] = createColorTable(imagePixels);
    std::string headerStr = generateHeader(header, static_cast<int>(colorTable.size()));

    std::vector<uint8_t> compressedData;
    compressedData.reserve(
        headerStr.size() +
        static_cast<size_t>(header.width) * static_cast<size_t>(header.height)
    );

    compressedData.insert(compressedData.end(), headerStr.begin(), headerStr.end());
    appendColorTable(compressedData, colorTable, header);
    appendPixelIndices(compressedData, imagePixels, colorIndex);

    BinaryIO::writeBinaryFile(outputFile, compressedData);
}

Image vectorToImage(const std::vector<uint8_t>& data, int width, int height, int channels) {
  if (data.size() != static_cast<size_t>(width * height * channels)) {
    throw std::runtime_error("Data size does not match width, height, and channels.");
  }

  Image image(static_cast<size_t>(height), std::vector<Pixel>(static_cast<size_t>(width)));
  size_t index = 0;

  for (size_t y = 0; y < static_cast<size_t>(height); ++y) {
    for (size_t x = 0; x < static_cast<size_t>(width); ++x) {
      Pixel pixel;
      pixel.r = data[index];
      pixel.g = (channels > 1) ? data[index + 1] : data[index];
      pixel.b = (channels > 2) ? data[index + 2] : data[index];
      image[y][x] = pixel;
      index += static_cast<size_t>(channels);
    }
  }

  return image;
}

std::vector<uint8_t> imageToVector(const Image& image, int channels) {
    std::vector<uint8_t> data;
    data.reserve(image.size() * image[0].size() * static_cast<size_t>(channels));

    for (const auto& row : image) {
        for (const auto& pixel : row) {
            data.push_back(pixel.r);
            if (channels > 1) data.push_back(pixel.g);
            if (channels > 2) data.push_back(pixel.b);
        }
    }

    return data;
}

Image resizeImageAoS(const Image& image, int newWidth, int newHeight) {
    int originalWidth = static_cast<int>(image[0].size());
    int originalHeight = static_cast<int>(image.size());

    // Crear la imagen escalada
    Image resizedImage(static_cast<size_t>(newHeight), std::vector<Pixel>(static_cast<size_t>(newWidth)));

    // Calcular las proporciones sin redondeo
    float xRatio = static_cast<float>(originalWidth - 1) / static_cast<float>(newWidth - 1);
    float yRatio = static_cast<float>(originalHeight - 1) / static_cast<float>(newHeight - 1);

    for (int y = 0; y < newHeight; ++y) {
        for (int x = 0; x < newWidth; ++x) {
            // Convertir `x` y `y` a `float` explícitamente antes de multiplicarlos por `xRatio` y `yRatio`
            float srcX = static_cast<float>(x) * xRatio;
            float srcY = static_cast<float>(y) * yRatio;

            // Índices enteros de los píxeles cercanos
            int xL = static_cast<int>(srcX);
            int yL = static_cast<int>(srcY);
            int xH = std::min(xL + 1, originalWidth - 1);
            int yH = std::min(yL + 1, originalHeight - 1);

            // Cálculo de pesos para la interpolación
            float xWeight = srcX - static_cast<float>(xL);
            float yWeight = srcY - static_cast<float>(yL);

            // Píxeles cercanos para interpolación
            Pixel topLeft = image[static_cast<size_t>(yL)][static_cast<size_t>(xL)];
            Pixel topRight = image[static_cast<size_t>(yL)][static_cast<size_t>(xH)];
            Pixel bottomLeft = image[static_cast<size_t>(yH)][static_cast<size_t>(xL)];
            Pixel bottomRight = image[static_cast<size_t>(yH)][static_cast<size_t>(xH)];

            // Interpolación bilineal
            Pixel interpolatedPixel;
            interpolatedPixel.r = static_cast<uint8_t>(
                topLeft.r * (1 - xWeight) * (1 - yWeight) +
                topRight.r * xWeight * (1 - yWeight) +
                bottomLeft.r * (1 - xWeight) * yWeight +
                bottomRight.r * xWeight * yWeight
            );
            interpolatedPixel.g = static_cast<uint8_t>(
                topLeft.g * (1 - xWeight) * (1 - yWeight) +
                topRight.g * xWeight * (1 - yWeight) +
                bottomLeft.g * (1 - xWeight) * yWeight +
                bottomRight.g * xWeight * yWeight
            );
            interpolatedPixel.b = static_cast<uint8_t>(
                topLeft.b * (1 - xWeight) * (1 - yWeight) +
                topRight.b * xWeight * (1 - yWeight) +
                bottomLeft.b * (1 - xWeight) * yWeight +
                bottomRight.b * xWeight * yWeight
            );

            // Asignar el píxel interpolado a la imagen escalada
            resizedImage[static_cast<size_t>(y)][static_cast<size_t>(x)] = interpolatedPixel;
        }
    }

    return resizedImage;
}


