//
// Created by liang on 4/10/24.
//

#include "imagesoa.hpp"
#include "color.hpp"
#include "color.cpp"
#include <iostream>
#include <fstream>
#include <cmath>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <numeric>
#include <string>
#include <unordered_set>
#include <memory>
#include <tuple>
#include <limits>

void processMaxLevel(const std::string& inputFile, int maxLevel) {
    // Lógica para el comando 'maxlevel'
    std::cout << "Processing 'maxlevel' for file: " << inputFile << " with max level: " << maxLevel << '\n';
    // Aquí iría la lógica para modificar el nivel máximo del archivo
}


// INTENTO DE CUTFREQ CON SOA

// Lee una imagen y almacena los colores y sus frecuencias en una estructura SOA

constexpr int MAX_COLOR_VALUE = 255;
constexpr size_t BUFFER_SIZE = 4096; // Puedes ajustar este valor según sea necesario

// Función de distancia cuadrada para evitar la raíz cuadrada
double distanciaCuadrada(const Color& colora, const Color& colorb) {
    int dr = colora.red - colorb.red;
    int dg = colora.green - colorb.green;
    int db = colora.blue - colorb.blue;
    return dr * dr + dg * dg + db * db;
}

std::tuple<std::vector<Color>, ColorPalette, std::vector<int>> readImageAndStoreColors(const std::string& inputFile) {
    std::cout << "Leyendo imagen y almacenando colores de: " << inputFile << '\n';

    // Abrir archivo en modo binario
    std::ifstream file(inputFile, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo de entrada: " << inputFile << '\n';
        return {{}, {}, {}};
    }

    std::string format;
    int width = 0, height = 0, maxColorValue = 0;
    file >> format >> width >> height >> maxColorValue;
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (format != "P6" || maxColorValue != 255) {
        std::cerr << "Error: Formato PPM no soportado o valor máximo de color inválido en " << inputFile << '\n';
        return {{}, {}, {}};
    }

    const size_t totalPixels = static_cast<size_t>(width) * static_cast<size_t>(height);
    std::vector<Color> pixelList;
    pixelList.reserve(totalPixels);

    // Usar un array en lugar de unordered_map para el conteo de frecuencias
    std::vector<int> directColorFrequency(256 * 256 * 256, 0);

    // Leer en bloques usando un buffer más pequeño
    std::vector<uint8_t> buffer(BUFFER_SIZE);
    size_t pixelsRead = 0;
    size_t colorIndex;

    while (pixelsRead < totalPixels) {
        size_t pixelsToRead = std::min(BUFFER_SIZE / 3, totalPixels - pixelsRead);
        file.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(pixelsToRead * 3));

        for (size_t i = 0; i < pixelsToRead; ++i) {
            uint8_t red = buffer[i * 3];
            uint8_t green = buffer[i * 3 + 1];
            uint8_t blue = buffer[i * 3 + 2];

            // Calcular índice directo para el color
            colorIndex = (static_cast<size_t>(red) << 16) | 
                         (static_cast<size_t>(green) << 8) | 
                         blue;

            pixelList.push_back({red, green, blue});
            directColorFrequency[colorIndex]++;
        }
        pixelsRead += pixelsToRead;
    }

    file.close();

    // Construir palette y vector de frecuencias solo para colores que existen
    ColorPalette colorPalette;
    std::vector<int> colorFrequencies;

    for (size_t i = 0; i < directColorFrequency.size(); ++i) {
        if (directColorFrequency[i] > 0) {
            uint8_t red = (i >> 16) & 0xFF;
            uint8_t green = (i >> 8) & 0xFF;
            uint8_t blue = i & 0xFF;
            colorPalette.addColor(red, green, blue);
            colorFrequencies.push_back(directColorFrequency[i]);
        }
    }

    return {pixelList, colorPalette, colorFrequencies};
}

