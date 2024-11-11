#include "common/imageinfo.hpp"
#include <gtest/gtest.h>
#include <vector>

TEST(ImageInfoTest, ReadPPMHeader_ValidHeader) {
    std::string inputFile = "input/lake-small.ppm";
    PPMHeader header;
    ASSERT_TRUE(readPPMHeader(inputFile, header));
    EXPECT_EQ(header.width, 133);
    EXPECT_EQ(header.height, 100);
    EXPECT_EQ(header.maxColorValue, 255);
}

TEST(ImageInfoTest, ReadPPMHeader_InvalidFormat) {
    std::string inputFile = "input/lake-small.ppm";
    PPMHeader header;
    ASSERT_FALSE(readPPMHeader(inputFile, header));
}
