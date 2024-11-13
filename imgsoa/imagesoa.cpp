#include "imagesoa.hpp"
#include "binaryio.hpp"
#include "imageinfo.hpp"
#include "color.hpp"
#include "progargs.hpp"
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>
#include <tuple>
#include <limits>
#include <cmath>

// Constantes descriptivas
constexpr uint16_t BYTE_MASK = 0xFF;
constexpr uint16_t MAX_COLOR_VALUE_8BIT = 255;
constexpr uint16_t MAX_COLOR_VALUE_16BIT = 65535;
constexpr uint8_t BITS_PER_BYTE = 8;
constexpr size_t COLOR_TABLE_RESERVE_SIZE = 256;

// Crear índice de colores
std::unordered_map<std::string, int> buildColorIndex(const ColorChannels& channels, ColorChannels& colorTable) {
    std::unordered_map<std::string, int> colorIndex;
    colorIndex.reserve(COLOR_TABLE_RESERVE_SIZE);

    size_t tableIndex = 0;

    for (size_t i = 0; i < channels.size(); ++i) {
        std::string const colorKey = std::to_string(channels.getRedChannel()[i]) + "," +
                                     std::to_string(channels.getGreenChannel()[i]) + "," +
                                     std::to_string(channels.getBlueChannel()[i]);

        auto [it, inserted] = colorIndex.emplace(colorKey, static_cast<int>(tableIndex));
        if (inserted) {
            colorTable.getRedChannel().push_back(channels.getRedChannel()[i]);
            colorTable.getGreenChannel().push_back(channels.getGreenChannel()[i]);
            colorTable.getBlueChannel().push_back(channels.getBlueChannel()[i]);
            tableIndex++;
        }
    }

    return colorIndex;
}

// Ordenar la tabla de colores en orden lexicográfico RGB
std::vector<size_t> sortColorTable(ColorChannels& colorTable) {
    std::vector<size_t> indices(colorTable.getRedChannel().size());
    std::iota(indices.begin(), indices.end(), 0);

    std::sort(indices.begin(), indices.end(), [&colorTable](size_t indexA, size_t indexB) {
        return std::tie(colorTable.getRedChannel()[indexA], colorTable.getGreenChannel()[indexA], colorTable.getBlueChannel()[indexA]) <
               std::tie(colorTable.getRedChannel()[indexB], colorTable.getGreenChannel()[indexB], colorTable.getBlueChannel()[indexB]);
    });

    return indices;
}

// Crear tabla de colores ordenada
ColorChannels createSortedColorTable(const ColorChannels& colorTable, const std::vector<size_t>& indices) {
    ColorChannels sortedColorTable(colorTable.size());

    for (unsigned long const index : indices) {
        sortedColorTable.getRedChannel().push_back(colorTable.getRedChannel()[index]);
        sortedColorTable.getGreenChannel().push_back(colorTable.getGreenChannel()[index]);
        sortedColorTable.getBlueChannel().push_back(colorTable.getBlueChannel()[index]);
    }

    return sortedColorTable;
}

// Reconstruir el índice de colores para la tabla ordenada
std::unordered_map<std::string, int> rebuildColorIndex(const ColorChannels& sortedColorTable) {
    std::unordered_map<std::string, int> colorIndex;
    colorIndex.reserve(COLOR_TABLE_RESERVE_SIZE);

    for (size_t i = 0; i < sortedColorTable.size(); ++i) {
        std::string const colorKey = std::to_string(sortedColorTable.getRedChannel()[i]) + "," +
                               std::to_string(sortedColorTable.getGreenChannel()[i]) + "," +
                               std::to_string(sortedColorTable.getBlueChannel()[i]);
        colorIndex[colorKey] = static_cast<int>(i);
    }

    return colorIndex;
}

// Crear tabla de colores y mapa de índices (SoA)
std::tuple<ColorChannels, std::unordered_map<std::string, int>>
createColorTableSoA(const ColorChannels& channels) {
    ColorChannels colorTable(channels.size());
    auto colorIndex = buildColorIndex(channels, colorTable);

    // Ordenar la tabla de colores
    auto indices = sortColorTable(colorTable);
    auto sortedColorTable = createSortedColorTable(colorTable, indices);

    // Reconstruir el índice de colores
    auto sortedColorIndex = rebuildColorIndex(sortedColorTable);

    return {sortedColorTable, sortedColorIndex};
}

// Añadir tabla de colores al archivo comprimido
void appendColorTableSoA(std::vector<uint8_t>& compressedData, const ColorChannels& colorTable, const PPMHeader& header) {
    colorTable.writeToBinary(compressedData, header);
}


// Añadir índices de píxeles al archivo comprimido
void appendPixelIndicesSoA(std::vector<uint8_t>& compressedData,
                           const ColorChannels& channels,
                           const std::unordered_map<std::string, int>& colorIndex) {
    for (size_t i = 0; i < channels.size(); ++i) {
        std::string const colorKey = std::to_string(channels.getRedChannel()[i]) + "," +
                                     std::to_string(channels.getGreenChannel()[i]) + "," +
                                     std::to_string(channels.getBlueChannel()[i]);

        int const index = colorIndex.at(colorKey);
        compressedData.push_back(static_cast<uint8_t>(index & BYTE_MASK));
        compressedData.push_back(static_cast<uint8_t>(index >> BITS_PER_BYTE));
    }
}


// Función principal de compresión SoA
void compressSoA(const FilePaths& paths){
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

    // Crear objeto ColorChannels para manejar los canales
    ColorChannels channels(static_cast<size_t>(header.width) * static_cast<size_t>(header.height));
    channels.extractFromBinary(fileData, header);

    // Crear la tabla de colores y el índice
    auto [colorTable, colorIndex] = createColorTableSoA(channels);
    std::string headerStr = generateHeader(header, static_cast<int>(colorTable.size()));

    // Crear datos comprimidos
    std::vector<uint8_t> compressedData;
    compressedData.insert(compressedData.end(), headerStr.begin(), headerStr.end());
    appendColorTableSoA(compressedData, colorTable, header);
    appendPixelIndicesSoA(compressedData, channels, colorIndex);

    // Escribir el archivo comprimido
    BinaryIO::writeBinaryFile(outputFile, compressedData);
}
//***********************************CUTFREQ************************************/

