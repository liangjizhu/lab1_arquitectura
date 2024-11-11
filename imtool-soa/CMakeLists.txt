# CMakeLists.txt en el directorio imtool-soa
add_executable(imtool-soa main.cpp)

# Asegurarse de que pueda encontrar progargs.hpp y otras cabeceras en common/
target_include_directories(imtool-soa PRIVATE ../common ../imgsoa)

# Enlazar con la biblioteca common si es necesario
target_link_libraries(imtool-soa PRIVATE imgsoa common)
