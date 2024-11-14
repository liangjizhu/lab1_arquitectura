#include "imageaos.hpp"

#include "binaryio.hpp"
#include "progargs.hpp"
#include "imageinfo.hpp"
#include "color.hpp"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <string>

// Constante descriptiva para el tamaño de reserva inicial de la tabla de colores
constexpr size_t COLOR_TABLE_RESERVE_SIZE = 256;
constexpr uint16_t BYTE_MASK = 0xFF;
constexpr uint16_t MAX_COLOR_VALUE_8BIT = 255;
constexpr uint16_t MAX_COLOR_VALUE_16BIT = 65535;
constexpr uint8_t BITS_PER_BYTE = 8;

// TODO:
//  - PARA ESTA OPERACIÓN CREO QUE SE PODRÍA OPTIMIZAR SI SE EJECUTA AL MISMO TIEMPO QUE SE LEA EL ARCHIVO
//  - clang-tidy
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
// ELIMINACIÓN DE COLORES POCO FRECUENTES

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
// Para encontrar los colores menos frecuentes necesitamos que el unordered map pase a ser un vector
// de pares, ya que los vectores son ordenables Ordenaremos el vector de menor a mayor frecuencia y
// seleccionaremos los primeros 'n' colores

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
// Calcular la distancia euclídea con los demás colores
// for (const auto& pixel : pixeles) {
//    for (const auto& color : menos_frecuentes) {
// double distancia = distancia_euclidiana(pixel, color);
// Hacer algo con la distancia
// }
// }
// }

// void processCutfreq(int numColors, std::vector<Color>& pixeles) {
////Determinar la frecuencia absoluta de cada color
// std::unordered_map<Color, int> frecuencia;
// frecuencia = contar_frecuencia(pixeles);

////Encontrar los colores menos frecuentes y ordenarlos
// std::vector<Color> menos_frecuentes;
// menos_frecuentes = encontrar_colores_menos_frecuentes(frecuencia, numColors);

////Calcular la distancia euclídea con los demás colores
//}

/********************************************* COMPRESS AOS *********************************************/
// Extraer los píxeles de la imagen a partir de los datos binarios (fileData)
std::vector<Color> extractImagePixels(const std::vector<uint8_t>& fileData, const PPMHeader& header) {
    const size_t numPixels = static_cast<size_t>(header.width) * static_cast<size_t>(header.height);
    // Determina el número de píxeles y el tamaño de cada píxel (3 bytes para imágenes de 8 bits y 6 bytes para imágenes de 16 bits)
    const size_t pixelSize = (header.maxColorValue > MAX_COLOR_VALUE_8BIT) ? 6 : 3;
    const size_t pixelStart = fileData.size() - numPixels * pixelSize;
    // Lista de colores
    std::vector<Color> imagePixels;
    imagePixels.reserve(numPixels);

    for (size_t i = pixelStart; i < fileData.size(); i += pixelSize) {
        imagePixels.push_back(Color::fromBinary(&fileData[i], header));
    }

    return imagePixels;
}

