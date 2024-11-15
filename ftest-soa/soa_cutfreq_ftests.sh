#!/bin/bash

# Crear los directorios de salida
mkdir -p ../output-soa/cutfreq
mkdir -p ../expected-output/cutfreq

# Función para ejecutar y comparar
cutfreq_and_compare() {
    local input=$1
    local output="../output-soa/cutfreq/$2.ppm"
    local expected_diff=$3

    echo "Ejecutando cutfreq para: $input con frecuencia límite: $expected_diff"

    # Ejecutar la operación cutfreq y capturar la salida
    output_log=$(../build/imtool-soa/imtool-soa "../in/$input" "$output" cutfreq $expected_diff)

    # Verificar si el archivo de salida se generó
    if [ ! -f "$output" ]; then
        echo "Error: El archivo de salida $output no se generó."
        exit 1
    fi

    # Extraer los colores únicos al principio y al final
    initial_colors=$(echo "$output_log" | grep "Colores únicos al principio" | awk '{print $5}')
    final_colors=$(echo "$output_log" | grep "Colores únicos al final" | awk '{print $5}')

    # Calcular la diferencia
    color_diff=$((initial_colors - final_colors))

    # Verificar si la diferencia coincide con el valor esperado
    if [ "$color_diff" -eq "$expected_diff" ]; then
        echo "Los resultados para $input SON los esperados. Diferencia de colores: $color_diff"
    else
        echo "Los resultados para $input NO SON los esperados. Diferencia de colores: $color_diff, se esperaba: $expected_diff"
        exit 1
    fi
}

# Ejecutar y comparar para cada imagen
cutfreq_and_compare "lake-large.ppm" "lake-large" 100000
cutfreq_and_compare "deer-small.ppm" "deer-small" 10000
