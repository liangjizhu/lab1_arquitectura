#!/bin/bash

# Crear los directorios de salida
mkdir -p ../output-soa/resize
mkdir -p ../expected-output/resize

# Función para ejecutar y comparar
resize_and_compare_soa() {
    local input=$1
    local output="../output-soa/resize/$2.ppm"
    local expected_width=$3
    local expected_height=$4
    echo "Ejecutando resize para: $input con tamaño esperado: ${expected_width}x${expected_height}"

    # Ejecutar la operación resize SOA y capturar la salida
    output_log=$(../build/imtool-soa/imtool-soa "../in/$input" "$output" resize $expected_width $expected_height)

    # Verificar si el archivo de salida se generó
    if [ ! -f "$output" ]; then
        echo "Error: El archivo de salida $output no se generó."
        exit 1
    fi

    # Obtener el tamaño de la imagen de salida
    output_size=$(identify -format "%wx%h" "$output")

    # Verificar si el tamaño coincide con el esperado
    if [ "$output_size" == "${expected_width}x${expected_height}" ]; then
        echo "Los resultados para $input SON los esperados. Tamaño de la imagen: $output_size"
    else
        echo "Los resultados para $input NO SON los esperados. Tamaño de la imagen: $output_size, se esperaba: ${expected_width}x${expected_height}"
        exit 1
    fi
}

# Ejecutar y comparar para cada imagen
resize_and_compare_soa "lake-large.ppm" "lake-large-resized-100" "100" "100"
resize_and_compare_soa "deer-small.ppm" "deer-small-resized-1000" "1000" "1000"
resize_and_compare_soa "lake-small.ppm" "lake-small-resized-100" "100" "100"
resize_and_compare_soa "deer-large.ppm" "deer-large-resized-1000" "1000" "1000"