// Genera una tabla de colores únicos (colorTable) y un mapa de índices (colorIndex)
std::pair<std::vector<Color>, std::unordered_map<Color, int>> createColorTable(const std::vector<Color>& imagePixels) {
    // Se reserva memoria del mismo tamaño, tanto para la tabla de colores como para el mapa de índices
    std::vector<Color> colorTable;
    colorTable.reserve(COLOR_TABLE_RESERVE_SIZE);

    std::unordered_map<Color, int> colorIndex;
    colorIndex.reserve(COLOR_TABLE_RESERVE_SIZE);

    // Crear la tabla de colores y actualizar el mapa de índices
    for (const auto& pixel : imagePixels) {
        // Si pixel ya está en colorIndex, inserted será false y no se agrega a la tabla de colores (colorTable)
        // Si pixel no está en el mapa, se inserta con el índice actual de colorTable y se agrega a la tabla
        auto [it, inserted] = colorIndex.emplace(pixel, static_cast<int>(colorTable.size()));
        if (inserted) {
            colorTable.push_back(pixel);
        }
    }

    // Ordenar la tabla de colores (orden RGB)
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

// Añade la tabla de colores al archivo comprimido
void appendColorTable(std::vector<uint8_t>& compressedData, const std::vector<Color>& colorTable, const PPMHeader& header) {
    for (const auto& color : colorTable) {
        color.writeToBinary(compressedData, header);
    }
}

// Añade los índices de los colores de los píxeles al archivo comprimido
void appendPixelIndices(std::vector<uint8_t>& compressedData, const std::vector<Color>& imagePixels, const std::unordered_map<Color, int>& colorIndex) {
    // Determina el tamaño del índice (1, 2 o 4 bytes) dependiendo del tamaño de la tabla de colores.
    size_t colorIndexSize = 1;
    if (colorIndex.size() > COLOR_TABLE_RESERVE_SIZE && colorIndex.size() <= MAX_COLOR_VALUE_16BIT) {
        colorIndexSize = 2;
    } else if (colorIndex.size() > MAX_COLOR_VALUE_16BIT) {
        colorIndexSize = 4;
    }
    // Reservar memoria para los índices de los colores
    compressedData.reserve(compressedData.size() + imagePixels.size() * colorIndexSize);
    // Recorre cada píxel de imagePixels
    for (const auto& pixel : imagePixels) {
        // at() -> excepción si el píxel no está en el mapa, garantizando seguridad de acceso
        const int index = colorIndex.at(pixel);

        auto appendIndex = [&compressedData](const int index, const size_t size) {
            for (size_t i = 0; i < size; ++i) {
                // Añadir el índice al archivo comprimido, byte por byte
                compressedData.push_back(static_cast<uint8_t>((index >> (BITS_PER_BYTE * i)) & BYTE_MASK));
            }
        };
        appendIndex(index, colorIndexSize);
    }
}

// Función principal para comprimir una imagen en formato AOS
void compressAoS(const FilePaths& paths) {
    // Asegurarse de que la extensión del archivo de salida sea .cppm
    std::string const outputFile = ensureCppmExtension(paths.outputFile);
    // Leer los datos binarios del archivo de entrada
    const std::vector<uint8_t> fileData = BinaryIO::readBinaryFile(paths.inputFile);
    if (fileData.empty()) {
        std::cerr << "Error: No se pudo abrir o leer el archivo de entrada: " << paths.inputFile << '\n';
        return;
    }
    // Leer el encabezado del archivo PPM
    PPMHeader header{};
    if (!readPPMHeader(paths.inputFile, header)) {
        std::cerr << "Error al leer el encabezado del archivo PPM." << '\n';
        return;
    }
    // Extraer los píxeles de la imagen a partir de los datos binarios
    auto imagePixels = extractImagePixels(fileData, header);
    // Crear la tabla de colores y el mapa de índices
    auto [colorTable, colorIndex] = createColorTable(imagePixels);
    // Generar el encabezado del archivo comprimido
    std::string headerStr = generateHeader(header, static_cast<int>(colorTable.size()));
    // Crear un vector para almacenar los datos comprimidos
    std::vector<uint8_t> compressedData;
    compressedData.reserve(
        headerStr.size() +
        static_cast<size_t>(header.width) * static_cast<size_t>(header.height)
    );
    // Añadir el encabezado y los datos comprimidos al archivo
    compressedData.insert(compressedData.end(), headerStr.begin(), headerStr.end());
    appendColorTable(compressedData, colorTable, header);
    appendPixelIndices(compressedData, imagePixels, colorIndex);
    // Escribir los datos comprimidos en el archivo de salida
    BinaryIO::writeBinaryFile(outputFile, compressedData);
}

// RESIZE AOS
Image vectorToImage(const std::vector<uint8_t>& data, int width, int height, int channels) {
  if (data.size() != static_cast<size_t>(width) * static_cast<size_t>(height) * static_cast<size_t>(channels)) {
    throw std::runtime_error("Data size does not match width, height, and channels.");
  }

  Image image(static_cast<size_t>(height), std::vector<Pixel>(static_cast<size_t>(width)));
  size_t index = 0;

  for (size_t y_pos= 0; y_pos< static_cast<size_t>(height); ++y_pos) {
    for (size_t x_pos = 0; x_pos < static_cast<size_t>(width); ++x_pos) {
      Pixel pixel = {
        .r = data[index],
        .g = (channels > 1) ? data[index + 1] : data[index],
        .b = (channels > 2) ? data[index + 2] : data[index]
    };
      image[y_pos][x_pos] = pixel;

      pixel.r = data[index];
      pixel.g = (channels > 1) ? data[index + 1] : data[index];
      pixel.b = (channels > 2) ? data[index + 2] : data[index];
      image[y_pos][x_pos] = pixel;
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
            if (channels > 1) {data.push_back(pixel.g);}
            if (channels > 2) {data.push_back(pixel.b);}
        }
    }

    return data;
}

namespace {
  struct ScaleRatios {
    float xRatio;
    float yRatio;
  };

  std::pair<float, float> computeSourceCoordinates(int targetX, int targetY, const ScaleRatios& ratios) {
    const float srcX = static_cast<float>(targetX) * ratios.xRatio;
    const float srcY = static_cast<float>(targetY) * ratios.yRatio;
    return {srcX, srcY};
  }
}


namespace {
  struct SourceCoordinates {
    float srcX;
    float srcY;
    size_t lowerX;
    size_t lowerY;
  };

  // Updated function to calculate weights with better precision
  std::pair<float, float> calculateWeights(const SourceCoordinates& coords) {
    const float xWeight = std::clamp(coords.srcX - static_cast<float>(coords.lowerX), 0.0f, 1.0f);
    const float yWeight = std::clamp(coords.srcY - static_cast<float>(coords.lowerY), 0.0f, 1.0f);
    return {xWeight, yWeight};
  }
}


// Helper function to perform bilinear interpolation
Pixel interpolatePixel(const Pixel& topLeft, const Pixel& topRight,
                       const Pixel& bottomLeft, const Pixel& bottomRight,
                       float xWeight, float yWeight) {
  Pixel interpolatedPixel = {0, 0, 0};

  interpolatedPixel.r = static_cast<uint8_t>(
      (topLeft.r * (1.0f - xWeight) * (1.0f - yWeight)) +
      (topRight.r * xWeight * (1.0f - yWeight)) +
      (bottomLeft.r * (1.0f - xWeight) * yWeight) +
      (bottomRight.r * xWeight * yWeight)
  );
  interpolatedPixel.g = static_cast<uint8_t>(
      (topLeft.g * (1.0f - xWeight) * (1.0f - yWeight)) +
      (topRight.g * xWeight * (1.0f - yWeight)) +
      (bottomLeft.g * (1.0f - xWeight) * yWeight) +
      (bottomRight.g * xWeight * yWeight)
  );
  interpolatedPixel.b = static_cast<uint8_t>(
      (topLeft.b * (1.0f - xWeight) * (1.0f - yWeight)) +
      (topRight.b * xWeight * (1.0f - yWeight)) +
      (bottomLeft.b * (1.0f - xWeight) * yWeight) +
      (bottomRight.b * xWeight * yWeight)
  );
  return interpolatedPixel;
}



Image resizeImageAoS(const Image& image, int newWidth, int newHeight) {
    const size_t originalWidth = image[0].size();
    const size_t originalHeight = image.size();

    // Create the resized image
    Image resizedImage(static_cast<size_t>(newHeight), std::vector<Pixel>(static_cast<size_t>(newWidth)));

    ScaleRatios ratios = {static_cast<float>(originalWidth - 1) / static_cast<float>(newWidth - 1),
                          static_cast<float>(originalHeight - 1) / static_cast<float>(newHeight - 1)};

    for (size_t y = 0; y < static_cast<size_t>(newHeight); ++y) {
        for (size_t x = 0; x < static_cast<size_t>(newWidth); ++x) {
            auto [srcX, srcY] = computeSourceCoordinates(static_cast<int>(x), static_cast<int>(y), ratios);

            // Integer indices of the neighboring pixels, cast to size_t
            const size_t xL = std::min(static_cast<size_t>(srcX), originalWidth - 1);
            const size_t yL = std::min(static_cast<size_t>(srcY), originalHeight - 1);
            const size_t xH = std::clamp(xL + 1, size_t(0), originalWidth - 1);
            const size_t yH = std::clamp(yL + 1, size_t(0), originalHeight - 1);

            // Create a SourceCoordinates instance
            SourceCoordinates coords = {srcX, srcY, xL, yL};

            // Calculate interpolation weights
            auto [xWeight, yWeight] = calculateWeights(coords);

            // Get the four neighboring pixels
            const Pixel& topLeft = image[yL][xL];
            const Pixel& topRight = image[yL][xH];
            const Pixel& bottomLeft = image[yH][xL];
            const Pixel& bottomRight = image[yH][xH];

            // Interpolate the pixel
            resizedImage[y][x] = interpolatePixel(topLeft, topRight, bottomLeft, bottomRight, xWeight, yWeight);
        }
    }

    return resizedImage;
}


//PRUEBAS CON KTREES
//*******************PROCESSCUTFREQ**********************
//Toma como parámetro de entrada el unordered map (llamado frecuencia) y el número de colores a seleccionar, y retorna el vector de los n colores menos frecuentes

//NOLINTBEGIN(misc-no-recursion)
const int MAX_COLOR_VALUE = 255;


std::vector<Color> encontrar_colores_menos_frecuentes(const std::unordered_map<Color, int, HashColor>& frecuencia, int n) {
    
    // Convertir el unordered_map en un vector de pares (color, frecuencia)
    std::vector<std::pair<Color, int>> colores_frecuentes(frecuencia.begin(), frecuencia.end());

    // Ordenar por frecuencia ascendente
    //sort es parte de <include algorithm>
    std::sort(colores_frecuentes.begin(), colores_frecuentes.end(), 
        [](const auto& colora, const auto& colorb) {
            return colora.second < colorb.second; // Comparar por la frecuencia (segundo elemento del par)
        }
    );

    // Seleccionar los primeros 'n' colores menos frecuentes
    //Creamos un vector vacío de colores
    //Metemos en el vector vacío los 'n' primeros colores menos frecuentes
    std::vector<Color> menos_frecuentes;
    for (size_t i = 0; i < static_cast<size_t>(n) && i < colores_frecuentes.size(); ++i) {
      menos_frecuentes.push_back(colores_frecuentes[i].first);
    }
    return menos_frecuentes;
}

std::vector<Color> encontrar_colores_menos_frecuentes_2(const std::unordered_map<Color, int, HashColor>& frecuencia, int n) {
    // Convertir el unordered_map en un vector de pares (color, frecuencia)
    std::vector<std::pair<Color, int>> colores_frecuentes(frecuencia.begin(), frecuencia.end());

    // Usar nth_element para encontrar los n colores menos frecuentes
    std::nth_element(colores_frecuentes.begin(), colores_frecuentes.begin() + n, colores_frecuentes.end(),
        [](const auto& colora, const auto& colorb) {
            return colora.second < colorb.second; // Comparar por la frecuencia (segundo elemento del par)
        }
    );

    // Seleccionar los primeros 'n' colores menos frecuentes
    std::vector<Color> menos_frecuentes;
    for (size_t i = 0; i < static_cast<size_t>(n) && i < colores_frecuentes.size(); ++i) {
        menos_frecuentes.push_back(colores_frecuentes[i].first);
    }

    return menos_frecuentes;
}

void escribirPPM(const std::string& filename, const std::vector<Color>& pixeles, int width, int height) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "Error: No se pudo abrir el archivo para escribir: " << filename << '\n';
        std::cerr << "Error: No se pudo abrir el archivo para escribir: " << filename << '\n';
        return;
    }

    // Escribir encabezado PPM
    file << "P6\n" << width << " " << height << "\n255\n"; // P6 es el formato binario para PPM

    // Escribir los datos de los píxeles en formato binario
    for (const auto& pixel : pixeles) {
        file.put(static_cast<char>(pixel.rgb.red));
        file.put(static_cast<char>(pixel.rgb.green));
        file.put(static_cast<char>(pixel.rgb.blue));
    }

    file.close();
}

