#include "common/binaryio.hpp"
#include <gtest/gtest.h>
#include <vector>

TEST(BinaryIOTest, ReadBinaryFile_ValidFile) {
    std::vector<uint8_t> data = BinaryIO::readBinaryFile("../input/deer-small.ppm");
    ASSERT_FALSE(data.empty());
}

TEST(BinaryIOTest, ReadBinaryFile_InvalidFile) {
    ASSERT_THROW(BinaryIO::readBinaryFile("nonexistent.ppm"), std::runtime_error);
}
