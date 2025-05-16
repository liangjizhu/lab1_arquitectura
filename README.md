# Programación Orientada Al Rendimiento

## Propósito del Proyecto
Este proyecto tiene como objetivo proporcionar herramientas para manipular y procesar imágenes en formato PPM. Las características principales incluyen la compresión de imágenes, ajuste de niveles de color, redimensionamiento de imágenes y corte de frecuencias de color.

## Características del Proyecto
- **Compresión de Imágenes**: Reduce el tamaño de las imágenes en formato PPM.
- **Ajuste de Niveles de Color**: Modifica los niveles de color de las imágenes.
- **Redimensionamiento de Imágenes**: Cambia las dimensiones de las imágenes.
- **Corte de Frecuencias de Color**: Elimina colores menos frecuentes en las imágenes.

## Integrantes
- Liang Ji Zhu -> compress, info
- Victoria Guzmán Clemente -> cutfreq
- Alfonso Pineda Fernández -> maxlevel
- Paolo Webb -> resize

## Dependencias
- CMake 3.22 o superior
- GoogleTest v1.15.2
- Microsoft GSL v4.0.0

### Instalación de Dependencias
Para instalar las dependencias, ejecute los siguientes comandos:

```bash
# Instalar CMake
sudo apt-get install cmake

# Clonar y construir GoogleTest
git clone https://github.com/google/googletest.git
cd googletest
mkdir build
cd build
cmake ..
make
sudo make install

# Clonar y construir Microsoft GSL
git clone https://github.com/microsoft/GSL.git
cd GSL
mkdir build
cd build
cmake ..
make
sudo make install
```

## Ejemplos de Uso
### Compresión de Imágenes
```bash
./imtool-aos in/lake-small.ppm output-aos/compressed.cppm compress
```

### Ajuste de Niveles de Color
```bash
./imtool-aos in/lake-small.ppm output-aos/adjusted.ppm maxlevel 128
```

### Redimensionamiento de Imágenes
```bash
./imtool-aos in/lake-small.ppm output-aos/resized.ppm resize 800 600
```

### Corte de Frecuencias de Color
```bash
./imtool-aos in/lake-small.ppm output-aos/cutfreq.ppm cutfreq 10
```

## Ejecución de los u-tests
- Se asume que hay una carpeta llamada in donde cogeremos el archivo lake-small.pp para realizar pruebas con ella
- Se asume que hay una carpeta llamada output-aos donde se alojan los archivos de salida de las funciones
- Se asume que hay una carpeta llamada output-soa donde se alojan los archivos de salida de las funciones

## Ejecución de los f-tests
- Se asume que hay una carpeta llamada in donde cogeremos el la imagen input para realizar pruebas con ella
- Se asume que hay una carpeta llamada output-aos donde se alojan los archivos de salida de las funciones
- Se asume que hay una carpeta llamada output-soa donde se alojan los archivos de salida de las funciones
- Se asume que hay una carpeta llamada expected-output donde están las imágenes de salida de referencia

## Compilación
- Ha habido una descoordinación en la compilación, debido a que el CMakeLists.txt del root
    no estaba correctamente configurado.
- Se ha comentado cierta parte del código con NOLINT para que se pueda realizar el proceso de compilación.

## Directrices de Contribución
1. Haga un fork del repositorio.
2. Cree una nueva rama para su función o corrección de errores (`git checkout -b feature/nueva-funcion`).
3. Realice sus cambios y confirme sus cambios (`git commit -m 'Añadir nueva función'`).
4. Empuje a la rama (`git push origin feature/nueva-funcion`).
5. Abra una solicitud de extracción.