std::tuple<int, int> getPPMDimensions(const std::string& inputFile) {
    int width = 0;
    int height = 0;
    std::string format;
    int maxColorValue = 0;

    // Abrir archivo de entrada en modo texto
    std::ifstream file(inputFile);
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo de entrada: " << inputFile << '\n';
        return {width, height}; // Retorna dimensiones 0 si falla
    }

    // Leer el encabezado del archivo PPM
    file >> format >> width >> height >> maxColorValue;

    // Verificar el formato y el valor máximo de color
    if (format != "P6" && format != "P3") { // P3 es el formato ASCII, P6 es binario
        std::cerr << "Error: Formato PPM no soportado en " << inputFile << '\n';
        return {width, height}; // Retorna dimensiones leídas hasta el momento
    }

    // Ignorar el salto de línea después del encabezado
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Cerrar el archivo
    file.close();

    return {width, height}; // Retornar ancho y alto
}

struct KDNode {
    Color color;
    std::unique_ptr<KDNode> left;  // Cambiado a std::unique_ptr
    std::unique_ptr<KDNode> right; // Cambiado a std::unique_ptr
    KDNode(Color color) : color(color), left(nullptr), right(nullptr) {}
};


// Función de distancia euclidiana
/*double distancia_euclidiana(const Color& c1, const Color& c2) {
    return std::sqrt(std::pow(c1.red - c2.red, 2) +
                     std::pow(c1.green - c2.green, 2) +
                     std::pow(c1.blue - c2.blue, 2));
}*/

