# Add to this list all files related to common library
add_library(common
        progargs.cpp
        binaryio.cpp
        ../utest-common/progargs_test.cpp
        ../utest-common/binaryio_test.cpp
        ../utest-img-aos/imageaos_test.cpp
        ../utest-img-soa/imagesoa_test.cpp
)

# Use this line only if you have dependencies from this library to GSL
target_link_libraries(common PRIVATE Microsoft.GSL::GSL)

# Include the current directory to find the headers
target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
