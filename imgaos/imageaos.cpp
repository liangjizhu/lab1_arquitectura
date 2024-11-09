//
// Created by liang on 4/10/24.
//

#include "imageaos.hpp"
#include "binaryio.hpp"
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
std::vector<Color> extractImagePixels(const std::vector<uint8_t>& fileData, const PPMHeader& header) {
    size_t numPixels = static_cast<size_t>(header.width) * static_cast<size_t>(header.height);
    size_t pixelSize = (header.maxColorValue > 255) ? 6 : 3;
    size_t pixelStart = fileData.size() - numPixels * pixelSize;

    std::vector<Color> imagePixels;
    imagePixels.reserve(numPixels);

    for (size_t i = pixelStart; i < fileData.size(); i += pixelSize) {
        imagePixels.push_back(Color::fromBinary(&fileData[i], header));
    }

    return imagePixels;
}

std::pair<std::vector<Color>, std::unordered_map<Color, int>> createColorTable(const std::vector<Color>& imagePixels) {
    std::vector<Color> colorTable;
    colorTable.reserve(256);  // Reserva espacio inicial

    std::unordered_map<Color, int> colorIndex;
    colorIndex.reserve(256);

    for (const auto& pixel : imagePixels) {
        auto [it, inserted] = colorIndex.emplace(pixel, colorTable.size());
        if (inserted) {
            colorTable.push_back(pixel);
        }
    }

    // Ordenar la tabla de colores (en orden RGB)
    std::sort(colorTable.begin(), colorTable.end(), [](const Color& a, const Color& b) {
        return std::tie(a.red, a.green, a.blue) < std::tie(b.red, b.green, b.blue);
    });

    // Actualizar el mapa de índices
    colorIndex.clear();
    colorIndex.reserve(colorTable.size());
    for (size_t i = 0; i < colorTable.size(); ++i) {
        colorIndex[colorTable[i]] = static_cast<int>(i);
    }

    return {colorTable, colorIndex};
}

std::string generateHeader(const PPMHeader& header, int colorTableSize) {
    std::ostringstream headerStream;
    headerStream << "C6 " << header.width << " " << header.height << " " << header.maxColorValue << " " << colorTableSize << "\n";
    return headerStream.str();
}

void appendColorTable(std::vector<uint8_t>& compressedData, const std::vector<Color>& colorTable, const PPMHeader& header) {
    for (const auto& color : colorTable) {
        color.writeToBinary(compressedData, header);  // `writeToBinary` convierte el color a bytes y los añade a `compressedData`
    }
}

void appendPixelIndices(std::vector<uint8_t>& compressedData, const std::vector<Color>& imagePixels, const std::unordered_map<Color, int>& colorIndex) {
    size_t colorIndexSize = static_cast<size_t>((colorIndex.size() <= 256) ? 1 : (colorIndex.size() <= 65536) ? 2 : 4);

    compressedData.reserve(compressedData.size() + imagePixels.size() * colorIndexSize);

    for (const auto& pixel : imagePixels) {
        int index = colorIndex.at(pixel);

        switch (colorIndexSize) {
            case 1:
                compressedData.push_back(static_cast<uint8_t>(index));
                break;
            case 2:
                compressedData.push_back(static_cast<uint8_t>(index & 0xFF));
                compressedData.push_back(static_cast<uint8_t>((index >> 8) & 0xFF));
                break;
            case 4:
                compressedData.push_back(static_cast<uint8_t>(index & 0xFF));
                compressedData.push_back(static_cast<uint8_t>((index >> 8) & 0xFF));
                compressedData.push_back(static_cast<uint8_t>((index >> 16) & 0xFF));
                compressedData.push_back(static_cast<uint8_t>((index >> 24) & 0xFF));
                break;
        }
    }
}

void compressAoS(const std::string& inputFile, std::string outputFile) {
    if (outputFile.find(".cppm") == std::string::npos) {
        outputFile += ".cppm";
    }

    std::vector<uint8_t> fileData = BinaryIO::readBinaryFile(inputFile);
    if (fileData.empty()) {
        std::cerr << "Error: No se pudo abrir o leer el archivo de entrada: " << inputFile << std::endl;
        return;
    }

    PPMHeader header;
    if (!readPPMHeader(inputFile, header)) {
        std::cerr << "Error al leer el encabezado del archivo PPM." << std::endl;
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


