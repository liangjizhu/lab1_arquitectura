# Add to this list all files related to imgaos library
add_library(imgaos
        imageaos.cpp
        imageaos.hpp
        color.hpp
        color.cpp
)

# Use this line only if you have dependencies from this library to GSL
target_link_libraries(imgaos PRIVATE Microsoft.GSL::GSL)