//Primero se lee la imagen y se almacenan los colores y sus frecuencias



/*void readImageAndStoreChannels(
    const std::string& inputFile, 
    ColorChannels& colorChannels, 
    std::unordered_map<uint32_t, int, HashColor>& colorFrequency) 
{
    PPMHeader header{};
    if (!readPPMHeader(inputFile, header)) {
      std::cerr << "Error al leer el encabezado del archivo PPM." << '\n';
      return;
    }
    std::cout << "Leyendo imagen y almacenando canales de color de: " << inputFile << '\n';

    // Abrir el archivo en modo binario
    std::ifstream file(inputFile, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo de entrada: " << inputFile << '\n';
        return;
    }

    // Leer el encabezado del archivo PPM
    std::string format;
    file >> format >> header.width >> header.height >> header.maxColorValue;
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignorar el salto de línea después del encabezado

    // Verificar formato y valor máximo de color
    if (format != "P6" || header.maxColorValue != 255) {
        std::cerr << "Error: Formato PPM no soportado o valor máximo de color inválido en " << inputFile << '\n';
        return;
    }

    // Calcular el número total de píxeles
    const size_t totalPixels = static_cast<size_t>(header.width) * static_cast<size_t>(header.height);

    // Reservar espacio en los canales de color
    colorChannels.reserve(totalPixels);

    // Leer todos los datos de píxeles en un solo bloque
    std::vector<uint8_t> buffer(totalPixels * 3); // Cada píxel tiene 3 componentes (RGB)
    file.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(buffer.size()));
    std::cout << "Extrayendo del binario\n";
    // Extraer los datos a los canales y calcular frecuencias
    colorChannels.extractFromBinaryWithFrequency(buffer, header, colorFrequency);

    file.close();
    std::cout << "Canales de color leídos y frecuencias de colores calculadas\n";
}

std::vector<std::pair<std::tuple<uint16_t, uint16_t, uint16_t>, int>> encontrar_colores_menos_frecuentes(
    const std::unordered_map<uint32_t, int, HashColor>& frecuencia, int n) {
    std::vector<std::pair<std::tuple<uint16_t, uint16_t, uint16_t>, int>> colores_frecuentes;

    for (const auto& [color, freq] : frecuencia) {
        uint16_t red = (color >> 16) & 0xFF;
        uint16_t green = (color >> 8) & 0xFF;
        uint16_t blue = color & 0xFF;
        colores_frecuentes.emplace_back(std::make_tuple(red, green, blue), freq);
    }

    // Ordenar por frecuencia
    std::sort(colores_frecuentes.begin(), colores_frecuentes.end(), [](const auto& a, const auto& b) {
        return a.second < b.second;
    });

    if (colores_frecuentes.size() > static_cast<size_t>(n)) {
        colores_frecuentes.resize(static_cast<size_t>(n));
    }

    return colores_frecuentes;
}

double calcularDistanciaEuclidiana(const std::tuple<uint16_t, uint16_t, uint16_t>& a,
                                   const std::tuple<uint16_t, uint16_t, uint16_t>& b) {
    double dr = std::get<0>(a) - std::get<0>(b);
    double dg = std::get<1>(a) - std::get<1>(b);
    double db = std::get<2>(a) - std::get<2>(b);
    return (dr * dr + dg * dg + db * db);
}

double calcularDistanciaCuadrada(const std::tuple<uint16_t, uint16_t, uint16_t>& color1,
                                 const std::tuple<uint16_t, uint16_t, uint16_t>& color2) {
    return std::pow(std::get<0>(color1) - std::get<0>(color2), 2) + 
           std::pow(std::get<1>(color1) - std::get<1>(color2), 2) + 
           std::pow(std::get<2>(color1) - std::get<2>(color2), 2);
}

struct KDNode {
    std::tuple<uint16_t, uint16_t, uint16_t> color;
    KDNode* left;
    KDNode* right;

    KDNode(const std::tuple<uint16_t, uint16_t, uint16_t>& color) 
        : color(color), left(nullptr), right(nullptr) {}
};

// Función para insertar un color en el K-D Tree
KDNode* insertarKDTree(KDNode* root, const std::tuple<uint16_t, uint16_t, uint16_t>& color, int depth = 0) {
    if (root == nullptr) {
        return new KDNode(color);
    }

    // Elegir el eje (0 -> R, 1 -> G, 2 -> B)
    int axis = depth % 3;
    if (axis == 0) {  // Compara el componente rojo (R)
        if (std::get<0>(color) < std::get<0>(root->color)) {
            root->left = insertarKDTree(root->left, color, depth + 1);
        } else {
            root->right = insertarKDTree(root->right, color, depth + 1);
        }
    } else if (axis == 1) {  // Compara el componente verde (G)
        if (std::get<1>(color) < std::get<1>(root->color)) {
            root->left = insertarKDTree(root->left, color, depth + 1);
        } else {
            root->right = insertarKDTree(root->right, color, depth + 1);
        }
    } else {  // Compara el componente azul (B)
        if (std::get<2>(color) < std::get<2>(root->color)) {
            root->left = insertarKDTree(root->left, color, depth + 1);
        } else {
            root->right = insertarKDTree(root->right, color, depth + 1);
        }
    }

    return root;
}

// Función para encontrar el vecino más cercano usando el K-D Tree
void buscarVecinoMasCercano(KDNode* root, const std::tuple<uint16_t, uint16_t, uint16_t>& color,
                             double& minDistancia, std::tuple<uint16_t, uint16_t, uint16_t>& mejorColor,
                             int depth = 0) {
    if (root == nullptr) return;

    // Calcular la distancia del color actual
    double distancia = calcularDistanciaEuclidiana(color, root->color);
    if (distancia < minDistancia) {
        minDistancia = distancia;
        mejorColor = root->color;
    }

    // Elegir el eje para la comparación
    int axis = depth % 3;

    // Primero, decidimos cuál subárbol es el más prometedor para buscar
    KDNode* primerSubArbol = nullptr;
    KDNode* segundoSubArbol = nullptr;

    if (axis == 0) {  // Compara el componente rojo (R)
        if (std::get<0>(color) < std::get<0>(root->color)) {
            primerSubArbol = root->left;
            segundoSubArbol = root->right;
        } else {
            primerSubArbol = root->right;
            segundoSubArbol = root->left;
        }
    } else if (axis == 1) {  // Compara el componente verde (G)
        if (std::get<1>(color) < std::get<1>(root->color)) {
            primerSubArbol = root->left;
            segundoSubArbol = root->right;
        } else {
            primerSubArbol = root->right;
            segundoSubArbol = root->left;
        }
    } else {  // Compara el componente azul (B)
        if (std::get<2>(color) < std::get<2>(root->color)) {
            primerSubArbol = root->left;
            segundoSubArbol = root->right;
        } else {
            primerSubArbol = root->right;
            segundoSubArbol = root->left;
        }
    }

    // Recursivamente buscar en el subárbol más prometedor primero
    buscarVecinoMasCercano(primerSubArbol, color, minDistancia, mejorColor, depth + 1);

    // Si la distancia mínima actual es mayor que la distancia al otro subárbol, podemos evitar buscar en el otro subárbol
    double distanciaAlEje = 0.0;
    if (axis == 0) {
        distanciaAlEje = std::abs(std::get<0>(color) - std::get<0>(root->color));
    } else if (axis == 1) {
        distanciaAlEje = std::abs(std::get<1>(color) - std::get<1>(root->color));
    } else {
        distanciaAlEje = std::abs(std::get<2>(color) - std::get<2>(root->color));
    }

    if (distanciaAlEje < minDistancia) {
        // Si la distancia al eje es menor que la distancia mínima, buscar también en el otro subárbol
        buscarVecinoMasCercano(segundoSubArbol, color, minDistancia, mejorColor, depth + 1);
    }
}


void buscarVecinoMasCercanoConPoda(KDNode* root, const std::tuple<uint16_t, uint16_t, uint16_t>& color,
                                    double& minDistancia, std::tuple<uint16_t, uint16_t, uint16_t>& mejorColor,
                                    int depth = 0) {
    if (root == nullptr) return;

    // Calcular la distancia al cuadrado entre el color y el nodo actual
    double distancia = calcularDistanciaCuadrada(color, root->color);
    if (distancia < minDistancia) {
        minDistancia = distancia;
        mejorColor = root->color;
    }

    int axis = depth % 3;  // Eje de comparación (rojo, verde, azul)

    // Variables para determinar qué subárbol explorar primero
    KDNode* primerSubArbol = nullptr;
    KDNode* segundoSubArbol = nullptr;

    // Elegir el subárbol a explorar primero basado en la comparación del color
    if (axis == 0) {  // Comparar componente rojo
        if (std::get<0>(color) < std::get<0>(root->color)) {
            primerSubArbol = root->left;
            segundoSubArbol = root->right;
        } else {
            primerSubArbol = root->right;
            segundoSubArbol = root->left;
        }
    } else if (axis == 1) {  // Comparar componente verde
        if (std::get<1>(color) < std::get<1>(root->color)) {
            primerSubArbol = root->left;
            segundoSubArbol = root->right;
        } else {
            primerSubArbol = root->right;
            segundoSubArbol = root->left;
        }
    } else {  // Comparar componente azul
        if (std::get<2>(color) < std::get<2>(root->color)) {
            primerSubArbol = root->left;
            segundoSubArbol = root->right;
        } else {
            primerSubArbol = root->right;
            segundoSubArbol = root->left;
        }
    }

    // Primero, explorar el primer subárbol
    buscarVecinoMasCercanoConPoda(primerSubArbol, color, minDistancia, mejorColor, depth + 1);

    // Verificar si debemos explorar el segundo subárbol utilizando poda
    double distanciaAlEje = 0.0;
    if (axis == 0) {
        distanciaAlEje = std::abs(std::get<0>(color) - std::get<0>(root->color));
    } else if (axis == 1) {
        distanciaAlEje = std::abs(std::get<1>(color) - std::get<1>(root->color));
    } else {
        distanciaAlEje = std::abs(std::get<2>(color) - std::get<2>(root->color));
    }

    // Si la diferencia al eje es menor que la distancia mínima encontrada, explorar el segundo subárbol
    if (distanciaAlEje < minDistancia) {
        buscarVecinoMasCercanoConPoda(segundoSubArbol, color, minDistancia, mejorColor, depth + 1);
    }
}

double calcularDistanciaEuclidiana(const std::tuple<uint16_t, uint16_t, uint16_t>& color1,
                                    const std::tuple<uint16_t, uint16_t, uint16_t>& color2) {
    uint16_t r1 = std::get<0>(color1);
    uint16_t g1 = std::get<1>(color1);
    uint16_t b1 = std::get<2>(color1);

    uint16_t r2 = std::get<0>(color2);
    uint16_t g2 = std::get<1>(color2);
    uint16_t b2 = std::get<2>(color2);

    return (std::pow(r1 - r2, 2) + std::pow(g1 - g2, 2) + std::pow(b1 - b2, 2));
}

// Función para encontrar el vecino más cercano con poda eficiente
void buscarVecinoMasCercanoOptimizado(KDNode* root, const std::tuple<uint16_t, uint16_t, uint16_t>& color,
                                       double& minDistancia, std::tuple<uint16_t, uint16_t, uint16_t>& mejorColor,
                                       int depth = 0) {
    if (root == nullptr) return;

    // Calcular la distancia al cuadrado entre el color y el nodo actual
    double distancia = calcularDistanciaCuadrada(color, root->color);
    if (distancia < minDistancia) {
        minDistancia = distancia;
        mejorColor = root->color;
    }

    int axis = depth % 3;  // Eje de comparación (rojo, verde, azul)

    // Variables para determinar qué subárbol explorar primero
    KDNode* primerSubArbol = nullptr;
    KDNode* segundoSubArbol = nullptr;

    // Elegir el subárbol a explorar primero basado en la comparación del color
    if (axis == 0) {  // Comparar componente rojo
        if (std::get<0>(color) < std::get<0>(root->color)) {
            primerSubArbol = root->left;
            segundoSubArbol = root->right;
        } else {
            primerSubArbol = root->right;
            segundoSubArbol = root->left;
        }
    } else if (axis == 1) {  // Comparar componente verde
        if (std::get<1>(color) < std::get<1>(root->color)) {
            primerSubArbol = root->left;
            segundoSubArbol = root->right;
        } else {
            primerSubArbol = root->right;
            segundoSubArbol = root->left;
        }
    } else {  // Comparar componente azul
        if (std::get<2>(color) < std::get<2>(root->color)) {
            primerSubArbol = root->left;
            segundoSubArbol = root->right;
        } else {
            primerSubArbol = root->right;
            segundoSubArbol = root->left;
        }
    }

    // Primero, explorar el primer subárbol
    buscarVecinoMasCercanoOptimizado(primerSubArbol, color, minDistancia, mejorColor, depth + 1);

    // Verificar si debemos explorar el segundo subárbol utilizando poda eficiente
    double distanciaAlEje = 0.0;
    if (axis == 0) {
        distanciaAlEje = std::abs(std::get<0>(color) - std::get<0>(root->color));
    } else if (axis == 1) {
        distanciaAlEje = std::abs(std::get<1>(color) - std::get<1>(root->color));
    } else {
        distanciaAlEje = std::abs(std::get<2>(color) - std::get<2>(root->color));
    }

    // Poda temprana: si la distancia al eje es mayor que la distancia mínima encontrada, no exploramos el segundo subárbol
    if (distanciaAlEje < minDistancia) {
        buscarVecinoMasCercanoOptimizado(segundoSubArbol, color, minDistancia, mejorColor, depth + 1);
    }
}



void sustituirColoresMenosFrecuentes(ColorChannels& colorChannels,
                                      const std::vector<std::tuple<uint16_t, uint16_t, uint16_t>>& menosFrecuentes,
                                      const std::vector<std::tuple<uint16_t, uint16_t, uint16_t>>& coloresRestantes) {
    // Crear un mapa de sustituciones previamente
    std::unordered_map<std::tuple<uint16_t, uint16_t, uint16_t>, std::tuple<uint16_t, uint16_t, uint16_t>> mapaSustituciones;

    std::cout << "Encontrando colores más cercanos para sustituir\n";
    // Para cada color menos frecuente, encuentra el color más cercano
    std::cout <<"Creando KDTree\n";
    KDNode* kdTreeRoot = nullptr;
for (const auto& colorRestante : coloresRestantes) {
    kdTreeRoot = construirKDTree(coloresRestantes);
}

// Buscar el vecino más cercano para cada color menos frecuente
std::cout << "Buscando vecinos más cercanos\n";
for (const auto& colorMenosFrecuente : menosFrecuentes) {
    double minDistancia = std::numeric_limits<double>::infinity();
    std::tuple<uint16_t, uint16_t, uint16_t> colorMasFrecuenteCercano;
    
    buscarVecinoMasCercanoOptimizado(kdTreeRoot, colorMenosFrecuente, minDistancia, colorMasFrecuenteCercano);
    
    // Guardar la sustitución en el mapa
    mapaSustituciones[colorMenosFrecuente] = colorMasFrecuenteCercano;
}

    std::cout << "Sustituyendo\n";
    // Ahora, sustituir todos los píxeles de la imagen
    const size_t totalPixels = colorChannels.size();  // Total de píxeles en la imagen
    for (size_t i = 0; i < totalPixels; ++i) {
        uint16_t r = colorChannels.getRedChannel()[i];
        uint16_t g = colorChannels.getGreenChannel()[i];
        uint16_t b = colorChannels.getBlueChannel()[i];

        // Crear una tupla con el color actual
        std::tuple<uint16_t, uint16_t, uint16_t> colorActual = std::make_tuple(r, g, b);

        // Si el color actual es uno de los menos frecuentes, lo sustituimos
        if (mapaSustituciones.find(colorActual) != mapaSustituciones.end()) {
            auto colorSustituido = mapaSustituciones[colorActual];
            colorChannels.getRedChannel()[i] = std::get<0>(colorSustituido);
            colorChannels.getGreenChannel()[i] = std::get<1>(colorSustituido);
            colorChannels.getBlueChannel()[i] = std::get<2>(colorSustituido);
        }
    }
}
void writePPM(const std::string& outputFile, const PPMHeader& header, const ColorChannels& colorChannels) {
    std::ofstream outFile(outputFile);
    if (!outFile.is_open()) {
        std::cerr << "Error al abrir el archivo de salida: " << outputFile << '\n';
        return;
    }

    // Escribir el encabezado
    outFile << "P3\n";  // Formato de texto
    outFile << "# Imagen procesada\n";
    outFile << header.width << " " << header.height << "\n";
    outFile << "255\n";  // Valor máximo para los colores

    // Escribir los píxeles
    const auto& reds = colorChannels.getRedChannel();
    const auto& greens = colorChannels.getGreenChannel();
    const auto& blues = colorChannels.getBlueChannel();

    for (size_t i = 0; i < reds.size(); ++i) {
        outFile << reds[i] << " " << greens[i] << " " << blues[i];
        if (i != reds.size() - 1) {
            outFile << " ";  // Separar cada píxel con un espacio
        }
        // Agregar salto de línea cada 5 píxeles por estética
        if ((i + 1) % 5 == 0) {
            outFile << "\n";
        }
    }

    std::cout << "Imagen procesada escrita en: " << outputFile << '\n';
}


// Definir constantes para los desplazamientos y la máscara
constexpr int SHIFT_RED = 16;
constexpr int SHIFT_GREEN = 8;
constexpr int MASK = 0xFF;


std::unordered_map<std::tuple<uint16_t, uint16_t, uint16_t>, std::tuple<uint16_t, uint16_t, uint16_t>, HashTuple> encontrarColoresReemplazo(
    const std::unordered_set<std::tuple<uint16_t, uint16_t, uint16_t>, HashTuple>& colorsToRemoveSet,
    const ColorChannels& colorChannels) {
    
    std::unordered_map<std::tuple<uint16_t, uint16_t, uint16_t>, std::tuple<uint16_t, uint16_t, uint16_t>, HashTuple> replacementMap;

    // Filtramos los colores candidatos para reemplazo (solo aquellos que no están en la lista de eliminados).
    //la lista de candidatos es candidateColors
    //colorsToRemoveSet tiene que ser la lista de n colores menos frecuentes
    std::vector<std::tuple<uint16_t, uint16_t, uint16_t>> candidateColors;
    for (size_t i = 0; i < colorChannels.size(); ++i) {
        auto candidateColor = std::make_tuple(colorChannels.getRedChannel()[i], colorChannels.getGreenChannel()[i], colorChannels.getBlueChannel()[i]);
        if (colorsToRemoveSet.find(candidateColor) == colorsToRemoveSet.end()) {
            candidateColors.push_back(candidateColor); // Guardamos solo los colores que no han sido eliminados.
        }
    }

    // Buscamos el color más cercano de los candidatos para cada color eliminado.
    for (const auto& colorToRemove : colorsToRemoveSet) {
        std::tuple<uint16_t, uint16_t, uint16_t> closestColor;
        double minDistance = std::numeric_limits<double>::max(); // Inicializamos la distancia más corta como un valor muy alto.

        // Recorremos los candidatos y calculamos la distancia euclidiana en el espacio RGB.
        for (const auto& candidateColor : candidateColors) {
            const int redDiff = static_cast<int>(std::get<0>(colorToRemove)) - static_cast<int>(std::get<0>(candidateColor));
            const int greenDiff = static_cast<int>(std::get<1>(colorToRemove)) - static_cast<int>(std::get<1>(candidateColor));
            const int blueDiff = static_cast<int>(std::get<2>(colorToRemove)) - static_cast<int>(std::get<2>(candidateColor));
            const double distance = (redDiff * redDiff) + (greenDiff * greenDiff) + (blueDiff * blueDiff);

            // Si encontramos una distancia menor, actualizamos el color más cercano.
            if (distance < minDistance) {
                minDistance = distance;
                closestColor = candidateColor;
            }
        }

        // Guardamos en el mapa de reemplazo el color que debe sustituir al eliminado.
        replacementMap[colorToRemove] = closestColor;
    }

    return replacementMap; // Devolvemos el mapa de reemplazo de colores.
}


std::unordered_set<std::tuple<uint16_t, uint16_t, uint16_t>, HashTuple> encontrar_colores_menos_frecuentes_2(
    const std::unordered_map<uint32_t, int, HashColor>& frecuencia, int n) {
    
    // Vector para almacenar colores con sus frecuencias y ordenar
    std::vector<std::pair<std::tuple<uint16_t, uint16_t, uint16_t>, int>> colores_frecuentes;

    // Convertimos cada color en una tupla RGB y lo almacenamos junto con su frecuencia
    for (const auto& [color, freq] : frecuencia) {
        uint16_t red = (color >> 16) & 0xFF;
        uint16_t green = (color >> 8) & 0xFF;
        uint16_t blue = color & 0xFF;
        colores_frecuentes.emplace_back(std::make_tuple(red, green, blue), freq);
    }

    // Ordenar el vector por frecuencia en orden ascendente
    std::sort(colores_frecuentes.begin(), colores_frecuentes.end(), [](const auto& a, const auto& b) {
        return a.second < b.second;
    });

    // Crear un conjunto para los colores menos frecuentes
    std::unordered_set<std::tuple<uint16_t, uint16_t, uint16_t>, HashTuple> colores_menos_frecuentes;

    // Agregar los primeros n colores menos frecuentes al conjunto
    for (size_t i = 0; i < std::min(static_cast<size_t>(n), colores_frecuentes.size()); ++i) {
        colores_menos_frecuentes.insert(colores_frecuentes[i].first);
    }

    return colores_menos_frecuentes;
}

void sustituirColoresEnImagen(
    ColorChannels& colorChannels,
    const std::unordered_map<std::tuple<uint16_t, uint16_t, uint16_t>, std::tuple<uint16_t, uint16_t, uint16_t>, HashTuple>& replacementMap) {
    
    // Recorremos todos los píxeles de la imagen
    for (size_t i = 0; i < colorChannels.size(); ++i) {
        // Obtenemos el color actual del píxel
        auto currentColor = std::make_tuple(
            colorChannels.getRedChannel()[i],
            colorChannels.getGreenChannel()[i],
            colorChannels.getBlueChannel()[i]
        );

        // Verificamos si el color actual necesita ser reemplazado
        auto it = replacementMap.find(currentColor);
        if (it != replacementMap.end()) {
            // Si el color actual está en el mapa de reemplazo, lo sustituimos por el color más cercano
            const auto& replacementColor = it->second;
            colorChannels.getRedChannel()[i] = std::get<0>(replacementColor);
            colorChannels.getGreenChannel()[i] = std::get<1>(replacementColor);
            colorChannels.getBlueChannel()[i] = std::get<2>(replacementColor);
        }
    }
}

void processCutfreq(const std::string& inputFile, int numColors, const std::string& outputFile) {
    // Leer la imagen y procesar píxeles
    std::cout << "Leyendo imagen y almacenando canales de color" << '\n';
    PPMHeader header{};
    if (!readPPMHeader(inputFile, header)) {
      std::cerr << "Error al leer el encabezado del archivo PPM." << '\n';
      return;
    }
    
    const size_t totalPixels = static_cast<size_t>(header.width) * static_cast<size_t>(header.height);
    std::cout << "Hay " << totalPixels << " píxeles en la imagen\n";
    ColorChannels colorChannels(totalPixels);
    std::unordered_map<uint32_t, int, HashColor> colorFrequency;
    std::cout << "Calculando frecuencias de colores\n";
    readImageAndStoreChannels(inputFile, colorChannels, colorFrequency);

    std::cout << "Encontrando n menos frecuentes\n";
    // Ordenar las frecuencias y encontrar los n colores menos frecuentes
    auto coloresMenosFrecuentes = encontrar_colores_menos_frecuentes_2(colorFrequency, numColors);
    std::cout << "Encontrados " << coloresMenosFrecuentes.size() << " colores menos frecuentes\n";

    // Crear el mapa de reemplazo
    std::cout << "Encontrando colores de reemplazo\n";
    auto replacementMap = encontrarColoresReemplazo(coloresMenosFrecuentes, colorChannels);

    std::cout << "Sustituyendo colores en la imagen\n";
    // Llamamos a la función para sustituir los colores menos frecuentes por sus reemplazos
    sustituirColoresEnImagen(colorChannels, replacementMap);

    // Guardar la imagen procesada
    std::cout << "Ahora los escribo en " << outputFile << '\n';
    writePPM(outputFile, header, colorChannels);
    std::cout << "Archivo procesado guardado en: " << outputFile << '\n';
}*/