std::vector<Color> encontrar_colores_menos_frecuentes(const std::unordered_map<Color, int>& frecuencia, int n) {
    // Convertir el unordered_map en un vector de pares (color, frecuencia)
    std::vector<std::pair<Color, int>> colores_frecuentes(frecuencia.begin(), frecuencia.end());

    // Ordenar por frecuencia ascendente
    std::sort(colores_frecuentes.begin(), colores_frecuentes.end(), 
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


void escribirPPM(const std::string& filename, const std::vector<Color>& pixels, int width, int height) {
    std::ofstream outputFile(filename, std::ios::binary);
    if (!outputFile) {
        std::cout << "Error: No se pudo abrir el archivo para escribir: " << filename << '\n';
        throw std::runtime_error("No se pudo abrir el archivo para escribir");
    }

    outputFile << "P6\n" << width << " " << height << "\n255\n";
    for (const auto& pixel : pixels) {
        outputFile.put(static_cast<char>(pixel.red));
        outputFile.put(static_cast<char>(pixel.green));
        outputFile.put(static_cast<char>(pixel.blue));
    }
    std::cout << "Archivo PPM escrito exitosamente: " << filename << '\n';
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
    size_t colorIndex; // Add this line
    std::unique_ptr<KDNode> left;
    std::unique_ptr<KDNode> right;

    KDNode() : color(0, 0, 0), left(nullptr), right(nullptr) {} // Constructor por defecto
    KDNode(const Color& c) : color(c), left(nullptr), right(nullptr) {}
};

// Función auxiliar para construir el K-D Tree en base a ColorPalette y una lista de índices
std::unique_ptr<KDNode> construirKDTreeRecursivo(ColorPalette& palette, std::vector<size_t>& indices, int depth) {
    if (indices.empty()) return nullptr;

    size_t axis = static_cast<size_t>(depth) % 3;
    size_t medianIndex = indices.size() / 2;

    std::nth_element(indices.begin(), indices.begin() + static_cast<std::vector<size_t>::difference_type>(medianIndex), indices.end(), [&palette, axis](size_t lhs, size_t rhs) {
        auto getAxisValue = [axis](const std::tuple<uint8_t, uint8_t, uint8_t>& color) -> uint8_t {
            switch (axis) {
                case 0: return std::get<0>(color);
                case 1: return std::get<1>(color);
                case 2: return std::get<2>(color);
                default: return std::get<0>(color); // Default case to avoid compiler warnings
            }
        };
        return getAxisValue(palette.getColor(lhs)) < getAxisValue(palette.getColor(rhs));
    });

    auto node = std::make_unique<KDNode>();
    node->colorIndex = indices[medianIndex];

    std::vector<size_t> leftIndices(indices.begin(), indices.begin() + static_cast<std::vector<size_t>::difference_type>(medianIndex));
    std::vector<size_t> rightIndices(indices.begin() + static_cast<std::vector<size_t>::difference_type>(medianIndex) + 1, indices.end());

    node->left = construirKDTreeRecursivo(palette, leftIndices, depth + 1);
    node->right = construirKDTreeRecursivo(palette, rightIndices, depth + 1);

    return node;
}

std::unique_ptr<KDNode> construirKDTree(ColorPalette& palette) {
    std::vector<size_t> indices(palette.size());
    std::iota(indices.begin(), indices.end(), 0);
    // Ya que tenemos los índices, deberíamos hacer que se ordenen en base a la dimensión
    // que seleccionemos en cada paso de la recursión, para maximizar la partición
    return construirKDTreeRecursivo(palette, indices, 0);
}

// Búsqueda del vecino más cercano en el K-D Tree
std::pair<Color, double> buscarVecinoMasCercano(const std::unique_ptr<KDNode>& node, const Color& colorObjetivo, int depth, const ColorPalette& palette) {
    if (!node) return {{0, 0, 0}, std::numeric_limits<double>::max()};  // Nodo nulo

    // Calculamos el eje de partición
    size_t axis = static_cast<size_t>(depth) % 3;

    // Pre-cargamos el color directamente desde la paleta
    auto colorTuple = palette.getColor(node->colorIndex);
    Color currentColor{std::get<0>(colorTuple), std::get<1>(colorTuple), std::get<2>(colorTuple)};

    // Calculamos la distancia cuadrada al color objetivo
    double distanciaActual = distanciaCuadrada(colorObjetivo, currentColor);

    // Inicializamos el mejor color y la mejor distancia
    Color mejorColor = currentColor;
    double mejorDistancia = distanciaActual;

    // Determinamos el siguiente nodo a explorar basado en el eje de partición
    const std::unique_ptr<KDNode>& siguienteNodo = (axis == 0 ? colorObjetivo.red < currentColor.red :
                                                    axis == 1 ? colorObjetivo.green < currentColor.green :
                                                                colorObjetivo.blue < currentColor.blue) ? node->left : node->right;
    const std::unique_ptr<KDNode>& nodoOpuesto = (siguienteNodo == node->left) ? node->right : node->left;

    // Llamada recursiva para explorar el siguiente nodo
    auto [colorVecino, distanciaVecino] = buscarVecinoMasCercano(siguienteNodo, colorObjetivo, depth + 1, palette);
    if (distanciaVecino < mejorDistancia) {
        mejorColor = colorVecino;
        mejorDistancia = distanciaVecino;
    }

    // Poda temprana: solo exploramos el subárbol opuesto si la distancia al plano es menor que la mejor distancia
    double distanciaAlPlano = (axis == 0 ? std::abs(colorObjetivo.red - currentColor.red) :
                               axis == 1 ? std::abs(colorObjetivo.green - currentColor.green) :
                                           std::abs(colorObjetivo.blue - currentColor.blue));

    // Solo se explora el subárbol opuesto si la distancia al plano es pequeña
    if (distanciaAlPlano < mejorDistancia) {
        auto [colorOpuesto, distanciaOpuesta] = buscarVecinoMasCercano(nodoOpuesto, colorObjetivo, depth + 1, palette);
        if (distanciaOpuesta < mejorDistancia) {
            mejorColor = colorOpuesto;
            mejorDistancia = distanciaOpuesta;
        }
    }

    return {mejorColor, mejorDistancia};
}


// Función auxiliar para encontrar los colores menos frecuentes usando SOA
std::vector<std::tuple<uint8_t, uint8_t, uint8_t>> encontrar_colores_menos_frecuentes_soa(const ColorPalette& palette, const std::vector<int>& frecuencias, int n) {
    std::vector<size_t> indices(frecuencias.size());
    std::iota(indices.begin(), indices.end(), 0);
    
    std::sort(indices.begin(), indices.end(), [&frecuencias](size_t i1, size_t i2) {
        return frecuencias[i1] < frecuencias[i2];
    });

    std::vector<std::tuple<uint8_t, uint8_t, uint8_t>> menos_frecuentes;
    for (size_t i = 0; i < static_cast<size_t>(n) && i < indices.size(); ++i) {
        menos_frecuentes.push_back(palette.getColor(indices[i]));
    }
    std::cout << "Colores menos frecuentes encontrados:" << menos_frecuentes.size() << '\n';
    return menos_frecuentes;
}

void processCutfreq(const std::string& inputFile, int numColors, const std::string& outputFile) {
    // Paso 1: Leer la imagen y obtener la información relevante
    auto[pixelList, colorPalette, colorFrequencies] = readImageAndStoreColors(inputFile);
    auto[width, height] = getPPMDimensions(inputFile);

    // Paso 2: Encontrar los colores menos frecuentes
    std::cout << "Encontrando colores menos frecuentes" << '\n';
    auto menos_frecuentes = encontrar_colores_menos_frecuentes_soa(colorPalette, colorFrequencies, numColors);

    // Paso 3: Crear un conjunto con los colores menos frecuentes (optimización de la inserción)
    std::unordered_set<std::tuple<uint8_t, uint8_t, uint8_t>> coloresMenosFrecuentesSet;
    for (const auto& color : menos_frecuentes) {
        coloresMenosFrecuentesSet.emplace(color);  // Usamos emplace para evitar copiar el valor
    }

    // Paso 4: Filtrar los colores restantes usando unordered_set (más eficiente que std::find)
    std::vector<size_t> indicesRestantes;
    for (size_t i = 0; i < colorPalette.size(); ++i) {
        auto color = colorPalette.getColor(i);
        auto colorTuple = std::make_tuple(std::get<0>(color), std::get<1>(color), std::get<2>(color));
        if (coloresMenosFrecuentesSet.find(colorTuple) == coloresMenosFrecuentesSet.end()) {
            indicesRestantes.push_back(i);  // Solo agregamos los colores que no son menos frecuentes
        }
    }

    // Paso 5: Construir el K-D Tree con los colores restantes
    std::cout << "Construyendo el K-D Tree con los colores restantes:" << indicesRestantes.size() << '\n';
    auto kdTreeRoot = construirKDTreeRecursivo(colorPalette, indicesRestantes, 0);

    // Paso 6: Buscar los colores más cercanos para los colores menos frecuentes
    std::cout << "Buscando colores más cercanos" << '\n';
    std::unordered_map<std::tuple<uint8_t, uint8_t, uint8_t>, std::tuple<uint8_t, uint8_t, uint8_t>> sustituciones;

    // Precalcular las tuplas de colores menos frecuentes
    std::unordered_map<std::tuple<uint8_t, uint8_t, uint8_t>, Color> colorMap;
    for (const auto& color : menos_frecuentes) {
        colorMap[color] = {std::get<0>(color), std::get<1>(color), std::get<2>(color)};
    }

    // Buscar el vecino más cercano para cada color menos frecuente
    for (const auto& color_menos_frecuente : menos_frecuentes) {
        Color menosFrecuenteColor = colorMap[color_menos_frecuente];  // Usar el mapa precalculado
        auto [colorMasCercano, minDistancia] = buscarVecinoMasCercano(kdTreeRoot, menosFrecuenteColor, 0, colorPalette);

        // Verificar que el color sustituido no sea uno de los menos frecuentes
        auto sustituto = std::make_tuple(colorMasCercano.red, colorMasCercano.green, colorMasCercano.blue);
        if (coloresMenosFrecuentesSet.find(sustituto) == coloresMenosFrecuentesSet.end()) {
            sustituciones[color_menos_frecuente] = sustituto;
        }
    }

    std::cout << "Número de sustituciones creadas: " << sustituciones.size() << '\n';

    // Paso 7: Sustituir los colores en la lista de píxeles
    std::cout << "Sustituyendo colores menos frecuentes" << '\n';
    for (auto& pixel : pixelList) {
        auto colorTuple = std::make_tuple(pixel.red, pixel.green, pixel.blue);
        if (sustituciones.find(colorTuple) != sustituciones.end()) {
            auto sustituto = sustituciones[colorTuple];
            pixel.red = std::get<0>(sustituto);
            pixel.green = std::get<1>(sustituto);
            pixel.blue = std::get<2>(sustituto);
        }
    }

    // Paso 8: Escribir el archivo de salida
    escribirPPM(outputFile, pixelList, width, height);
}

/*//PRUEBAS CON GRID

#include "imagesoa.hpp"
#include "color.hpp"
#include "color.cpp"
#include <iostream>
#include <fstream>
#include <cmath>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <numeric>
#include <string>
#include <unordered_set>
#include <memory>
#include <tuple>
#include <limits>
#include <cstdint>

constexpr int MAX_COLOR_VALUE = 255;
constexpr size_t BUFFER_SIZE = 4096; // Ajusta según sea necesario

// Función para calcular la distancia cuadrada entre dos colores
double distanciaCuadrada(const Color& colora, const Color& colorb) {
    return (colora.red - colorb.red) * (colora.red - colorb.red) +
           (colora.green - colorb.green) * (colora.green - colorb.green) +
           (colora.blue - colorb.blue) * (colora.blue - colorb.blue);
}

// Función para leer imagen y almacenar los colores en un vector
std::tuple<std::vector<Color>, ColorPalette, std::vector<int>> readImageAndStoreColors(const std::string& inputFile) {
    std::cout << "Leyendo imagen de: " << inputFile << '\n';

    std::ifstream file(inputFile, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo de entrada: " << inputFile << '\n';
        return {{}, {}, {}};
    }

    std::string format;
    int width = 0, height = 0, maxColorValue = 0;
    file >> format >> width >> height >> maxColorValue;
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (format != "P6" || maxColorValue != 255) {
        std::cerr << "Error: Formato PPM no soportado o valor máximo de color inválido en " << inputFile << '\n';
        return {{}, {}, {}};
    }

    const size_t totalPixels = static_cast<size_t>(width) * static_cast<size_t>(height);
    std::vector<Color> pixelList;
    pixelList.reserve(totalPixels);

    std::vector<int> directColorFrequency(256 * 256 * 256, 0); // Para contar las frecuencias

    std::vector<uint8_t> buffer(BUFFER_SIZE);
    size_t pixelsRead = 0;
    size_t colorIndex;

    while (pixelsRead < totalPixels) {
        size_t pixelsToRead = std::min(BUFFER_SIZE / 3, totalPixels - pixelsRead);
        file.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(pixelsToRead * 3));

        for (size_t i = 0; i < pixelsToRead; ++i) {
            uint8_t red = buffer[i * 3];
            uint8_t green = buffer[i * 3 + 1];
            uint8_t blue = buffer[i * 3 + 2];

            colorIndex = (static_cast<size_t>(red) << 16) | 
                         (static_cast<size_t>(green) << 8) | 
                         blue;

            pixelList.push_back({red, green, blue});
            directColorFrequency[colorIndex]++;
        }
        pixelsRead += pixelsToRead;
    }

    file.close();

    ColorPalette colorPalette;
    std::vector<int> colorFrequencies;

    for (size_t i = 0; i < directColorFrequency.size(); ++i) {
        if (directColorFrequency[i] > 0) {
            uint8_t red = (i >> 16) & 0xFF;
            uint8_t green = (i >> 8) & 0xFF;
            uint8_t blue = i & 0xFF;
            colorPalette.addColor(red, green, blue);
            colorFrequencies.push_back(directColorFrequency[i]);
        }
    }

    return {pixelList, colorPalette, colorFrequencies};
}

// Función para encontrar los n colores menos frecuentes
std::vector<Color> encontrar_colores_menos_frecuentes(const std::unordered_map<Color, int>& frecuencia, int n) {
    std::vector<std::pair<Color, int>> colores_frecuentes(frecuencia.begin(), frecuencia.end());
    std::sort(colores_frecuentes.begin(), colores_frecuentes.end(), 
        [](const auto& colora, const auto& colorb) {
            return colora.second < colorb.second;
        }
    );

    std::vector<Color> menos_frecuentes;
    for (size_t i = 0; i < static_cast<size_t>(n) && i < colores_frecuentes.size(); ++i) {
        menos_frecuentes.push_back(colores_frecuentes[i].first);
    }
    return menos_frecuentes;
}

// Función para asignar colores a celdas del grid
std::tuple<int, int, int> asignarCelda(const Color& color) {
    constexpr int GRID_SIZE = 16; // Define el tamaño del grid
    int x = static_cast<int>(color.red * (GRID_SIZE - 1) / 255.0);
    int y = static_cast<int>(color.green * (GRID_SIZE - 1) / 255.0);
    int z = static_cast<int>(color.blue * (GRID_SIZE - 1) / 255.0);
    return {x, y, z};
}

// Función para buscar el vecino más cercano usando el grid
std::pair<Color, double> buscarVecinoMasCercanoEnGrid(const Color& colorObjetivo, const std::vector<std::vector<std::vector<std::vector<Color>>>>& grid) {
    constexpr int GRID_SIZE = 16; // Asegúrate de que coincida con el valor usado en asignarCelda
    auto [cx, cy, cz] = asignarCelda(colorObjetivo);

    double mejorDistancia = std::numeric_limits<double>::max();
    Color mejorColor = {0, 0, 0};

    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dz = -1; dz <= 1; ++dz) {
                int nx = cx + dx;
                int ny = cy + dy;
                int nz = cz + dz;

                if (nx >= 0 && nx < GRID_SIZE && ny >= 0 && ny < GRID_SIZE && nz >= 0 && nz < GRID_SIZE) {
                    for (const auto& vecino : grid[nx][ny][nz]) {
                        double distancia = distanciaCuadrada(colorObjetivo, vecino);
                        if (distancia < mejorDistancia) {
                            mejorDistancia = distancia;
                            mejorColor = vecino;
                        }
                    }
                }
            }
        }
    }

    return {mejorColor, mejorDistancia};
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

void escribirPPM(const std::string& filename, const std::vector<Color>& pixels, int width, int height) {
    std::ofstream outputFile(filename, std::ios::binary);
    if (!outputFile) {
        std::cout << "Error: No se pudo abrir el archivo para escribir: " << filename << '\n';
        throw std::runtime_error("No se pudo abrir el archivo para escribir");
    }

    outputFile << "P6\n" << width << " " << height << "\n255\n";
    for (const auto& pixel : pixels) {
        outputFile.put(static_cast<char>(pixel.red));
        outputFile.put(static_cast<char>(pixel.green));
        outputFile.put(static_cast<char>(pixel.blue));
    }
    std::cout << "Archivo PPM escrito exitosamente: " << filename << '\n';
}

std::vector<std::vector<std::vector<std::vector<Color>>>> construirGrid(const ColorPalette& colorPalette, const std::vector<size_t>& indicesRestantes, int gridSize) {
    std::vector<std::vector<std::vector<std::vector<Color>>>> grid(gridSize, std::vector<std::vector<std::vector<Color>>>(gridSize, std::vector<std::vector<Color>>(gridSize)));

    for (size_t idx : indicesRestantes) {
        auto colorTuple = colorPalette.getColor(idx);
        Color color{std::get<0>(colorTuple), std::get<1>(colorTuple), std::get<2>(colorTuple)};
        auto [x, y, z] = asignarCelda(color);
        grid[x][y][z].push_back(color);
    }


    return grid;
}
std::vector<std::tuple<uint8_t, uint8_t, uint8_t>> encontrar_colores_menos_frecuentes_soa(const ColorPalette& palette, const std::vector<int>& frecuencias, int n) {
    std::vector<size_t> indices(frecuencias.size());
    std::iota(indices.begin(), indices.end(), 0);
    
    std::sort(indices.begin(), indices.end(), [&frecuencias](size_t i1, size_t i2) {
        return frecuencias[i1] < frecuencias[i2];
    });

    std::vector<std::tuple<uint8_t, uint8_t, uint8_t>> menos_frecuentes;
    for (size_t i = 0; i < static_cast<size_t>(n) && i < indices.size(); ++i) {
        menos_frecuentes.push_back(palette.getColor(indices[i]));
    }
    std::cout << "Colores menos frecuentes encontrados:" << menos_frecuentes.size() << '\n';
    return menos_frecuentes;
}

// Función principal para procesar la imagen, realizar sustituciones y guardar la nueva imagen
void processCutfreq(const std::string& inputFile, int numColors, const std::string& outputFile) {
    auto[pixelList, colorPalette, colorFrequencies] = readImageAndStoreColors(inputFile);
    auto[width, height] = getPPMDimensions(inputFile);

    std::cout << "Encontrando colores menos frecuentes\n";
    auto menos_frecuentes = encontrar_colores_menos_frecuentes_soa(colorPalette, colorFrequencies, numColors);

    // Construir el grid con los colores restantes
    std::cout << "Construyendo el Grid con los colores restantes\n";
    std::vector<size_t> indicesRestantes;
    for (size_t i = 0; i < colorPalette.size(); ++i) {
        auto color = colorPalette.getColor(i);
        if (std::find(menos_frecuentes.begin(), menos_frecuentes.end(), color) == menos_frecuentes.end()) {
            indicesRestantes.push_back(i);
        }
    }
    auto grid = construirGrid(colorPalette, indicesRestantes, 32);  // Tamaño del grid ajustable

    std::cout << "Buscando colores más cercanos\n";
    std::unordered_map<std::tuple<uint8_t, uint8_t, uint8_t>, Color> sustituciones;

    // Realizar sustituciones
    for (const auto& color_menos_frecuente : menos_frecuentes) {
        Color colorTemp{std::get<0>(color_menos_frecuente), std::get<1>(color_menos_frecuente), std::get<2>(color_menos_frecuente)};
        auto [colorMasCercano, _] = buscarVecinoMasCercanoEnGrid(colorTemp, grid);
        sustituciones[color_menos_frecuente] = colorMasCercano;
    }

    // Sustituir colores en los píxeles de la imagen
    for (auto& pixel : pixelList) {
        auto colorTuple = std::make_tuple(pixel.red, pixel.green, pixel.blue);
        if (sustituciones.find(colorTuple) != sustituciones.end()) {
            pixel = sustituciones[colorTuple];
        }
    }

    // Escribir la imagen resultante
    escribirPPM(outputFile, pixelList, width, height);
}*/




/*//PRUEBAS CON OCTREES
#include "imagesoa.hpp"
#include "color.hpp"
#include "color.cpp"
#include <iostream>
#include <fstream>
#include <cmath>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <numeric>
#include <string>
#include <unordered_set>
#include <memory>
#include <tuple>
#include <limits>
#include <unordered_map>

constexpr size_t BUFFER_SIZE = 4096;  // Buffer size for reading image data
struct ColorHash {
    size_t operator()(const Color& color) const {
        return (static_cast<size_t>(color.red) << 16) |
               (static_cast<size_t>(color.green) << 8) |
               static_cast<size_t>(color.blue);
    }
};

double distanciaCuadrada(const Color& colora, const Color& colorb) {
    int dr = colora.red - colorb.red;
    int dg = colora.green - colorb.green;
    int db = colora.blue - colorb.blue;
    return dr * dr + dg * dg + db * db;
}

// Octree 3D para la gestión de colores
class Octree {
public:
    Octree(int depth = 0) : depth(depth) {}

    void insert(const Color& color) {
        if (depth == MAX_DEPTH) {
            colors.push_back(color);
            return;
        }
        int octant = getOctant(color);
        if (octant < 0 || octant >= 8) {
            std::cerr << "Error: octant fuera de rango: " << octant << '\n';
            return;
        }
        if (!children[octant]) {
            children[octant] = std::make_unique<Octree>(depth + 1);
        }
        children[octant]->insert(color);
    }

    void searchNearbyColors(const Color& target, std::vector<Color>& result, double maxDist) {
        if (depth == MAX_DEPTH) {
            for (const auto& color : colors) {
                if (distanciaCuadrada(color, target) < maxDist) {
                    result.push_back(color);
                }
            }
            return;
        }
        int octant = getOctant(target);
        if (octant < 0 || octant >= 8) {
            std::cerr << "Error: octant fuera de rango: " << octant << '\n';
            return;
        }
        if (children[octant]) {
            children[octant]->searchNearbyColors(target, result, maxDist);
        }
    }

private:
    static constexpr int MAX_DEPTH = 8;  // Profundidad máxima del Octree
    int depth;
    std::vector<Color> colors;
    std::unique_ptr<Octree> children[8] = {nullptr};

    int getOctant(const Color& color) const {
        return ((color.red > 128) << 2) | ((color.green > 128) << 1) | (color.blue > 128);
    }
};


// Función para encontrar colores menos frecuentes
// Función para encontrar colores menos frecuentes


// Función para leer la imagen y almacenar frecuencias de colores con unordered_map
std::tuple<std::vector<Color>, std::unordered_map<Color, int, ColorHash>> leerImagenYGuardarColores(const std::string& inputFile) {
    std::ifstream file(inputFile, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo de entrada: " << inputFile << '\n';
        return {{}, {}};
    }

    std::string format;
    int width = 0, height = 0, maxColorValue = 0;
    file >> format >> width >> height >> maxColorValue;
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (format != "P6" || maxColorValue != 255) {
        std::cerr << "Error: Formato PPM no soportado o valor máximo de color inválido en " << inputFile << '\n';
        return {{}, {}};
    }

    size_t totalPixels = static_cast<size_t>(width) * static_cast<size_t>(height);
    std::vector<Color> pixelList;
    pixelList.reserve(totalPixels);

    std::unordered_map<Color, int, ColorHash> colorFrequency;

    std::vector<uint8_t> buffer(BUFFER_SIZE);
    size_t pixelsRead = 0;

    while (pixelsRead < totalPixels) {
        size_t pixelsToRead = std::min(BUFFER_SIZE / 3, totalPixels - pixelsRead);
        file.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(pixelsToRead * 3));

        for (size_t i = 0; i < pixelsToRead; ++i) {
            uint8_t red = buffer[i * 3];
            uint8_t green = buffer[i * 3 + 1];
            uint8_t blue = buffer[i * 3 + 2];
            Color color = {red, green, blue};
            pixelList.push_back(color);
            colorFrequency[color]++;
        }
        pixelsRead += pixelsToRead;
    }

    file.close();

    return {pixelList, colorFrequency};
}

// Función para encontrar colores menos frecuentes
std::vector<Color> encontrarColoresMenosFrecuentes(const std::vector<Color>& colores, const std::unordered_map<Color, int, ColorHash>& frecuencias, int n) {
    std::vector<std::pair<Color, int>> frecuenciaVec(frecuencias.begin(), frecuencias.end());
    
    // Ordenar en función de la frecuencia
    std::sort(frecuenciaVec.begin(), frecuenciaVec.end(), [](const std::pair<Color, int>& a, const std::pair<Color, int>& b) {
        return a.second < b.second;
    });

    std::vector<Color> menosFrecuentes;
    for (size_t i = 0; i < static_cast<size_t>(n) && i < frecuenciaVec.size(); ++i) {
        menosFrecuentes.push_back(frecuenciaVec[i].first);
    }

    return menosFrecuentes;
}
std::vector<Color> encontrarColoresMenosFrecuentes(const std::unordered_map<Color, int, ColorHash>& frecuencias, int n) {
    std::vector<std::pair<Color, int>> frecuenciaVec(frecuencias.begin(), frecuencias.end());
    
    // Ordenar en función de la frecuencia
    std::sort(frecuenciaVec.begin(), frecuenciaVec.end(), [](const std::pair<Color, int>& a, const std::pair<Color, int>& b) {
        return a.second < b.second;
    });

    std::vector<Color> menosFrecuentes;
    for (size_t i = 0; i < static_cast<size_t>(n) && i < frecuenciaVec.size(); ++i) {
        menosFrecuentes.push_back(frecuenciaVec[i].first);
    }

    return menosFrecuentes;
}
std::vector<Color> encontrarColoresMenosFrecuentes(const std::vector<Color>& colores, const std::unordered_map<Color, int, ColorHash>& frecuencias, int n) {
    std::vector<std::pair<Color, int>> frecuenciaVec(frecuencias.begin(), frecuencias.end());
    
    // Ordenar en función de la frecuencia
    std::sort(frecuenciaVec.begin(), frecuenciaVec.end(), [](const std::pair<Color, int>& a, const std::pair<Color, int>& b) {
        return a.second < b.second;
    });

    std::vector<Color> menosFrecuentes;
    for (size_t i = 0; i < static_cast<size_t>(n) && i < frecuenciaVec.size(); ++i) {
        menosFrecuentes.push_back(frecuenciaVec[i].first);
    }

    return menosFrecuentes;
}

// Función principal para procesar la imagen
void processCutfreq(const std::string& inputFile, int numColors, const std::string& outputFile) {
    std::cout << "Leyendo imagen y almacenando colores de: " << inputFile << '\n';
    auto[pixelList, colorFrequency] = leerImagenYGuardarColores(inputFile);

    // Verificar si pixelList está vacío
    if (pixelList.empty()) {
        std::cerr << "Error: La lista de píxeles está vacía.\n";
        return;
    }

    std::cout << "Imagen leída con éxito. Total de píxeles leídos: " << pixelList.size() << '\n';

    // Paso 1: Encontrar los colores menos frecuentes
    std::cout << "Encontrando colores menos frecuentes\n";
    std::vector<Color> menosFrecuentes = encontrarColoresMenosFrecuentes(colorFrequency, numColors);

    // Verificar si se encontraron colores menos frecuentes
    if (menosFrecuentes.empty()) {
        std::cerr << "Error: No se encontraron colores menos frecuentes.\n";
        return;
    }

    std::cout << "Total de colores menos frecuentes encontrados: " << menosFrecuentes.size() << "\n";

    // Paso 2: Crear el Octree y agregar los colores menos frecuentes
    std::cout << "Creando el Octree y agregando los colores menos frecuentes\n";
    Octree octree;
    for (const auto& color : menosFrecuentes) {
        octree.insert(color);
    }

    // Paso 3: Buscar los colores cercanos en el Octree
    std::cout << "Buscando los colores cercanos en el Octree\n";
    std::unordered_map<Color, Color, ColorHash> sustituciones;
    for (const auto& color : menosFrecuentes) {
        std::vector<Color> resultados;
        octree.searchNearbyColors(color, resultados, 100.0); // Umbral de distancia cuadrada
        if (!resultados.empty()) {
            sustituciones[color] = resultados.front();  // Usar el primer color encontrado
        }
    }

    // Paso 4: Sustituir los colores en la imagen
    std::cout << "Sustituyendo los colores en la imagen\n";
    for (auto& pixel : pixelList) {
        auto it = sustituciones.find(pixel);
        if (it != sustituciones.end()) {
            pixel = it->second;
        }
    }

    // Paso 5: Escribir el archivo de salida PPM
    std::cout << "Escribiendo el archivo de salida PPM\n";
    std::ofstream outFile(outputFile, std::ios::binary);
    outFile << "P6\n" << 256 << " " << 256 << "\n255\n";
    for (const auto& pixel : pixelList) {
        outFile.put(static_cast<char>(pixel.red))
               .put(static_cast<char>(pixel.green))
               .put(static_cast<char>(pixel.blue));
    }
    outFile.close();
    std::cout << "Archivo PPM escrito exitosamente: " << outputFile << '\n";
}*/