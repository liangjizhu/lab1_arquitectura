#!/bin/bash

# Crear el directorio de build si no existe
mkdir -p "build"
cd "build"

# Generar el proyecto en modo Release
cmake -DCMAKE_BUILD_TYPE=Release ..
make
