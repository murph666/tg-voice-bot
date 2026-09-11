#include <gtest/gtest.h>

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
// Expect two strings not to be equal.
    EXPECT_STRNE("hello", "world");
}

TEST(HelloTest2, BasicAssertions) {
// Expect equality.
    EXPECT_EQ(7 * 6, 42);
}