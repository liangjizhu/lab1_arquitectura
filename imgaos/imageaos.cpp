#include "imageaos.hpp"
#include "binaryio.hpp"
#include "progargs.hpp"
#include "imageinfo.hpp"
#include "color.hpp"

#include <iostream>
#include <fstream>
// #include <cmath>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
#include <memory>
#include <limits>
void processCutfreq(const std::string& inputFile, int frequency, const std::string& outputFile);
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
    std::sort(colorTable.begin(), colorTable.end(), [](const Color& colorA, const Color& colorB) {
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

//PRUEBAS CON KTREES
//*******************PROCESSCUTFREQ**********************
//Toma como parámetro de entrada el unordered map (llamado frecuencia) y el número de colores a seleccionar, y retorna el vector de los n colores menos frecuentes

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

void readImageAndStoreColors(const std::string& inputFile, std::vector<Color>& pixelList, std::unordered_map<Color, int, HashColor>& colorFrequency) {
    std::cout << "Leyendo imagen y almacenando colores de: " << inputFile << '\n';
    // Abrir archivo en modo binario
    std::ifstream file(inputFile, std::ios::binary);
    std::string format;
    int width = 0;
    int height = 0;
    int maxColorValue = 0;
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo de entrada: " << inputFile << '\n';
        return; // Retorna ambos vacíos si falla
    }
    // Leer el encabezado PPM
    file >> format >> width >> height >> maxColorValue;
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignorar el salto de línea
    if (format != "P6" || maxColorValue != MAX_COLOR_VALUE) {
        std::cerr << "Error: Formato PPM no soportado o valor máximo de color inválido en " << inputFile << '\n';
        return; // Retorna ambos vacíos si hay error en formato
    }
    const size_t totalPixels = static_cast<size_t>(width) * static_cast<size_t>(height); // Asegúrate de que ambos sean size_t
    //std::vector<Color> pixelList;

    pixelList.reserve(totalPixels); // Cambiado a size_t
    // Preparar el mapa de frecuencia de colores
    //std::unordered_map<Color, int> colorFrequency;
    // Leer todos los datos de píxeles de una vez en un solo bloque
    std::vector<char> buffer(totalPixels * 3);  // Cambiamos a char para evitar reinterpret_cast
    file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));  // Ahora no es necesario reinterpret_cast
    // Procesar el buffer en bloques de tres bytes para extraer los colores

    for (size_t i = 0; i < buffer.size(); i += 3) {
        const RGBColor rgbColor = {static_cast<uint16_t>(buffer[i]), 
                           static_cast<uint16_t>(buffer[i + 1]), 
                           static_cast<uint16_t>(buffer[i + 2])};
        const Color pixelColor(rgbColor);
        pixelList.push_back(pixelColor);
        colorFrequency[pixelColor]++;
    }
    file.close();
    std::cout << "Píxeles leídos y frecuencias calculadas" << '\n';
    return;
}


void readImageAndStoreColors2(const std::string& inputFile, std::vector<Color>& pixelList, std::unordered_map<Color, int, HashColor>& colorFrequency) {
    std::cout << "Leyendo imagen y almacenando colores de: " << inputFile << '\n';
    // Abrir archivo en modo binario
    std::ifstream file(inputFile, std::ios::binary);
    std::string format;
    int width = 0;
    int height = 0;
    int maxColorValue = 0;
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo de entrada: " << inputFile << '\n';
        return; // Retorna ambos vacíos si falla
    }
    // Leer el encabezado PPM
    file >> format >> width >> height >> maxColorValue;
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignorar el salto de línea
    if (format != "P6" || maxColorValue != MAX_COLOR_VALUE) {
        std::cerr << "Error: Formato PPM no soportado o valor máximo de color inválido en " << inputFile << '\n';
        return; // Retorna ambos vacíos si hay error en formato
    }
    const size_t totalPixels = static_cast<size_t>(width) * static_cast<size_t>(height); // Asegúrate de que ambos sean size_t
    //std::vector<Color> pixelList;

    pixelList.reserve(totalPixels); // Cambiado a size_t
    // Preparar el mapa de frecuencia de colores
    //std::unordered_map<Color, int> colorFrequency;
    // Leer todos los datos de píxeles de una vez en un solo bloque
    std::vector<char> buffer(totalPixels * 3);  // Cambiamos a char para evitar reinterpret_cast
    file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));  // Ahora no es necesario reinterpret_cast
    // Procesar el buffer en bloques de tres bytes para extraer los colores

    for (size_t i = 0; i < buffer.size(); i += 3) {
        const RGBColor rgbColor = {static_cast<uint16_t>(buffer[i]), 
                           static_cast<uint16_t>(buffer[i + 1]), 
                           static_cast<uint16_t>(buffer[i + 2])};
        const Color pixelColor(rgbColor);
        pixelList.push_back(pixelColor);
        colorFrequency[pixelColor]++;
    }
    file.close();
    std::cout << "Píxeles leídos y frecuencias calculadas" << '\n';
    return;
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
        return {Color(), std::numeric_limits<double>::max()};
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

