# Add to this list all files related to imgsoa library
add_library(imgsoa
        imagesoa.hpp
        imagesoa.cpp
        color.hpp
        color.cpp
        command_handlers.cpp
        command_handlers.hpp
)

# Asegurarse de que pueda encontrar progargs.hpp y otras cabeceras en common/
target_include_directories(imgsoa PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/common)

# Use this line only if you have dependencies from this library to GSL
target_link_libraries(imgsoa PRIVATE Microsoft.GSL::GSL)
