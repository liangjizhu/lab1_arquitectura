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


// COLOR IMPLEMENTADO EN COLOR.HPP
//FUNCIÓN PARA CALCULAR LA DISTANCIA EUCLÍDEA
double distancia_euclidiana(const Color& c1, const Color& c2) {
    return std::pow(c1.red - c2.red, 2) +
                     std::pow(c1.green - c2.green, 2) +
                     std::pow(c1.blue - c2.blue, 2);
}


//FUNCIÓN PARA CONTAR LA FRECUENCIA DE CADA COLOR
//Un unordered_map es como un diccionario pero su tiempo de búsqueda es O(1), y si se repite la clave, se sobreescribe
//Paso el vector por referencia para no hacer una copia y así ser más eficiente
std::unordered_map<Color, int> contar_frecuencia(const std::vector<Color>& pixeles) {
    
    std::unordered_map<Color, int> frecuencia;

    for (const auto& pixel : pixeles) {
        frecuencia[pixel]++;
    }

    return frecuencia;
}

// FUNCIÓN PARA ENCONTRAR LOS COLORES MENOS FRECUENTES
// Para encontrar los colores menos frecuentes necesitamos que el unordered map pase a ser un vector de pares, ya que los vectores son ordenables
// Ordenaremos el vector de menor a mayor frecuencia y seleccionaremos los primeros 'n' colores

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

// void distancias_euclideas(std::vector<Color>& menos_frecuentes, std::vector<Color>& pixeles) {
    //Calcular la distancia euclídea con los demás colores
    // for (const auto& pixel : pixeles) {
    //    for (const auto& color : menos_frecuentes) {
            // double distancia = distancia_euclidiana(pixel, color);
            // Hacer algo con la distancia
    //    }
    // }
// }

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

//Esta función lee una imagen PPM y almacena los colores en una tabla de colores, que es un unordered_map cuyos valores son un color
//y su frecuencia de aparición
std::pair<std::vector<Color>, std::unordered_map<Color, int>> readImageAndStoreColors(const std::string& inputFile) {
    std::cout << "Leyendo imagen y almacenando colores de: " << inputFile << std::endl;
    // Vector para guardar los píxeles en orden
    std::vector<Color> pixelList;

    // Map para guardar la frecuencia de cada color
    std::unordered_map<Color, int> colorFrequency;

    // Abrir archivo de entrada en modo binario
    std::ifstream file(inputFile, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo de entrada: " << inputFile << std::endl;
        return {pixelList, colorFrequency}; // Retorna ambos vacíos si falla
    }
    std::cout << "Archivo abierto" << std::endl;
    std::string format;
    int width, height, maxColorValue;

    // Leer el encabezado del archivo PPM (suponiendo formato P6)
    file >> format >> width >> height >> maxColorValue;
    std ::cout << "Formato: " << format << " Ancho: " << width << " Alto: " << height << " Máximo valor de color: " << maxColorValue << std::endl;
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignorar el salto de línea después del encabezado
    std::cout << "Ignorado salto de línea" << std::endl;
    if (format != "P6" || maxColorValue <= 0 || maxColorValue > 65535) {
        std::cerr << "Error: Formato PPM no soportado o valor máximo de color inválido en " << inputFile << std::endl;
        return {pixelList, colorFrequency}; // Retorna ambos vacíos si hay error en formato
    }

    // Leer píxeles de la imagen
    std::cout << "Leyendo píxeles de la imagen" << std::endl;
    for (int i = 0; i < width * height; ++i) {
        //std::cout << "Leyendo píxel " << i << std::endl;
        uint8_t r, g, b;
        file.read(reinterpret_cast<char*>(&r), 1);
        file.read(reinterpret_cast<char*>(&g), 1);
        file.read(reinterpret_cast<char*>(&b), 1);

        Color pixelColor = {r, g, b};

        // Guardar píxel en el vector en el orden de lectura
        pixelList.push_back(pixelColor);

        // Incrementar la frecuencia en el mapa de frecuencias
        colorFrequency[pixelColor]++;
    }
    std::cout << "Píxeles leídos" << std::endl;
    file.close();
    std ::cout << "Archivo cerrado" << std::endl;
    return {pixelList, colorFrequency};  // Retornar ambos valores
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

//ESTO ESTÁ MAL, TIENE QUE COGER UN STD::STR Y PASARLO A VECTOR DE COLORES
void processCutfreq(const std::string& inputFile, const std::string& outputFile, int numColors) {
    //Determinar la frecuencia absoluta de cada color
    std::cout << "Procesando 'cutfreq' para archivo: " << inputFile << " con " << numColors << " colores a eliminar" << std::endl;
    auto[pixelList, colorTable] = readImageAndStoreColors(inputFile);
    std::cout << "Número de colores en la imagen: " << colorTable.size() << std::endl;
    //Encontrar los colores menos frecuentes y ordenarlos
    std::vector<Color> menos_frecuentes  = encontrar_colores_menos_frecuentes(colorTable, numColors);
    std::cout << "Colores menos frecuentes: " << menos_frecuentes.size() << std::endl;
    std::unordered_map<Color, Color> sustituciones;
    std::unordered_set<Color> menos_frecuentes_set(menos_frecuentes.begin(), menos_frecuentes.end());

    for (const auto& color_menos_frecuente : menos_frecuentes) {
        double min_distancia = std::numeric_limits<double>::max();
        Color color_mas_parecido;

        for (const auto& entry : colorTable) {
            const Color& color = entry.first;
            if (menos_frecuentes_set.find(color) == menos_frecuentes_set.end()) {
                double distancia = distancia_euclidiana(color_menos_frecuente, color);
                if (distancia < min_distancia) {
                    min_distancia = distancia;
                    color_mas_parecido = color;
                }
            }
        }
        sustituciones[color_menos_frecuente] = color_mas_parecido;
    }
    std::cout << "Mapa de sustituciones:\n";
    for (const auto& pair : sustituciones) {
        std::cout << "Color menos frecuente:" << pair.first << " será sustituido por: " << pair.second << std::endl;
    }
    // Paso 5: Sustituir cada aparición de un color menos frecuente en pixeles por su color más cercano
    for (auto& pixel : pixelList) {
        if (sustituciones.find(pixel) != sustituciones.end()) {
            std::cout << "Reemplazando pixel " << pixel << " por " << sustituciones[pixel] << std::endl;
            pixel = sustituciones[pixel];
        }
    }
    auto [width, height] = getPPMDimensions(inputFile);
    // Escribir la imagen modificada
    escribirPPM(outputFile, pixelList, width, height);
    std::cout << "Imagen modificada escrita en: " << outputFile << std::endl;

    auto[pixelList2, colorTable2] = readImageAndStoreColors(outputFile);
    std::cout << "Número de colores en la imagen: " << colorTable2.size() << std::endl;
}

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

