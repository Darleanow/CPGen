#include <gtest/gtest.h>

TEST(HelloTest, BasicAssertions) {
  EXPECT_STRNE("hello", "world");

  EXPECT_EQ(1 + 1, 3 - 1);
}