// Construcción del K-D Tree en 3 dimensiones (RGB)
std::unique_ptr<KDNode> construirKDTree(std::vector<Color>& colores, int depth = 0) {
    if (colores.empty()){
        return nullptr;
        }

    const int axis = depth % 3;
    auto comp = [axis](const Color& colora, const Color& colorb) {
        if (axis == 0) {
            return colora.rgb.red < colorb.rgb.red;
        }
        if (axis == 1) {
            return colora.rgb.green < colorb.rgb.green;
        }
        if ((axis != 0) && (axis != 1)) {
            return colora.rgb.blue < colorb.rgb.blue;
        }
        throw std::invalid_argument("Axis value out of range");
    };

    const std::vector<Color>::size_type mid = colores.size() / 2;
    
    // Usa nth_element para colocar el mediano en su posición sin ordenar todo el vector
    std::nth_element(colores.begin(), colores.begin() + static_cast<std::vector<Color>::difference_type>(mid), colores.end(), comp);

    // Crear el nodo actual con el elemento mediano
    auto node = std::make_unique<KDNode>(colores[mid]); // Cambiado a std::make_unique

    // Dividir en sub-vectores usando el mediano como pivote
    std::vector<Color> leftColors(colores.begin(), colores.begin() + static_cast<std::vector<Color>::difference_type>(mid));
    std::vector<Color> rightColors(colores.begin() + static_cast<std::vector<Color>::difference_type>(mid) + 1, colores.end());

    node->left = construirKDTree(leftColors, depth + 1);
    node->right = construirKDTree(rightColors, depth + 1);

    return node; // Devuelve std::unique_ptr
}

