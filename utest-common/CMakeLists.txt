# Executable for all unit tests with list of sources
# Crear el ejecutable para los tests unitarios
add_executable(utest-common
    progargs_test.cpp
    binaryio_test.cpp
    imageinfo_test.cpp
)

# Incluir directorios necesarios
target_include_directories(utest-common
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ../common
)

# Enlazar con las bibliotecas necesarias
target_link_libraries(utest-common
    PRIVATE
        common
        GTest::gtest_main
        Microsoft.GSL::GSL
)

# Registrar el test
add_test(NAME utest-common COMMAND utest-common)
