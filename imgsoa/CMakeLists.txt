# Add to this list all files related to imgsoa library
add_library(imgsoa
        imagesoa.hpp
        imagesoa.cpp
        color.hpp
        color.cpp
)

# Asegurarse de que pueda encontrar progargs.hpp y otras cabeceras en common/
target_include_directories(imgsoa PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ../common)

# Use this line only if you have dependencies from this library to GSL
target_link_libraries(common PRIVATE Microsoft.GSL::GSL)