// Función de búsqueda de vecino más cercano
// Función de distancia cuadrada para evitar la raíz cuadrada
double distanciaCuadrada(const Color& colora, const Color& colorb) {
    return (colora.rgb.red - colorb.rgb.red) * (colora.rgb.red - colorb.rgb.red) +
           (colora.rgb.green - colorb.rgb.green) * (colora.rgb.green - colorb.rgb.green) +
           (colora.rgb.blue - colorb.rgb.blue) * (colora.rgb.blue - colorb.rgb.blue);
}

std::pair<Color, double> buscarVecinoMasCercano(std::unique_ptr<KDNode>& root, const Color& target, int depth) {
    if (!root) {
        // Si el nodo es nulo, devolvemos un valor con la máxima distancia posible
        return {Color(RGBColor{0, 0, 0}), std::numeric_limits<double>::max()};
    }
    // Calcular la distancia cuadrada
    const double distancia = distanciaCuadrada(target, root->color);
    Color mejorColor = root->color;
    double mejorDistanciaCuadrada = distancia;
    // Determinar el eje de comparación actual
    const int axis = depth % 3;
    std::unique_ptr<KDNode>& siguiente = (axis == 0 && target.rgb.red < root->color.rgb.red) ||
                                         (axis == 1 && target.rgb.green < root->color.rgb.green) ||
                                         (axis == 2 && target.rgb.blue < root->color.rgb.blue) ? root->left : root->right;

    std::unique_ptr<KDNode>& opuesto = (siguiente == root->left) ? root->right : root->left;
    // Búsqueda recursiva en el subárbol relevante
    auto resultadoSiguiente = buscarVecinoMasCercano(siguiente, target, depth + 1);
    if (resultadoSiguiente.second < mejorDistanciaCuadrada) {
        mejorDistanciaCuadrada = resultadoSiguiente.second;
        mejorColor = resultadoSiguiente.first;
    }
    // Calcular la distancia cuadrada en el eje de partición
    double distanciaEje = 0;
    if (axis == 0) {
        distanciaEje = (target.rgb.red - root->color.rgb.red) * (target.rgb.red - root->color.rgb.red);
    } else if (axis == 1) {
        distanciaEje = (target.rgb.green - root->color.rgb.green) * (target.rgb.green - root->color.rgb.green);
    } else {
        distanciaEje = (target.rgb.blue - root->color.rgb.blue) * (target.rgb.blue - root->color.rgb.blue);
    }
    // Podar el subárbol opuesto si es necesario
    if (distanciaEje < mejorDistanciaCuadrada) {
        auto resultadoOpuesto = buscarVecinoMasCercano(opuesto, target, depth + 1);
        if (resultadoOpuesto.second < mejorDistanciaCuadrada) {
            mejorDistanciaCuadrada = resultadoOpuesto.second;
            mejorColor = resultadoOpuesto.first;
        }
    }
    return {mejorColor, mejorDistanciaCuadrada};
}


