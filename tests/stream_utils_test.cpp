#include "utils/utils.hpp"
#include <gtest/gtest.h>

TEST(GetStringStreamSize, empty) {
    std::stringstream ss;
    EXPECT_EQ(utils::getStringStreamSize(ss), 0);
}

TEST(GetStringStreamSize, nonEmpty) {
    std::stringstream ss;
    ss << "Hello, world!";
    EXPECT_EQ(utils::getStringStreamSize(ss), 13);
}
