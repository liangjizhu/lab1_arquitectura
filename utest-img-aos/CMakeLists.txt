# Executable for all unit tests with list of sources
# Crear el ejecutable para los tests unitarios
add_executable(utest-img-aos
    imageaos_test.cpp
    color_test.cpp
)

# Incluir directorios necesarios
target_include_directories(utest-img-aos
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ../imgaos
        ../common
)

# Enlazar con las bibliotecas necesarias
target_link_libraries(utest-img-aos
    PRIVATE
        common
        imgaos
        GTest::gtest_main
        Microsoft.GSL::GSL
)

# Registrar el test
add_test(NAME utest-img-aos COMMAND utest-img-aos)
