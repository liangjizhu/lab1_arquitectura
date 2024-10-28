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


// ******* COLOR IMPLEMENTADO EN COLOR.HPP *******
//FUNCIÓN PARA CALCULAR LA DISTANCIA EUCLÍDEA
double distancia_euclidiana(const Color& c1, const Color& c2) {
    return std::sqrt(std::pow(c1.red - c2.red, 2) +
                     std::pow(c1.green - c2.green, 2) +
                     std::pow(c1.blue - c2.blue, 2));
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

std::vector<Color> encontrar_colores_menos_frecuentes(const std::unordered_map<Color, int>& frecuencia, long unsigned int n) {
    
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

//void processCutfreq(int numColors, std::vector<Color>& pixeles) {
    ////Determinar la frecuencia absoluta de cada color
    //std::unordered_map<Color, int> frecuencia;
    //frecuencia = contar_frecuencia(pixeles);

    ////Encontrar los colores menos frecuentes y ordenarlos
    //std::vector<Color> menos_frecuentes;
    //menos_frecuentes = encontrar_colores_menos_frecuentes(frecuencia, numColors);

    ////Calcular la distancia euclídea con los demás colores
//}

// TODO
// CORREGIR TABLA DE COLORES
void compressAoS(const std::string& inputFile, std::string outputFile) {
    // Asegurarse de que el archivo de salida tenga extensión .cppm
    if (outputFile.find(".cppm") == std::string::npos) {
        outputFile += ".cppm";
    }

    // Abrir archivo de entrada en modo binario
    std::ifstream file(inputFile, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo de entrada: " << inputFile << std::endl;
        return;
    }

    std::string format;
    int width, height, maxColorValue;

    // Leer el encabezado del archivo PPM
    file >> format >> width >> height >> maxColorValue;
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignorar el salto de línea después del encabezado

    if (format != "P6" || maxColorValue <= 0 || maxColorValue > 65535) {
        std::cerr << "Error: Formato PPM no soportado o valor máximo de color inválido en " << inputFile << std::endl;
        return;
    }

    // Leer píxeles de la imagen
    std::vector<Color> imagePixels;
    if (maxColorValue <= 255) {
        // Leer cada píxel como 3 bytes
        for (int i = 0; i < width * height; ++i) {
            uint8_t r, g, b;
            file.read(reinterpret_cast<char*>(&r), 1);
            file.read(reinterpret_cast<char*>(&g), 1);
            file.read(reinterpret_cast<char*>(&b), 1);
            imagePixels.emplace_back(r, g, b);
        }
    } else {
        // Leer cada píxel como 6 bytes (2 bytes por canal de color en little-endian)
        for (int i = 0; i < width * height; ++i) {
            uint16_t r, g, b;
            file.read(reinterpret_cast<char*>(&r), 2);
            file.read(reinterpret_cast<char*>(&g), 2);
            file.read(reinterpret_cast<char*>(&b), 2);
            imagePixels.emplace_back(r, g, b);
        }
    }
    file.close();

    // Crear tabla de colores y mapa de índices
    std::vector<Color> colorTable;
    std::unordered_map<Color, int> colorIndex;
    for (const auto& pixel : imagePixels) {
        if (colorIndex.find(pixel) == colorIndex.end()) {
            colorIndex[pixel] = static_cast<int>(colorTable.size());
            colorTable.push_back(pixel);
        }
    }

    // Abrir archivo de salida en modo binario
    std::ofstream outFile(outputFile, std::ios::binary);
    outFile << "C6 " << width << " " << height << " " << maxColorValue << " " << colorTable.size() << "\n";

    // Escribir la tabla de colores en formato binario
    for (const auto& color : colorTable) {
        if (maxColorValue <= 255) {
            outFile.put(static_cast<char>(color.red));
            outFile.put(static_cast<char>(color.green));
            outFile.put(static_cast<char>(color.blue));
        } else {
            uint16_t redLE = htole16(color.red);  // Convertir a little-endian
            uint16_t greenLE = htole16(color.green);
            uint16_t blueLE = htole16(color.blue);
            outFile.write(reinterpret_cast<const char*>(&redLE), 2);
            outFile.write(reinterpret_cast<const char*>(&greenLE), 2);
            outFile.write(reinterpret_cast<const char*>(&blueLE), 2);
        }
    }

    // Determinar el tamaño de índice necesario para los píxeles
    int colorIndexSize = (colorTable.size() <= 256) ? 1 : (colorTable.size() <= 65536) ? 2 : 4;

    // Escribir los índices de píxeles en formato binario
    for (const auto& pixel : imagePixels) {
        int index = colorIndex[pixel];
        if (colorIndexSize == 1) {
            uint8_t index8 = static_cast<uint8_t>(index);
            outFile.write(reinterpret_cast<const char*>(&index8), 1);
        } else if (colorIndexSize == 2) {
            uint16_t index16 = htole16(static_cast<uint16_t>(index));
            outFile.write(reinterpret_cast<const char*>(&index16), 2);
        } else if (colorIndexSize == 4) {
            uint32_t index32 = htole32(static_cast<uint32_t>(index));
            outFile.write(reinterpret_cast<const char*>(&index32), 4);
        }
    }

    outFile.close();
}