//************PRUEBAS CON ÁBOLES*************/

void readImageAndStoreChannels(
    const std::string& inputFile, 
    ColorChannels& colorChannels, 
    std::unordered_map<uint32_t, int, HashColor>& colorFrequency) 
{
    PPMHeader header{};
    if (!readPPMHeader(inputFile, header)) {
      std::cerr << "Error al leer el encabezado del archivo PPM." << '\n';
      return;
    }
    std::cout << "Leyendo imagen y almacenando canales de color de: " << inputFile << '\n';

    // Abrir el archivo en modo binario
    std::ifstream file(inputFile, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo de entrada: " << inputFile << '\n';
        return;
    }

    // Leer el encabezado del archivo PPM
    std::string format;
    file >> format >> header.width >> header.height >> header.maxColorValue;
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignorar el salto de línea después del encabezado

    // Verificar formato y valor máximo de color
    if (format != "P6" || header.maxColorValue != 255) {
        std::cerr << "Error: Formato PPM no soportado o valor máximo de color inválido en " << inputFile << '\n';
        return;
    }

    // Calcular el número total de píxeles
    const size_t totalPixels = static_cast<size_t>(header.width) * static_cast<size_t>(header.height);

    // Reservar espacio en los canales de color
    colorChannels.reserve(totalPixels);

    // Leer todos los datos de píxeles en un solo bloque
    std::vector<uint8_t> buffer(totalPixels * 3); // Cada píxel tiene 3 componentes (RGB)
    file.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(buffer.size()));
    std::cout << "Extrayendo del binario\n";
    // Extraer los datos a los canales y calcular frecuencias
    colorChannels.extractFromBinaryWithFrequency(buffer, header, colorFrequency);

    file.close();
    std::cout << "Canales de color leídos y frecuencias de colores calculadas\n";
}

