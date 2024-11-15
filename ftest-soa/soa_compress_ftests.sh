#!/bin/bash

# Crear los directorios de salida
mkdir -p ../output-soa/compress
mkdir -p ../expected-output/compress

# Función para ejecutar y comparar
compress_and_compare() {
    local input=$1
    local output="../output-soa/compress/$2.cppm"
    local expected="../expected-output/compress/$2.cppm"

    echo "Ejecutando compresión para: $input"

    # Ejecutar la compresión
    ../build/imtool-soa/imtool-soa "../in/$input" "$output" compress

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
compress_and_compare "deer-small.ppm" "deer-small"
compress_and_compare "lake-small.ppm" "lake-small"
