#include <string>
#include <iostream>
#include <fstream>
#include "gtest/gtest.h"
#include <vector>
#include <algorithm>    // std::random_shuffle
#include "q2solution.hpp"
//#include <sstream>
//#include "ics46goody.hpp"

class SetTest : public ::testing::Test {
protected:
    virtual void SetUp()    {}
    virtual void TearDown() {}
};


template<class T>
T nth(LN<T>* ll, int n) {
  for (int i=0; i<n; ++i)
    ll = ll->next;
  return ll->value;
}

template<class T>
bool in(LN<T>* ll, T value) {
  for (; ll != nullptr; ll=ll->next)
    if (ll->value == value)
      return true;

  return false;
}

TEST_F(SetTest, relation) {
  ASSERT_EQ('=',relation("",""));
  ASSERT_EQ('>',relation("a",""));
  ASSERT_EQ('<',relation("","a"));
  ASSERT_EQ('=',relation("ant","ant"));
  ASSERT_EQ('<',relation("ant","anteater"));
  ASSERT_EQ('>',relation("anteater","ant"));
  ASSERT_EQ('>',relation("x","ant"));
}


TEST_F(SetTest, add_ordered_i) {
  int* init_values = new int[10]{5, 1, 3, 8, 7, 6, 2, 4, 0, 9};
  std::vector<int> values;
  for (int i=0; i<10; ++i)
    values.push_back(init_values[i]);


  for (int test=1; test<=100; ++test) {
    LN<int>* ll = nullptr;
    for (unsigned i=0; i<values.size(); ++i) {
      add_ordered_i(ll,values[i]);

      //Every value in linked list
      for (unsigned int n=0; n<i+1; ++n)
        ASSERT_TRUE(in(ll,values[n]));
      //Linked list sorted
      for (unsigned int n=0; n<i; ++n)
        ASSERT_LT(nth(ll,n),nth(ll,n+1));
    }
    std::random_shuffle(values.begin(),values.end());
  }
}


TEST_F(SetTest, add_ordered_r) {
  int* init_values = new int[10]{5, 1, 3, 8, 7, 6, 2, 4, 0, 9};
  std::vector<int> values;
  for (int i=0; i<10; ++i)
    values.push_back(init_values[i]);


  for (int test=1; test<=100; ++test) {
    LN<int>* ll = nullptr;
    for (unsigned i=0; i<values.size(); ++i) {
      add_ordered_r(ll,values[i]);

      //Every value in linked list
      for (unsigned int n=0; n<i+1; ++n)
        ASSERT_TRUE(in(ll,values[n]));
      //Linked list sorted
      for (unsigned int n=0; n<i; ++n)
        ASSERT_LT(nth(ll,n),nth(ll,n+1));
    }
    std::random_shuffle(values.begin(),values.end());
  }
}





int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
