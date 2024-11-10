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

// Función de distancia cuadrada para evitar la raíz cuadrada
double distanciaCuadrada(const Color& colora, const Color& colorb) {
    return (colora.red - colorb.red) * (colora.red - colorb.red) +
           (colora.green - colorb.green) * (colora.green - colorb.green) +
           (colora.blue - colorb.blue) * (colora.blue - colorb.blue);
}

std::tuple<std::vector<Color>, ColorPalette, std::vector<int>> readImageAndStoreColors(const std::string& inputFile) {
    std::cout << "Leyendo imagen y almacenando colores de: " << inputFile << '\n';
    // Abrir archivo en modo binario
    std::ifstream file(inputFile, std::ios::binary);
    std::string format;
    int width = 0;
    int height = 0;
    int maxColorValue = 0;
    
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo de entrada: " << inputFile << '\n';
        return {{}, {}, {}}; // Retorna todos vacíos si falla
    }
    
    // Leer el encabezado PPM
    file >> format >> width >> height >> maxColorValue;
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignorar el salto de línea
    if (format != "P6" || maxColorValue != MAX_COLOR_VALUE) {
        std::cerr << "Error: Formato PPM no soportado o valor máximo de color inválido en " << inputFile << '\n';
        return {{}, {}, {}}; // Retorna todos vacíos si hay error en formato
    }
    
    const size_t totalPixels = static_cast<size_t>(width) * static_cast<size_t>(height); // Asegúrate de que ambos sean size_t
    std::vector<Color> pixelList;
    pixelList.reserve(totalPixels); // Cambiado a size_t
    
    // Preparar el mapa de frecuencia de colores
    std::unordered_map<Color, int> colorFrequency;
    
    // Leer todos los datos de píxeles de una vez en un solo bloque
    std::vector<uint8_t> buffer(totalPixels * 3);  // Cambiamos a uint8_t para evitar reinterpret_cast
    file.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(buffer.size()));  // Ahora no es necesario reinterpret_cast
    
    // Procesar el buffer en bloques de tres bytes para extraer los colores 
    for (size_t i = 0; i < buffer.size(); i += 3) {
        uint8_t red = buffer[i];
        uint8_t green = buffer[i + 1];
        uint8_t blue = buffer[i + 2];
        
        // Crear el color solo cuando es necesario
        Color pixelColor = {red, green, blue};
        pixelList.push_back(pixelColor);
        colorFrequency[pixelColor]++;
    }
    
    file.close();
    std::cout << "Píxeles leídos y frecuencias calculadas" << '\n';
    
    // Convertir el mapa de frecuencia de colores a un vector de frecuencias
    std::vector<int> colorFrequencies;
    ColorPalette colorPalette;
    for (const auto& [color, frequency] : colorFrequency) {
        colorPalette.addColor(color.red, color.green, color.blue);
        colorFrequencies.push_back(frequency);
    }
    
    return {pixelList, colorPalette, colorFrequencies};
}



