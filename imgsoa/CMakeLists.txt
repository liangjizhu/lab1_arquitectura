# Add to this list all files related to imgsoa library
add_library(imgsoa
        imagesoa.cpp
)

# Asegurarse de que pueda encontrar progargs.hpp y otras cabeceras en common/
target_include_directories(imgsoa PRIVATE ../common)

# Use this line only if you have dependencies from this library to GSL
target_link_libraries(common PRIVATE Microsoft.GSL::GSL)
