#include <iostream>
#include <sstream>
#include "gtest/gtest.h"
#include "Linear_Array_Queue.hpp"

typedef ics::LinearArrayQueue<std::string> QueueType;

class QueueTest : public ::testing::Test {
protected:
    virtual void SetUp()    {}
    virtual void TearDown() {}
};


TEST_F(QueueTest, empty) {
  QueueType q;
  ASSERT_TRUE(q.empty());
}


TEST_F(QueueTest, size) {
  QueueType q;
  ASSERT_EQ(0,q.size());
}


TEST_F(QueueTest, enqueue) {
  QueueType q;
  q.enqueue("a");
  ASSERT_FALSE(q.empty());
  ASSERT_EQ(1, q.size());

  q.enqueue("b");
  ASSERT_FALSE(q.empty());
  ASSERT_EQ(2, q.size());

  q.enqueue("c");
  ASSERT_FALSE(q.empty());
  ASSERT_EQ(3, q.size());
}


TEST_F(QueueTest, enqueue_iterator_param) {
  QueueType q,q1;
  q1.enqueue("a");
  q1.enqueue("b");
  q1.enqueue("c");
  q.enqueue(q1.ibegin(),q1.iend());
  ASSERT_FALSE(q.empty());
  ASSERT_EQ(3, q.size());
  ASSERT_EQ(q,q1);
}


TEST_F(QueueTest, peek) {
  QueueType q;
  ASSERT_THROW(q.peek(),ics::EmptyError);

  q.enqueue("a");
  ASSERT_EQ("a", q.peek());

  q.enqueue("b");
  ASSERT_EQ("a", q.peek());

  q.enqueue("c");
  ASSERT_EQ("a", q.peek());
}


TEST_F(QueueTest, operator_eq_ne) { //== and !=
  QueueType q,q1;
  ASSERT_EQ(q,q1);

  q.enqueue("a");
  ASSERT_NE(q,q1);

  q.enqueue("b");
  ASSERT_NE(q,q1);

  q.enqueue("c");
  ASSERT_NE(q,q1);

  q1.enqueue("a");
  ASSERT_NE(q,q1);

  q1.enqueue("b");
  ASSERT_NE(q,q1);

  q1.enqueue("c");
  ASSERT_EQ(q,q1);
  ASSERT_EQ(q,q);
  ASSERT_EQ(q1,q1);
}


TEST_F(QueueTest, operator_stream_insert) { //<<
  std::ostringstream value;
  QueueType q;
  value << q;
  ASSERT_EQ("queue[]:rear", value.str());

  value.str("");
  q.enqueue("a");
  value << q;
  ASSERT_EQ("queue[a]:rear", value.str());

  value.str("");
  q.enqueue("b");
  value << q;
  ASSERT_EQ("queue[a,b]:rear", value.str());

  value.str("");
  q.enqueue("c");
  value << q;
  ASSERT_EQ("queue[a,b,c]:rear", value.str());
}


TEST_F(QueueTest, clear) {
  QueueType q;
  q.enqueue("a");
  q.enqueue("b");
  q.enqueue("c");
  q.clear();
  ASSERT_TRUE(q.empty());
  ASSERT_EQ(0, q.size());
}


TEST_F(QueueTest, dequeue1) {
  QueueType q;
  q.enqueue("a");
  q.enqueue("b");
  q.enqueue("c");
  ASSERT_EQ("a",q.dequeue());
  ASSERT_EQ("b",q.dequeue());
  ASSERT_EQ("c",q.dequeue());
  ASSERT_TRUE(q.empty());
  ASSERT_EQ(0, q.size());
}


TEST_F(QueueTest, dequeue2) {
  QueueType q;
  q.enqueue("a");
  q.enqueue("b");
  q.enqueue("c");
  ASSERT_EQ("a",q.dequeue());
  ASSERT_FALSE(q.empty());
  ASSERT_EQ(2, q.size());
  ASSERT_EQ("b", q.peek());

  ASSERT_EQ("b",q.dequeue());
  ASSERT_FALSE(q.empty());
  ASSERT_EQ(1, q.size());
  ASSERT_EQ("c", q.peek());

  ASSERT_EQ("c",q.dequeue());
  ASSERT_TRUE(q.empty());
  ASSERT_EQ(0, q.size());
  ASSERT_THROW(q.peek(),ics::EmptyError);
}


TEST_F(QueueTest, assignment) {
  QueueType q,q1;
  q1.enqueue("a");
  q1.enqueue("b");
  q1.enqueue("c");
  q = q1;
  ASSERT_EQ(q,q1);
}

TEST_F(QueueTest, iterator_simple) {
  QueueType q;
  q.enqueue("a");
  q.enqueue("b");
  q.enqueue("c");

  std::string qvalues[] ={"a","b","c"};
  int i = 0;
  for (std::string x : q)
    ASSERT_EQ(qvalues[i++],x);

  i = 0;
  for (ics::Iterator<std::string>& it = q.ibegin(); it != q.iend(); ++it)
    ASSERT_EQ(qvalues[i++],*it);

  for (ics::Iterator<std::string>& it = q.ibegin(); it != q.iend(); ++it)
    it.erase();
  ASSERT_TRUE(q.empty());
  ASSERT_EQ(0,q.size());
}

//assertions for other iterator commands to be added later!


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
