#!/bin/bash

# Crear los directorios de salida
mkdir -p ../output-aos/maxlevel
mkdir -p ../expected-output/maxlevel

# Función para ejecutar y comparar
maxlevel_and_compare() {
    local input=$1
    local output="../output-aos/maxlevel/$2.cppm"
    local expected="../expected-output/maxlevel/$2.cppm"
    local level=$3

    echo "Ejecutando compresión para: $input"

    # Ejecutar la modificación de intensidad
    ../build/imtool-aos/imtool-aos "../in/$input" "$output" maxlevel "$level"

    # Verificar si el archivo de salida se generó
    if [ ! -f "$output" ]; then
        echo "Error: El archivo de salida $output no se generó."
        exit 1
    fi

    # Comparar los archivos de salida con los esperados
    if cmp -s "$output" "$expected"; then
        echo "Los resultados para $input SON los esperados"
    else
        echo "Los resultados para $input NO SON los esperados"
        exit 1
    fi
}

# Ejecutar y comparar para cada imagen
maxlevel_and_compare "deer-small.ppm" "deer-small"
maxlevel_and_compare "lake-small.ppm" "lake-small"