// Función para encontrar el color más cercano para una lista de colores menos frecuentes
std::vector<Color> encontrarColoresCercanos(std::unique_ptr<KDNode>& root, const std::vector<Color>& coloresMenosFrecuentes) {
    std::vector<Color> coloresCercanos;
    coloresCercanos.reserve(coloresMenosFrecuentes.size()); // Reservar espacio para evitar redimensionamientos

    for (const auto& color : coloresMenosFrecuentes) {
        // Capturar el resultado de buscarVecinoMasCercano
        auto [mejorColor, mejorDistanciaCuadrada] = buscarVecinoMasCercano(root, color, 0);
        
        // Agregar el mejor color encontrado a la lista de colores cercanos
        coloresCercanos.push_back(mejorColor);
    }

    return coloresCercanos;
}

std::vector<Color> readPixelsFromImage(const std::string& inputFile, int width, int height) {
    std::cout << "Leyendo píxeles de la imagen: " << inputFile << '\n';
    std::ifstream file(inputFile, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo de entrada: " << inputFile << '\n';
        return {};
    }

    // Saltamos el encabezado PPM
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    const size_t totalPixels = static_cast<size_t>(width) * static_cast<size_t>(height);
    std::vector<Color> pixelList;
    pixelList.reserve(totalPixels);

    // Leer todos los datos de píxeles de una vez en un solo bloque
    std::vector<char> buffer(totalPixels * 3);
    file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));

    // Procesar el buffer en bloques de tres bytes para extraer los colores
    for (size_t i = 0; i < buffer.size(); i += 3) {
        const RGBColor rgbColor = {
            static_cast<uint16_t>(buffer[i]),
            static_cast<uint16_t>(buffer[i + 1]),
            static_cast<uint16_t>(buffer[i + 2])
        };
        const Color pixelColor(rgbColor);
        pixelList.push_back(pixelColor);
    }

    file.close();
    std::cout << "Píxeles leídos" << '\n';
    return pixelList;
}