std::unordered_set<std::tuple<uint16_t, uint16_t, uint16_t>, HashTuple> encontrar_colores_menos_frecuentes_2(
    const std::unordered_map<uint32_t, int, HashColor>& frecuencia, int n) {
    
    // Vector para almacenar colores con sus frecuencias y ordenar
    std::vector<std::pair<std::tuple<uint16_t, uint16_t, uint16_t>, int>> colores_frecuentes;

    // Convertimos cada color en una tupla RGB y lo almacenamos junto con su frecuencia
    for (const auto& [color, freq] : frecuencia) {
        uint16_t red = (color >> 16) & 0xFF;
        uint16_t green = (color >> 8) & 0xFF;
        uint16_t blue = color & 0xFF;
        colores_frecuentes.emplace_back(std::make_tuple(red, green, blue), freq);
    }

    // Ordenar el vector por frecuencia en orden ascendente
    std::sort(colores_frecuentes.begin(), colores_frecuentes.end(), [](const auto& a, const auto& b) {
        return a.second < b.second;
    });

    // Crear un conjunto para los colores menos frecuentes
    std::unordered_set<std::tuple<uint16_t, uint16_t, uint16_t>, HashTuple> colores_menos_frecuentes;

    // Agregar los primeros n colores menos frecuentes al conjunto
    for (size_t i = 0; i < std::min(static_cast<size_t>(n), colores_frecuentes.size()); ++i) {
        colores_menos_frecuentes.insert(colores_frecuentes[i].first);
    }

    return colores_menos_frecuentes;
}


