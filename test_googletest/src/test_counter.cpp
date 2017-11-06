#include <iostream>
using namespace std;
#include "gtest/gtest.h"
#include "counter.hpp"


TEST(Counter, Increment) {
  Counter c;
  EXPECT_EQ(0, c.Incr());
  EXPECT_EQ(1, c.Incr());
  EXPECT_EQ(2, c.Incr());
}


TEST(Counter2, value) {
  Counter c;
  c.Incr();

  EXPECT_EQ(1, c.value());
  EXPECT_EQ(2, c.value()); //This test should fail
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}