/*std::unordered_map<Color, int> calculateColorFrequency(const std::vector<Color>& pixelList) {
    std::cout << "Calculando frecuencias de colores" << '\n';
    std::unordered_map<Color, int> colorFrequency;

    // Reservar espacio para evitar realocaciones innecesarias
    colorFrequency.reserve(100000);  // Ajusta este número según tus expectativas

    // Usamos try_emplace para evitar búsquedas duplicadas
    for (const auto& color : pixelList) {
        colorFrequency.try_emplace(color, 0).first->second++;
    }

    return colorFrequency;
}*/

/*std::pair<std::vector<Color>, std::unordered_map<Color, int>> readImageAndStoreColorsTrial(const std::string& inputFile, int width, int height) {
    std::vector<Color> pixelList = readPixelsFromImage(inputFile, width, height);
    std::unordered_map<Color, int> colorFrequency = calculateColorFrequency(pixelList);
    return {pixelList, colorFrequency};
}*/
struct ImageData {
    std::vector<Color> pixels;
    std::vector<Color> uniqueColors;
    std::vector<int> colorCount;
    int width = 0;
    int height = 0;
};

// Función para cargar y preparar los datos de la imagen
ImageData loadImageData(const std::string& inputFile) {
    ImageData data;
    auto [width, height] = getPPMDimensions(inputFile);
    data.width = width;
    data.height = height;
    
    const std::vector<uint8_t> fileData = BinaryIO::readBinaryFile(inputFile);
    if (fileData.empty()) {
        throw std::runtime_error("No se pudo abrir o leer el archivo de entrada: " + inputFile);
    }
    
    PPMHeader header{};
    if (!readPPMHeader(inputFile, header)) {
        throw std::runtime_error("Error al leer el encabezado del archivo PPM.");
    }
    
    data.pixels = extractImagePixels(fileData, header);
    return data;
}

// Función para calcular frecuencias de colores
void calculateColorFrequencies(ImageData& data) {
    constexpr int colorBits = 8;
    const size_t arraySize = static_cast<size_t>(1) << (3 * colorBits);
    data.colorCount.resize(arraySize, 0);
    data.uniqueColors.reserve(arraySize / 4);
    
    for (const auto& pixel : data.pixels) {
        const size_t index = (static_cast<size_t>(pixel.rgb.red) << 16) | 
                           (static_cast<size_t>(pixel.rgb.green) << 8) | 
                           static_cast<size_t>(pixel.rgb.blue);
        if (data.colorCount.at(index)++ == 0) {
            data.uniqueColors.push_back(pixel);
        }
    }
}

// Función para encontrar los colores menos frecuentes
std::vector<Color> findLessFrequentColors(const ImageData& data, int numColors) {
    std::vector<std::pair<int, Color>> colorFreqPairs;
    colorFreqPairs.reserve(data.uniqueColors.size());
    // Crear los pares (frecuencia, color)
    for (const auto& color : data.uniqueColors) {
        const size_t index = (static_cast<size_t>(color.rgb.red) << 16) | 
                           (static_cast<size_t>(color.rgb.green) << 8) | 
                           static_cast<size_t>(color.rgb.blue);
        colorFreqPairs.emplace_back(data.colorCount.at(index), color);
    }
    // Función de comparación que considera BGR en orden de prioridad
    auto compareColors = [](const auto& pair1, const auto& pair2) {
        // Si las frecuencias son diferentes, ordenar por frecuencia
        if (pair1.first != pair2.first) {
            return pair1.first < pair2.first;
        }
        // Si las frecuencias son iguales, ordenar por componente B
        if (pair1.second.rgb.blue != pair2.second.rgb.blue) {
            return pair1.second.rgb.blue > pair2.second.rgb.blue;
        }
        // Si B es igual, ordenar por componente G
        if (pair1.second.rgb.green != pair2.second.rgb.green) {
            return pair1.second.rgb.green > pair2.second.rgb.green;
        }
        // Si G es igual, ordenar por componente R
        return pair1.second.rgb.red > pair2.second.rgb.red;
    };
    const size_t num = std::min(static_cast<size_t>(numColors), colorFreqPairs.size());
    std::nth_element(
        colorFreqPairs.begin(),
        colorFreqPairs.begin() + static_cast<std::vector<Color>::difference_type>(num),
        colorFreqPairs.end(),
        compareColors
    );
    std::vector<Color> menos_frecuentes;
    menos_frecuentes.reserve(num);
    for (size_t i = 0; i < num; ++i) {
        menos_frecuentes.push_back(colorFreqPairs[i].second);
    }
    return menos_frecuentes;
}

