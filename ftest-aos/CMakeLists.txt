# CMakeLists.txt en el directorio ftest-aos
add_executable(ftests-aos
    imtool_aos_ftest.cpp
)

# Incluir directorios y enlazar bibliotecas
target_include_directories(ftests-aos PRIVATE ../common ../imgaos)
target_link_libraries(ftests-aos
    PRIVATE
    imgaos
    common
    GTest::gtest_main
    Microsoft.GSL::GSL
)

# Agregar las pruebas
add_test(NAME ftests-aos COMMAND ${CMAKE_BINARY_DIR}/ftest-aos/ftests-aos)
set_tests_properties(ftests-aos PROPERTIES WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
