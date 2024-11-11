# Executable for all unit tests with list of sources
# For example, you may have one *_test.cpp for each *.cpp in common
add_executable(utest-img-soa
        imagesoa_test.cpp
        color_test.cpp
        )

# Library dependencies
target_link_libraries(utest-img-soa
        PRIVATE
        imgsoa
        common
        GTest::gtest_main
        Microsoft.GSL::GSL
)

add_test(NAME utest_img_soa COMMAND ${CMAKE_BINARY_DIR}/utest-img-soa/utest-img-soa)