void sustituirColoresEnImagen(
    ColorChannels& colorChannels,
    const std::unordered_map<std::tuple<uint16_t, uint16_t, uint16_t>, std::tuple<uint16_t, uint16_t, uint16_t>, HashTuple>& replacementMap) {
    
    // Recorremos todos los píxeles de la imagen
    for (size_t i = 0; i < colorChannels.size(); ++i) {
        // Obtenemos el color actual del píxel
        auto currentColor = std::make_tuple(
            colorChannels.getRedChannel()[i],
            colorChannels.getGreenChannel()[i],
            colorChannels.getBlueChannel()[i]
        );

        // Verificamos si el color actual necesita ser reemplazado
        auto it = replacementMap.find(currentColor);
        if (it != replacementMap.end()) {
            // Si el color actual está en el mapa de reemplazo, lo sustituimos por el color más cercano
            const auto& replacementColor = it->second;
            colorChannels.getRedChannel()[i] = std::get<0>(replacementColor);
            colorChannels.getGreenChannel()[i] = std::get<1>(replacementColor);
            colorChannels.getBlueChannel()[i] = std::get<2>(replacementColor);
        }
    }
}


struct KDNode {
    std::tuple<uint16_t, uint16_t, uint16_t> color;
    KDNode* left;
    KDNode* right;