std::vector<Color> encontrar_colores_menos_frecuentes(const std::unordered_map<Color, int>& frecuencia, int n) {
    
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
std::pair<Color, double> buscarVecinoMasCercano(std::unique_ptr<KDNode>& node, const Color& colorObjetivo, int depth, ColorPalette& palette) {
    if (!node) return {{0, 0, 0}, std::numeric_limits<double>::max()};

    size_t axis = static_cast<size_t>(depth) % 3;
    auto currentColor = palette.getColor(node->colorIndex);
    double distanciaActual = distanciaCuadrada(colorObjetivo, {std::get<0>(currentColor), std::get<1>(currentColor), std::get<2>(currentColor)});

    Color mejorColor = {std::get<0>(currentColor), std::get<1>(currentColor), std::get<2>(currentColor)};
    double mejorDistancia = distanciaActual;

    // Determinamos el siguiente nodo a visitar en función del eje de partición
    std::unique_ptr<KDNode>& siguienteNodo = [&]() -> std::unique_ptr<KDNode>& {
        switch (axis) {
            case 0: return (colorObjetivo.red < std::get<0>(currentColor)) ? node->left : node->right;
            case 1: return (colorObjetivo.green < std::get<1>(currentColor)) ? node->left : node->right;
            case 2: return (colorObjetivo.blue < std::get<2>(currentColor)) ? node->left : node->right;
            default: return node->left; // Para evitar advertencias del compilador
        }
    }();

    auto [colorVecino, distanciaVecino] = buscarVecinoMasCercano(siguienteNodo, colorObjetivo, depth + 1, palette);

    if (distanciaVecino < mejorDistancia) {
        mejorColor = colorVecino;
        mejorDistancia = distanciaVecino;
    }

    // Comprobar si el color vecino es idéntico al color objetivo
    if (mejorColor.red == colorObjetivo.red && mejorColor.green == colorObjetivo.green && mejorColor.blue == colorObjetivo.blue) {
        // Si el color es idéntico al objetivo, exploramos el subárbol opuesto
        std::unique_ptr<KDNode>& nodoOpuesto = (siguienteNodo == node->left) ? node->right : node->left;
        auto [colorOpuesto, distanciaOpuesta] = buscarVecinoMasCercano(nodoOpuesto, colorObjetivo, depth + 1, palette);

        // Si encontramos un color en el subárbol opuesto que no sea idéntico y que esté más cerca, lo tomamos como el mejor
        if (distanciaOpuesta < mejorDistancia) {
            mejorColor = colorOpuesto;
            mejorDistancia = distanciaOpuesta;
        }
    }

    return {mejorColor, mejorDistancia};
}

// Función de búsqueda de vecino más cercano


std::pair<Color, double> buscarVecinoMasCercano(std::unique_ptr<KDNode>& root, const Color& target, int depth) {
    if (!root) {
        // Si el nodo es nulo, devolvemos un valor con la máxima distancia posible
        return {Color{0, 0, 0}, std::numeric_limits<double>::max()};
    }
    // Calcular la distancia cuadrada
    const double distancia = distanciaCuadrada(target, root->color);
    Color mejorColor = root->color;
    double mejorDistanciaCuadrada = distancia;
    // Determinar el eje de comparación actual
    const int axis = depth % 3;
    std::unique_ptr<KDNode>& siguiente = (axis == 0 && target.red < root->color.red) ||
                                         (axis == 1 && target.green < root->color.green) ||
                                         (axis == 2 && target.blue < root->color.blue) ? root->left : root->right;

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
        distanciaEje = (target.red - root->color.red) * (target.red - root->color.red);
    } else if (axis == 1) {
        distanciaEje = (target.green - root->color.green) * (target.green - root->color.green);
    } else {
        distanciaEje = (target.blue - root->color.blue) * (target.blue - root->color.blue);
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
    auto[pixelList, colorPalette, colorFrequencies] = readImageAndStoreColors(inputFile);
    auto[width, height] = getPPMDimensions(inputFile);

    // Encontrar los colores menos frecuentes
    std::cout << "Encontrando colores menos frecuentes" << '\n';
    auto menos_frecuentes = encontrar_colores_menos_frecuentes_soa(colorPalette, colorFrequencies, numColors);

    // Construir un conjunto de colores menos frecuentes para facilitar la búsqueda
    std::unordered_set<std::tuple<uint8_t, uint8_t, uint8_t>> coloresMenosFrecuentesSet;
    for (const auto& color : menos_frecuentes) {
        coloresMenosFrecuentesSet.insert(color);
    }

    // Filtrar los colores en el ColorPalette para excluir los colores menos frecuentes
    std::vector<size_t> indicesRestantes;
    for (size_t i = 0; i < colorPalette.size(); ++i) {
        auto color = colorPalette.getColor(i);
        auto colorTuple = std::make_tuple(std::get<0>(color), std::get<1>(color), std::get<2>(color));
        if (coloresMenosFrecuentesSet.find(colorTuple) == coloresMenosFrecuentesSet.end()) {
            indicesRestantes.push_back(i); // Solo agregar los índices de los colores que no son menos frecuentes
        }
    }

    // Construir el K-D Tree con los colores restantes
    std::cout << "Construyendo el K-D Tree con los colores restantes:" << indicesRestantes.size() << '\n';
    auto kdTreeRoot = construirKDTreeRecursivo(colorPalette, indicesRestantes, 0);

    std::cout << "Buscando colores más cercanos" << '\n';
    std::unordered_map<std::tuple<uint8_t, uint8_t, uint8_t>, std::tuple<uint8_t, uint8_t, uint8_t>> sustituciones;

    for (const auto& color_menos_frecuente : menos_frecuentes) {
        Color menosFrecuenteColor = {std::get<0>(color_menos_frecuente), std::get<1>(color_menos_frecuente), std::get<2>(color_menos_frecuente)};
        auto [colorMasCercano, minDistancia] = buscarVecinoMasCercano(kdTreeRoot, menosFrecuenteColor, 0, colorPalette);

        // Verificar que el color sustituto no sea uno de los colores menos frecuentes
        auto sustituto = std::make_tuple(colorMasCercano.red, colorMasCercano.green, colorMasCercano.blue);
        if (std::find(menos_frecuentes.begin(), menos_frecuentes.end(), sustituto) == menos_frecuentes.end()) {
            sustituciones[color_menos_frecuente] = sustituto;
        }
    }
    std::cout << "Número de sustituciones creadas: " << sustituciones.size() << '\n';

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

    escribirPPM(outputFile, pixelList, width, height);
}
