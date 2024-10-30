//
// Created by liang on 4/10/24.
//

#include "imageaos.hpp"
#include "binaryio.hpp"
#include "color.hpp"

#include <iostream>
#include <fstream>
#include <cmath>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <string>
#include <unordered_set>


// TODO:
//  - PARA ESTA OPERACIÓN CREO QUE SE PODRÍA OPTIMIZAR SI SE EJECUTA AL MISMO TIEMPO QUE SE LEA EL ARCHIVO
void processMaxLevel(std::vector<uint8_t> inputFile, int maxLevel) {
    int contadorDeLineas = 0;
    std::string maxIntensidadStr = "";
    int maxIntensidadInt;
    //bool altaIntensidad;

    //int contador = 0;
    //int contador_aux = 0;
    //uint8_t pet[2];
    //uint16_t caster;
    
    //Leer archivo
    for (auto i = inputFile.begin(); i != inputFile.end(); ++i){
        if (contadorDeLineas < 3){
            std::cout << *i;
        }

        //Tener en cuenta las líneas (usando los newlines)
        if (*i == '\n'){
            //Ubicación de máxima intensidad en el archivo ppm
            if (contadorDeLineas == 2){ 
                std::cout << "Máxima intensidad: " << maxIntensidadStr << "\n";
                maxIntensidadInt  = std::stoi(maxIntensidadStr);
                
                if (maxIntensidadInt < 256){
                    std::cout << "Intensidad baja";
                }
            }
            contadorDeLineas++;
        } else if ((*i == ' ') || (*i == '\t')){
            std::cout << "\ndo nothing\n";
        } else {
            if (contadorDeLineas == 2) {
                maxIntensidadStr.append(1, static_cast<char>(*i));
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
    std::cout << "\nProcessing 'maxlevel' for file: " << inputFile.size() << " with max level: " << maxLevel << std::endl;

    // Aquí iría la lógica para modificar el nivel máximo del archivo
}


//VOY A IMAGINAR QUE HAY UN STRUCT PARA CADA COLOR (lo voy a llamar Color) 
//Y UN VECTOR LLAMADO ArrayOfColors QUE CONTIENE TODOS LOS COLORES DE LA IMAGEN

// COLOR IMPLEMENTADO EN COLOR.HPP
//FUNCIÓN PARA CALCULAR LA DISTANCIA EUCLÍDEA

//Tengo una idea para optimizar la función de distancia euclídea
//En vez de hacerlo con todos los píxeles, lo hago con todos los colores - los menos frecuentes

double distancia_euclidiana(const Color& c1, const Color& c2) {
    return std::pow(c1.red - c2.red, 2) +
                     std::pow(c1.green - c2.green, 2) +
                     std::pow(c1.blue - c2.blue, 2);
}


//FUNCIÓN PARA CONTAR LA FRECUENCIA DE CADA COLOR
//Un unordered_map es como un diccionario pero su tiempo de búsqueda es O(1), y si se repite la clave, se sobreescribe
//Paso el vector por referencia para no hacer una copia y así ser más eficiente
/*std::unordered_map<Color, int> contar_frecuencia(const std::vector<Color>& pixeles) {
    
    std::unordered_map<Color, int> frecuencia;

    for (const auto& pixel : pixeles) {
        frecuencia[pixel]++;
    }

    return frecuencia;
}*/

// FUNCIÓN PARA ENCONTRAR LOS COLORES MENOS FRECUENTES
// Para encontrar los colores menos frecuentes necesitamos que el unordered map pase a ser un vector de pares, ya que los vectores son ordenables
// Ordenaremos el vector de menor a mayor frecuencia y seleccionaremos los primeros 'n' colores


//ESTO ESTÁ MAL, TIENE QUE COGER UN STD::STR Y PASARLO A VECTOR DE COLORES
/*void processCutfreq(const std::string& inputFile, const std::string& outputFile, int numColors) {
    //Determinar la frecuencia absoluta de cada color
    std::cout << "Procesando 'cutfreq' para archivo: " << inputFile << " con " << numColors << " colores a eliminar" << std::endl;
    auto[pixelList, colorTable] = readImageAndStoreColors(inputFile);
    std::cout << "Número de colores en la imagen: " << colorTable.size() << std::endl;
    //Encontrar los colores menos frecuentes y ordenarlos
    std::vector<Color> menos_frecuentes  = encontrar_colores_menos_frecuentes(colorTable, numColors);
    std::cout << "Colores menos frecuentes: " << menos_frecuentes.size() << std::endl;
    std::unordered_map<Color, Color> sustituciones;
    std::unordered_set<Color> menos_frecuentes_set(menos_frecuentes.begin(), menos_frecuentes.end());

    std::unordered_set<Color> colores_restantes;

    // Llenar el set de colores restantes, excluyendo los menos frecuentes
    for (const auto& entry : colorTable) {
        const Color& color = entry.first;
        if (std::find(menos_frecuentes.begin(), menos_frecuentes.end(), color) == menos_frecuentes.end()) {
            colores_restantes.insert(color);
        }
    }

    std::cout << "Número de colores restantes: " << colores_restantes.size() << std::endl;

    // Calcular distancias euclídeas
    for (const auto& color_menos_frecuente : menos_frecuentes) {
        double min_distancia = std::numeric_limits<double>::max();
        Color color_mas_parecido;

        for (const auto& color : colores_restantes) {
            double distancia = distancia_euclidiana(color_menos_frecuente, color);
            if (distancia < min_distancia) {
                min_distancia = distancia;
                color_mas_parecido = color;
            }
        }
        sustituciones[color_menos_frecuente] = color_mas_parecido;
        //std::cout << "Color menos frecuente: " << color_menos_frecuente << " está más cerca de: " << color_mas_parecido << " (distancia: " << min_distancia << ")\n";
    }




    //for (const auto& color_menos_frecuente : menos_frecuentes) {
        //double min_distancia = std::numeric_limits<double>::max();
        //Color color_mas_parecido;

        //for (const auto& entry : colorTable) {
        //    const Color& color = entry.first;
        //    if (menos_frecuentes_set.find(color) == menos_frecuentes_set.end()) {
        //        double distancia = distancia_euclidiana(color_menos_frecuente, color);
        //        if (distancia < min_distancia) {
        //            min_distancia = distancia;
        //            color_mas_parecido = color;
        //        }
        //    }
        //}
        //sustituciones[color_menos_frecuente] = color_mas_parecido;
    //}
    //std::cout << "Mapa de sustituciones:\n";
    //for (const auto& pair : sustituciones) {
        //std::cout << "Color menos frecuente:" << pair.first << " será sustituido por: " << pair.second << std::endl;
    //}
    // Paso 5: Sustituir cada aparición de un color menos frecuente en pixeles por su color más cercano
    for (auto& pixel : pixelList) {
        if (sustituciones.find(pixel) != sustituciones.end()) {
            //std::cout << "Reemplazando pixel " << pixel << " por " << sustituciones[pixel] << std::endl;
            pixel = sustituciones[pixel];
        }
    }
    auto [width, height] = getPPMDimensions(inputFile);
    // Escribir la imagen modificada
    escribirPPM(outputFile, pixelList, width, height);
    std::cout << "Imagen modificada escrita en: " << outputFile << std::endl;

    //auto[pixelList2, colorTable2] = readImageAndStoreColors(outputFile);
    //std::cout << "Número de colores en la imagen: " << colorTable2.size() << std::endl;
}*/

// TODO
// CORREGIR TABLA DE COLORES
void compressAoS(const std::string& inputFile, const std::string& outputFile) {
    // Abrir archivo de entrada en modo binario
    std::ifstream file(inputFile, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo de entrada: " << inputFile << std::endl;
        return;
    }

    std::string format;
    int width, height, maxColorValue;

    // Leer el encabezado del archivo PPM (suponiendo formato P6)
    file >> format >> width >> height >> maxColorValue;
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignorar el salto de línea después del encabezado

    if (format != "P6" || maxColorValue <= 0 || maxColorValue > 65535) {
        std::cerr << "Error: Formato PPM no soportado o valor máximo de color inválido en " << inputFile << std::endl;
        return;
    }

    // Leer píxeles de la imagen
    std::vector<Color> imagePixels;
    for (int i = 0; i < width * height; ++i) {
        uint8_t r, g, b;
        file.read(reinterpret_cast<char*>(&r), 1);
        file.read(reinterpret_cast<char*>(&g), 1);
        file.read(reinterpret_cast<char*>(&b), 1);
        imagePixels.emplace_back(r, g, b);
    }
    file.close();

    // Crear tabla de colores y mapa de índices
    std::vector<Color> colorTable;
    std::unordered_map<Color, int> colorIndex;
    for (const auto& pixel : imagePixels) {
        if (colorIndex.find(pixel) == colorIndex.end()) {
            colorIndex[pixel] = static_cast<int>(colorTable.size()); // Cambio aquí para hacer casting a int
            colorTable.push_back(pixel);
        }
    }

    // Abrir archivo de salida en modo binario
    std::ofstream outFile(outputFile, std::ios::binary);
    outFile << "C6 " << width << " " << height << " " << maxColorValue << " " << colorTable.size() << "\n";

    // Escribir la tabla de colores
    for (const auto& color : colorTable) {
        if (maxColorValue <= 255) {
            outFile.put(static_cast<char>(color.red));
            outFile.put(static_cast<char>(color.green));
            outFile.put(static_cast<char>(color.blue));
        } else {
            outFile.write(reinterpret_cast<const char*>(&color.red), 2);
            outFile.write(reinterpret_cast<const char*>(&color.green), 2);
            outFile.write(reinterpret_cast<const char*>(&color.blue), 2);
        }
    }

    // Escribir los índices de píxeles
    int colorIndexSize = (colorTable.size() <= 256) ? 1 : (colorTable.size() <= 65536) ? 2 : 4;
    for (const auto& pixel : imagePixels) {
        int index = colorIndex[pixel];
        if (colorIndexSize == 1) {
            outFile.put(static_cast<char>(static_cast<uint8_t>(index)));
        } else if (colorIndexSize == 2) {
            uint16_t index16 = static_cast<uint16_t>(index);
            outFile.write(reinterpret_cast<const char*>(&index16), 2);
        } else if (colorIndexSize == 4) {
            uint32_t index32 = static_cast<uint32_t>(index);
            outFile.write(reinterpret_cast<const char*>(&index32), 4);
        }
    }

    outFile.close();
}


//PRUEBAS CON KTREES

//Toma como parámetro de entrada el unordered map (llamado frecuencia) y el número de colores a seleccionar, y retorna el vector de los n colores menos frecuentes

std::vector<Color> encontrar_colores_menos_frecuentes(const std::unordered_map<Color, int>& frecuencia, int n) {
    
    // Convertir el unordered_map en un vector de pares (color, frecuencia)
    std::vector<std::pair<Color, int>> colores_frecuentes(frecuencia.begin(), frecuencia.end());

    // Ordenar por frecuencia ascendente
    //sort es parte de <include algorithm>
    std::sort(colores_frecuentes.begin(), colores_frecuentes.end(), 
        [](const auto& a, const auto& b) {
            return a.second < b.second; // Comparar por la frecuencia (segundo elemento del par)
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
        std::cout << "Error: No se pudo abrir el archivo para escribir: " << filename << std::endl;
        std::cerr << "Error: No se pudo abrir el archivo para escribir: " << filename << std::endl;
        return;
    }

    // Escribir encabezado PPM
    file << "P6\n" << width << " " << height << "\n255\n"; // P6 es el formato binario para PPM

    // Escribir los datos de los píxeles en formato binario
    for (const auto& pixel : pixeles) {
        file.write(reinterpret_cast<const char*>(&pixel.red), sizeof(pixel.red));
        file.write(reinterpret_cast<const char*>(&pixel.green), sizeof(pixel.green));
        file.write(reinterpret_cast<const char*>(&pixel.blue), sizeof(pixel.blue));
    }

    file.close();
}


std::pair<std::vector<Color>, std::unordered_map<Color, int>> readImageAndStoreColors(const std::string& inputFile) {
    std::cout << "Leyendo imagen y almacenando colores de: " << inputFile << std::endl;

    // Abrir archivo en modo binario
    std::ifstream file(inputFile, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo de entrada: " << inputFile << std::endl;
        return {{}, {}}; // Retorna ambos vacíos si falla
    }

    // Leer el encabezado PPM
    std::string format;
    int width, height, maxColorValue;
    file >> format >> width >> height >> maxColorValue;
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignorar el salto de línea
    if (format != "P6" || maxColorValue != 255) {
        std::cerr << "Error: Formato PPM no soportado o valor máximo de color inválido en " << inputFile << std::endl;
        return {{}, {}}; // Retorna ambos vacíos si hay error en formato
    }

    // Usar size_t para cálculos de tamaño
    size_t totalPixels = static_cast<size_t>(width) * static_cast<size_t>(height); // Asegúrate de que ambos sean size_t

    // Preparar el vector de píxeles con capacidad reservada
    std::vector<Color> pixelList;
    pixelList.reserve(totalPixels); // Cambiado a size_t

    // Preparar el mapa de frecuencia de colores
    std::unordered_map<Color, int> colorFrequency;

    // Leer todos los datos de píxeles de una vez en un solo bloque
    std::vector<uint8_t> buffer(totalPixels * 3); // Cambiado a uint8_t y size_t
    file.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(buffer.size())); // Asegúrate de que el tamaño se maneje correctamente

    // Procesar el buffer en bloques de tres bytes para extraer los colores
    for (size_t i = 0; i < buffer.size(); i += 3) {
        Color pixelColor = {buffer[i], buffer[i + 1], buffer[i + 2]};

        // Agregar el color al vector y actualizar la frecuencia
        pixelList.push_back(pixelColor);
        colorFrequency[pixelColor]++;
    }

    file.close();
    std::cout << "Píxeles leídos y frecuencias calculadas" << std::endl;
    return {pixelList, colorFrequency};
}


std::tuple<int, int> getPPMDimensions(const std::string& inputFile) {
    int width = 0, height = 0;
    std::string format;
    int maxColorValue;

    // Abrir archivo de entrada en modo texto
    std::ifstream file(inputFile);
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo de entrada: " << inputFile << std::endl;
        return {width, height}; // Retorna dimensiones 0 si falla
    }

    // Leer el encabezado del archivo PPM
    file >> format >> width >> height >> maxColorValue;

    // Verificar el formato y el valor máximo de color
    if (format != "P6" && format != "P3") { // P3 es el formato ASCII, P6 es binario
        std::cerr << "Error: Formato PPM no soportado en " << inputFile << std::endl;
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
    KDNode* left;
    KDNode* right;
    KDNode(Color c) : color(c), left(nullptr), right(nullptr) {}
};

// Función de distancia euclidiana
/*double distancia_euclidiana(const Color& c1, const Color& c2) {
    return std::sqrt(std::pow(c1.red - c2.red, 2) +
                     std::pow(c1.green - c2.green, 2) +
                     std::pow(c1.blue - c2.blue, 2));
}*/

// Construcción del K-D Tree en 3 dimensiones (RGB)
KDNode* construirKDTree(std::vector<Color>& colores, int depth = 0) {
    if (colores.empty()) return nullptr;

    int axis = depth % 3;
    auto comp = [axis](const Color& a, const Color& b) {
        return (axis == 0) ? a.red < b.red :
               (axis == 1) ? a.green < b.green : a.blue < b.blue;
    };

    // Define `mid` como size_t
    std::vector<Color>::size_type mid = colores.size() / 2;
    
    // Usa nth_element para colocar el mediano en su posición sin ordenar todo el vector
    std::nth_element(colores.begin(), colores.begin() + static_cast<std::vector<Color>::difference_type>(mid), colores.end(), comp);

    // Crear el nodo actual con el elemento mediano
    KDNode* node = new KDNode(colores[mid]);

    // Dividir en sub-vectores usando el mediano como pivote
    std::vector<Color> leftColors(colores.begin(), colores.begin() + static_cast<std::vector<Color>::difference_type>(mid));
    std::vector<Color> rightColors(colores.begin() + static_cast<std::vector<Color>::difference_type>(mid) + 1, colores.end());

    node->left = construirKDTree(leftColors, depth + 1);
    node->right = construirKDTree(rightColors, depth + 1);

    return node;
}

// Función de búsqueda de vecino más cercano
// Función de distancia cuadrada para evitar la raíz cuadrada
double distanciaCuadrada(const Color& a, const Color& b) {
    return (a.red - b.red) * (a.red - b.red) +
           (a.green - b.green) * (a.green - b.green) +
           (a.blue - b.blue) * (a.blue - b.blue);
}

void buscarVecinoMasCercano(KDNode* root, const Color& target, int depth, Color& mejorColor, double& mejorDistanciaCuadrada) {
    if (!root) return;

    // Calcular la distancia cuadrada en lugar de la distancia completa
    double distancia = distanciaCuadrada(target, root->color);
    if (distancia < mejorDistanciaCuadrada) {
        mejorDistanciaCuadrada = distancia;
        mejorColor = root->color;
    }

    // Determinar el eje actual de comparación
    int axis = depth % 3;
    KDNode* siguiente = nullptr;
    KDNode* opuesto = nullptr;

    // Decide el siguiente nodo a explorar
    if ((axis == 0 && target.red < root->color.red) ||
        (axis == 1 && target.green < root->color.green) ||
        (axis == 2 && target.blue < root->color.blue)) {
        siguiente = root->left;
        opuesto = root->right;
    } else {
        siguiente = root->right;
        opuesto = root->left;
    }

    // Búsqueda recursiva en el subárbol relevante
    buscarVecinoMasCercano(siguiente, target, depth + 1, mejorColor, mejorDistanciaCuadrada);

    // Calcular la distancia cuadrada en el eje de partición
    double distanciaEje = (axis == 0) ? (target.red - root->color.red) * (target.red - root->color.red) :
                          (axis == 1) ? (target.green - root->color.green) * (target.green - root->color.green) :
                                        (target.blue - root->color.blue) * (target.blue - root->color.blue);

    // Podar el subárbol opuesto si es posible
    if (distanciaEje < mejorDistanciaCuadrada) {
        buscarVecinoMasCercano(opuesto, target, depth + 1, mejorColor, mejorDistanciaCuadrada);
    }
}

// Función para encontrar el color más cercano para una lista de colores menos frecuentes
std::vector<Color> encontrarColoresCercanos(KDNode* root, const std::vector<Color>& coloresMenosFrecuentes) {
    std::vector<Color> coloresCercanos;
    coloresCercanos.reserve(coloresMenosFrecuentes.size()); // Reservar espacio para evitar redimensionamientos

    for (const auto& color : coloresMenosFrecuentes) {
        Color mejorColor;
        double mejorDistanciaCuadrada = std::numeric_limits<double>::max();
        buscarVecinoMasCercano(root, color, 0, mejorColor, mejorDistanciaCuadrada);
        coloresCercanos.push_back(mejorColor);
    }

    return coloresCercanos;
}


void processCutfreq(const std::string& inputFile, const std::string& outputFile, int numColors) {
    //Leyendo los píxeles es donde más tarda
    auto[pixelList, colorTable] = readImageAndStoreColors(inputFile);
    std::cout << "Número de colores en la imagen: " << colorTable.size() << std::endl;

    // Encontrar los colores menos frecuentes
    std::vector<Color> menos_frecuentes = encontrar_colores_menos_frecuentes(colorTable, numColors);

    // Filtrar colores frecuentes para construir el K-D Tree
    std::unordered_set<Color> menosFrecuentesSet(menos_frecuentes.begin(), menos_frecuentes.end());
    std::vector<Color> coloresRestantes;
    for (const auto& [color, _] : colorTable) {
        if (menosFrecuentesSet.find(color) == menosFrecuentesSet.end()) {
            coloresRestantes.push_back(color);
        }
    }

    // Construir el K-D Tree con los colores restantes
    std::cout << "Construyendo K-D Tree" << std::endl;
    KDNode* kdTreeRoot = construirKDTree(coloresRestantes);

    // Mapear cada color menos frecuente a su color más cercano en el K-D Tree
    std::cout << "Buscando colores más cercanos" << std::endl;
    std::unordered_map<Color, Color> sustituciones;
    for (const auto& color_menos_frecuente : menos_frecuentes) {
        Color colorMasCercano;
        double minDistancia = std::numeric_limits<double>::max();
        buscarVecinoMasCercano(kdTreeRoot, color_menos_frecuente, 0, colorMasCercano, minDistancia);
        sustituciones[color_menos_frecuente] = colorMasCercano;
    }

    //Aquí tarda un buen rato
    // Sustituir cada aparición de un color menos frecuente en la lista de píxeles
    std::cout << "Sustituyendo colores menos frecuentes" << std::endl;
    for (auto& pixel : pixelList) {
        if (sustituciones.find(pixel) != sustituciones.end()) {
            pixel = sustituciones[pixel];
        }
    }

    // Obtener dimensiones de la imagen y guardar la imagen resultante
    auto [width, height] = getPPMDimensions(inputFile);
    escribirPPM(outputFile, pixelList, width, height);
    std::cout << "Imagen modificada escrita en: " << outputFile << std::endl;

    // Liberar memoria del K-D Tree
    // (agregar una función recursiva para liberar nodos si es necesario)
}