    KDNode(const std::tuple<uint16_t, uint16_t, uint16_t>& color)
        : color(color), left(nullptr), right(nullptr) {}
};

// Construcción balanceada del árbol KD
KDNode* construirKDTree(std::vector<std::tuple<uint16_t, uint16_t, uint16_t>>& colors, int depth = 0) {
    if (colors.empty()) return nullptr;

    int axis = depth % 3;
    std::sort(colors.begin(), colors.end(), [axis](const auto& a, const auto& b) {
        if (axis == 0) return std::get<0>(a) < std::get<0>(b);
        if (axis == 1) return std::get<1>(a) < std::get<1>(b);
        return std::get<2>(a) < std::get<2>(b);
    });

    size_t medianIndex = colors.size() / 2;
    KDNode* root = new KDNode(colors[medianIndex]);

    std::vector<std::tuple<uint16_t, uint16_t, uint16_t>> leftColors(colors.begin(), colors.begin() + static_cast<std::vector<std::tuple<uint16_t, uint16_t, uint16_t>>::difference_type>(medianIndex));
    std::vector<std::tuple<uint16_t, uint16_t, uint16_t>> rightColors(colors.begin() + static_cast<std::vector<std::tuple<uint16_t, uint16_t, uint16_t>>::difference_type>(medianIndex) + 1, colors.end());

    root->left = construirKDTree(leftColors, depth + 1);
    root->right = construirKDTree(rightColors, depth + 1);

    return root;
}