std::unordered_map<Color, int> calculateColorFrequency(const std::vector<Color>& pixelList) {
    std::cout << "Calculando frecuencias de colores" << '\n';
    std::unordered_map<Color, int> colorFrequency;

    // Reservar espacio para evitar realocaciones innecesarias
    colorFrequency.reserve(100000);  // Ajusta este número según tus expectativas

    // Usamos try_emplace para evitar búsquedas duplicadas
    for (const auto& color : pixelList) {
        colorFrequency.try_emplace(color, 0).first->second++;
    }

    return colorFrequency;
}

/*std::pair<std::vector<Color>, std::unordered_map<Color, int>> readImageAndStoreColorsTrial(const std::string& inputFile, int width, int height) {
    std::vector<Color> pixelList = readPixelsFromImage(inputFile, width, height);
    std::unordered_map<Color, int> colorFrequency = calculateColorFrequency(pixelList);
    return {pixelList, colorFrequency};
}*/

void processCutfreq(const std::string& inputFile, int numColors, const std::string& outputFile) {
    std::cout << "Leyendo imagen y almacenando colores" << '\n';
    std::vector<Color> pixelList;
    std::unordered_map<Color, int, HashColor> colorTable;
    auto [width, height] = getPPMDimensions(inputFile);
    readImageAndStoreColors(inputFile, pixelList, colorTable);
    std::cout << "Número de colores en la imagen: " << colorTable.size() << '\n';

    // Encontrar los colores menos frecuentes
    std::vector<Color> menos_frecuentes = encontrar_colores_menos_frecuentes(colorTable, numColors);
    
    // Filtrar colores frecuentes para construir el K-D Tree
    std::unordered_set<Color, HashColor> menosFrecuentesSet(menos_frecuentes.begin(), menos_frecuentes.end());
    std::vector<Color> coloresRestantes;
    for (const auto& [color, _] : colorTable) {
        if (menosFrecuentesSet.find(color) == menosFrecuentesSet.end()) {
            coloresRestantes.push_back(color);
        }
    }

    // Construir el K-D Tree con los colores restantes
    std::cout << "Construyendo K-D Tree" << '\n';
    std::unique_ptr<KDNode> kdTreeRoot = construirKDTree(coloresRestantes);

    // Mapear cada color menos frecuente a su color más cercano en el K-D Tree
    std::cout << "Buscando colores más cercanos" << '\n';
    std::unordered_map<Color, Color, HashColor> sustituciones;
    sustituciones.reserve(menos_frecuentes.size()); // Reservamos espacio para el mapa de sustituciones
    for (const auto& color_menos_frecuente : menos_frecuentes) {
        auto [colorMasCercano, minDistancia] = buscarVecinoMasCercano(kdTreeRoot, color_menos_frecuente, 0);
        sustituciones[color_menos_frecuente] = colorMasCercano;
    }

    // Sustituir los colores directamente en la lista de píxeles de manera más eficiente
    std::cout << "Sustituyendo colores menos frecuentes" << '\n';
    for (auto& pixel : pixelList) {
        auto it = sustituciones.find(pixel);  // Solo se hace una búsqueda
        if (it != sustituciones.end()) {
            pixel = it->second;  // Sustituir directamente el color en memoria
        }
    }

    std::cout << "Escribiendo imagen modificada" << '\n';
    escribirPPM(outputFile, pixelList, width, height);
    std::cout << "Imagen modificada escrita en: " << outputFile << '\n';
}

//NOLINTEND(misc-no-recursion)
