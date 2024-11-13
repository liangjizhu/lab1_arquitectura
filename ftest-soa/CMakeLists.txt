# CMakeLists.txt en el directorio ftest-soa
add_executable(ftests-soa
    imtool_soa_ftest.cpp
)

# Incluir directorios y enlazar bibliotecas
target_include_directories(ftests-soa PRIVATE ../common ../imgaos)
target_link_libraries(ftests-soa
    PRIVATE
    imgaos
    common
    GTest::gtest_main
    Microsoft.GSL::GSL
)

# Agregar las pruebas
add_test(NAME ftests-soa COMMAND ${CMAKE_BINARY_DIR}/ftest-soa/ftests-soa)
set_tests_properties(ftests-soa PROPERTIES WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