// Calcular la distancia al cuadrado entre dos colores
double calcularDistanciaCuadrada(const std::tuple<uint16_t, uint16_t, uint16_t>& color1,
                                 const std::tuple<uint16_t, uint16_t, uint16_t>& color2) {
    return std::pow(std::get<0>(color1) - std::get<0>(color2), 2) + 
           std::pow(std::get<1>(color1) - std::get<1>(color2), 2) + 
           std::pow(std::get<2>(color1) - std::get<2>(color2), 2);
}

// Búsqueda del vecino más cercano
void buscarVecinoMasCercanoOptimizado(KDNode* root, const std::tuple<uint16_t, uint16_t, uint16_t>& color,
                                      double& minDistanciaCuadrada, std::tuple<uint16_t, uint16_t, uint16_t>& mejorColor,
                                      int depth = 0) {
    if (root == nullptr) return;

    double distancia = calcularDistanciaCuadrada(color, root->color);
    if (distancia < minDistanciaCuadrada) {
        minDistanciaCuadrada = distancia;
        mejorColor = root->color;
    }

    int axis = depth % 3;
    KDNode* primerSubArbol = nullptr;
    KDNode* segundoSubArbol = nullptr;

    if ((axis == 0 && std::get<0>(color) < std::get<0>(root->color)) ||
        (axis == 1 && std::get<1>(color) < std::get<1>(root->color)) ||
        (axis == 2 && std::get<2>(color) < std::get<2>(root->color))) {
        primerSubArbol = root->left;
        segundoSubArbol = root->right;
    } else {
        primerSubArbol = root->right;
        segundoSubArbol = root->left;
    }

    buscarVecinoMasCercanoOptimizado(primerSubArbol, color, minDistanciaCuadrada, mejorColor, depth + 1);

    double distanciaAlEje = 0.0;
    if (axis == 0) {
        distanciaAlEje = std::pow(std::get<0>(color) - std::get<0>(root->color), 2);
    } else if (axis == 1) {
        distanciaAlEje = std::pow(std::get<1>(color) - std::get<1>(root->color), 2);
    } else {
        distanciaAlEje = std::pow(std::get<2>(color) - std::get<2>(root->color), 2);
    }

    if (distanciaAlEje < minDistanciaCuadrada) {
        buscarVecinoMasCercanoOptimizado(segundoSubArbol, color, minDistanciaCuadrada, mejorColor, depth + 1);
    }
}

void writePPM(const std::string& outputFile, const PPMHeader& header, const ColorChannels& colorChannels) {
    std::ofstream outFile(outputFile);
    if (!outFile.is_open()) {
        std::cerr << "Error al abrir el archivo de salida: " << outputFile << '\n';
        return;
    }

    // Escribir el encabezado
    outFile << "P3\n";  // Formato de texto
    outFile << "# Imagen procesada\n";
    outFile << header.width << " " << header.height << "\n";
    outFile << "255\n";  // Valor máximo para los colores

    // Escribir los píxeles
    const auto& reds = colorChannels.getRedChannel();
    const auto& greens = colorChannels.getGreenChannel();
    const auto& blues = colorChannels.getBlueChannel();

    for (size_t i = 0; i < reds.size(); ++i) {
        outFile << reds[i] << " " << greens[i] << " " << blues[i];
        if (i != reds.size() - 1) {
            outFile << " ";  // Separar cada píxel con un espacio
        }
        // Agregar salto de línea cada 5 píxeles por estética
        if ((i + 1) % 5 == 0) {
            outFile << "\n";
        }
    }

    std::cout << "Imagen procesada escrita en: " << outputFile << '\n';
}


