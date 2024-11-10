# Add to this list all files related to imgsoa library
# Add to this list all files related to imgaos library
add_library(imgsoa
        imagesoa.cpp
        imagesoa.hpp
        color.hpp
        color.cpp
)

# Asegurarse de que pueda encontrar progargs.hpp y otras cabeceras en common/
target_include_directories(imgsoa PRIVATE ../common)

# Use this line only if you have dependencies from this library to GSL
target_link_libraries(imgsoa PRIVATE common Microsoft.GSL::GSL)