// Función para preparar los colores restantes para el KD-Tree
std::vector<Color> prepareRemainingColors(const ImageData& data, 
                                        const std::vector<Color>& menosFrecuentes) {
    const size_t arraySize = data.colorCount.size();
    std::vector<bool> isLessFrequent(arraySize, false);
    
    for (const auto& color : menosFrecuentes) {
        const size_t index = (static_cast<size_t>(color.rgb.red) << 16) | 
                           (static_cast<size_t>(color.rgb.green) << 8) | 
                           static_cast<size_t>(color.rgb.blue);
        isLessFrequent.at(index) = true;
    }
    
    std::vector<Color> coloresRestantes;
    coloresRestantes.reserve(data.uniqueColors.size() - menosFrecuentes.size());
    
    for (const auto& color : data.uniqueColors) {
        const size_t index = (static_cast<size_t>(color.rgb.red) << 16) | 
                           (static_cast<size_t>(color.rgb.green) << 8) | 
                           static_cast<size_t>(color.rgb.blue);
        if (!isLessFrequent.at(index)) {
            coloresRestantes.push_back(color);
        }
    }
    return coloresRestantes;
}

// Función para sustituir los colores menos frecuentes
void substituteColors(std::vector<Color>& pixels, 
                     const std::vector<Color>& menosFrecuentes,
                     std::unique_ptr<KDNode>& kdTreeRoot) {
    constexpr int colorBits = 8;
    const size_t arraySize = static_cast<size_t>(1) << (3 * colorBits);
    std::vector<bool> isLessFrequent(arraySize, false);
    std::vector<Color> substitutionLookup(arraySize);
    
    for (const auto& color : menosFrecuentes) {
        const size_t index = (static_cast<size_t>(color.rgb.red) << 16) | 
                           (static_cast<size_t>(color.rgb.green) << 8) | 
                           static_cast<size_t>(color.rgb.blue);
        isLessFrequent.at(index) = true;
        
        auto [colorMasCercano, _] = buscarVecinoMasCercano(kdTreeRoot, color, 0);
        substitutionLookup.at(index) = colorMasCercano;
    }
    
    for (auto& pixel : pixels) {
        const size_t index = (static_cast<size_t>(pixel.rgb.red) << 16) | 
                           (static_cast<size_t>(pixel.rgb.green) << 8) | 
                           static_cast<size_t>(pixel.rgb.blue);
        if (isLessFrequent.at(index)) {
            pixel = substitutionLookup.at(index);
        }
    }
}

// Función principal refactorizada
void processCutfreq(const std::string& inputFile, int numColors, const std::string& outputFile) {
    try {
        std::cout << "Leyendo imagen..." << '\n';
        ImageData imageData = loadImageData(inputFile);
        
        std::cout << "Calculando frecuencias..." << '\n';
        calculateColorFrequencies(imageData);
        
        std::cout << "Identificando colores menos frecuentes..." << '\n';
        const std::vector<Color> menosFrecuentes = findLessFrequentColors(imageData, numColors);
        
        std::cout << "Construyendo KD-Tree..." << '\n';
        std::vector<Color> coloresRestantes = prepareRemainingColors(imageData, menosFrecuentes);
        auto kdTreeRoot = construirKDTree(coloresRestantes);
        
        std::cout << "Realizando sustituciones..." << '\n';
        substituteColors(imageData.pixels, menosFrecuentes, kdTreeRoot);
        
        std::cout << "Escribiendo imagen modificada..." << '\n';
        escribirPPM(outputFile, imageData.pixels, imageData.width, imageData.height);
        
        std::cout << "Proceso completado." << '\n';
    } catch (const std::exception& e) {
        std::cerr << "Error durante el procesamiento: " << e.what() << '\n';
    }
}
//NOLINTEND(misc-no-recursion)
