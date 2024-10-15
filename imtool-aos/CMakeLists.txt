# CMakeLists.txt en el directorio imtool-aos
add_executable(imtool-aos main.cpp)

# Asegurarse de que pueda encontrar progargs.hpp y otras cabeceras en common/
target_include_directories(imtool-aos PRIVATE ../common)

# Enlazar con la biblioteca common si es necesario
target_link_libraries(imtool-aos PRIVATE common)
