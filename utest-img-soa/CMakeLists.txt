# Executable for all unit tests with list of sources
# Crear el ejecutable para los tests unitarios
add_executable(utest-img-soa
    imagesoa_test.cpp
    color_test.cpp
)

# Incluir directorios necesarios
target_include_directories(utest-img-soa
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ../imgsoa
        ../common
)

# Enlazar con las bibliotecas necesarias
target_link_libraries(utest-img-soa
    PRIVATE
        imgsoa
        common
        GTest::gtest_main
        Microsoft.GSL::GSL
)

# Registrar el test
add_test(NAME utest-img-soa COMMAND utest-img-soa)
