# Add to this list all files related to imgaos library
add_library(imgaos
        imageaos.hpp
        imageaos.cpp
        color.hpp
        color.cpp
)

# Asegurarse de que pueda encontrar progargs.hpp y otras cabeceras en common/
target_include_directories(imgaos PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/common)

# Use this line only if you have dependencies from this library to GSL
target_link_libraries(imgaos PRIVATE common Microsoft.GSL::GSL)