// Definir constantes para los desplazamientos y la máscara
constexpr int SHIFT_RED = 16;
constexpr int SHIFT_GREEN = 8;
constexpr int MASK = 0xFF;


std::unordered_map<std::tuple<uint16_t, uint16_t, uint16_t>, std::tuple<uint16_t, uint16_t, uint16_t>, HashTuple> encontrarColoresReemplazo(
    const std::unordered_set<std::tuple<uint16_t, uint16_t, uint16_t>, HashTuple>& colorsToRemoveSet,
    const ColorChannels& colorChannels) {
    
    std::unordered_map<std::tuple<uint16_t, uint16_t, uint16_t>, std::tuple<uint16_t, uint16_t, uint16_t>, HashTuple> replacementMap;

    // Filtramos los colores candidatos para reemplazo (solo aquellos que no están en la lista de eliminados).
    //la lista de candidatos es candidateColors
    //colorsToRemoveSet tiene que ser la lista de n colores menos frecuentes
    std::vector<std::tuple<uint16_t, uint16_t, uint16_t>> candidateColors;
    for (size_t i = 0; i < colorChannels.size(); ++i) {
        auto candidateColor = std::make_tuple(colorChannels.getRedChannel()[i], colorChannels.getGreenChannel()[i], colorChannels.getBlueChannel()[i]);
        if (colorsToRemoveSet.find(candidateColor) == colorsToRemoveSet.end()) {
            candidateColors.push_back(candidateColor); // Guardamos solo los colores que no han sido eliminados.
        }
    }

    // Buscamos el color más cercano de los candidatos para cada color eliminado.
    for (const auto& colorToRemove : colorsToRemoveSet) {
        std::tuple<uint16_t, uint16_t, uint16_t> closestColor;
        double minDistance = std::numeric_limits<double>::max(); // Inicializamos la distancia más corta como un valor muy alto.

        // Recorremos los candidatos y calculamos la distancia euclidiana en el espacio RGB.
        for (const auto& candidateColor : candidateColors) {
            const int redDiff = static_cast<int>(std::get<0>(colorToRemove)) - static_cast<int>(std::get<0>(candidateColor));
            const int greenDiff = static_cast<int>(std::get<1>(colorToRemove)) - static_cast<int>(std::get<1>(candidateColor));
            const int blueDiff = static_cast<int>(std::get<2>(colorToRemove)) - static_cast<int>(std::get<2>(candidateColor));
            const double distance = (redDiff * redDiff) + (greenDiff * greenDiff) + (blueDiff * blueDiff);

            // Si encontramos una distancia menor, actualizamos el color más cercano.
            if (distance < minDistance) {
                minDistance = distance;
                closestColor = candidateColor;
            }
        }

        // Guardamos en el mapa de reemplazo el color que debe sustituir al eliminado.
        replacementMap[colorToRemove] = closestColor;
    }

    return replacementMap; // Devolvemos el mapa de reemplazo de colores.
}

// Implementación de processCutfreq
void processCutfreq(const std::string& inputFile, int numColors, const std::string& outputFile) {
    std::cout << "Leyendo imagen y almacenando canales de color\n";
    PPMHeader header{};
    if (!readPPMHeader(inputFile, header)) {
        std::cerr << "Error al leer el encabezado del archivo PPM.\n";
        return;
    }

    const size_t totalPixels = static_cast<size_t>(header.width) * static_cast<size_t>(header.height);
    std::cout << "Hay " << totalPixels << " píxeles en la imagen\n";
    ColorChannels colorChannels(totalPixels);
    std::unordered_map<uint32_t, int, HashColor> colorFrequency;
    std::cout << "Calculando frecuencias de colores\n";
    readImageAndStoreChannels(inputFile, colorChannels, colorFrequency);

    std::cout << "Encontrando colores menos frecuentes\n";
    auto coloresMenosFrecuentes = encontrar_colores_menos_frecuentes_2(colorFrequency, numColors);
    std::cout << "Encontrados " << coloresMenosFrecuentes.size() << " colores menos frecuentes\n";

    std::vector<std::tuple<uint16_t, uint16_t, uint16_t>> coloresRestantes;
    for (const auto& [color, _] : colorFrequency) {
        uint16_t red = (color >> 16) & 0xFF;
        uint16_t green = (color >> 8) & 0xFF;
        uint16_t blue = color & 0xFF;
        auto colorTupla = std::make_tuple(red, green, blue);

        if (coloresMenosFrecuentes.find(colorTupla) == coloresMenosFrecuentes.end()) {
            coloresRestantes.push_back(colorTupla);
        }
    }

    std::cout << "Construyendo KDTree para colores restantes\n";
    KDNode* kdTreeRoot = construirKDTree(coloresRestantes);
    std::cout << "KDTree construido\n";

    std::cout << "Navegando el árbol KD\n";
    std::unordered_map<std::tuple<uint16_t, uint16_t, uint16_t>, std::tuple<uint16_t, uint16_t, uint16_t>, HashTuple> replacementMap;
    for (const auto& colorMenosFrecuente : coloresMenosFrecuentes) {
        double minDistancia = std::numeric_limits<double>::infinity();
        std::tuple<uint16_t, uint16_t, uint16_t> colorMasFrecuenteCercano;

        buscarVecinoMasCercanoOptimizado(kdTreeRoot, colorMenosFrecuente, minDistancia, colorMasFrecuenteCercano);
        replacementMap[colorMenosFrecuente] = colorMasFrecuenteCercano;
    }
    std::cout << "Colores de reemplazo encontrados\n";
    
    std::cout << "Sustituyendo colores\n";
    sustituirColoresEnImagen(colorChannels, replacementMap);
    std::cout << "Escribiendo la imagen procesada en " << outputFile << '\n';
    writePPM(outputFile, header, colorChannels);
    std::cout << "Archivo procesado guardado en: " << outputFile << '\n';
}