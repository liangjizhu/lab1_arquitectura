//
// Created by liang on 4/10/24.
//

#include "imageaos.hpp"
#include "color.hpp"

#include <iostream>
#include <fstream>
#include <cmath>
#include <unordered_map>
#include <array>
#include <vector>
#include <algorithm>

void processInfo(const std::string& inputFile) {
    std::ifstream file(inputFile, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open input file: " << inputFile << std::endl;
        return;
    }

    std::string format;
    int width, height, maxColorValue;

    // Leer el formato de la imagen (e.g., P3 o P6 para PPM)
    file >> format;
    if (format != "P3" && format != "P6") {
        std::cerr << "Error: Unsupported PPM format. Only P3 and P6 are supported." << std::endl;
        return;
    }

    // Leer ancho, alto y valor máximo de color
    file >> width >> height >> maxColorValue;

    // Mostrar metadatos de la imagen
    std::cout << "File: " << inputFile << std::endl;
    std::cout << "Format: " << format << std::endl;
    std::cout << "Width: " << width << std::endl;
    std::cout << "Height: " << height << std::endl;
    std::cout << "Max Color Value: " << maxColorValue << std::endl;

    file.close();
}

void processMaxLevel(const std::string& inputFile, int maxLevel) {
    // Lógica para el comando 'maxlevel'
    std::cout << "Processing 'maxlevel' for file: " << inputFile << " with max level: " << maxLevel << std::endl;
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

void processCutfreq(int numColors, std::vector<Color>& pixeles) {
    //Determinar la frecuencia absoluta de cada color
    std::unordered_map<Color, int> frecuencia;
    frecuencia = contar_frecuencia(pixeles);

    //Encontrar los colores menos frecuentes y ordenarlos
    std::vector<Color> menos_frecuentes;
    menos_frecuentes = encontrar_colores_menos_frecuentes(frecuencia, numColors);

    //Calcular la distancia euclídea con los demás colores


}