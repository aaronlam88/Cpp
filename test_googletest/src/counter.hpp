#ifndef COUNTER_HPP_
#define COUNTER_HPP_

class Counter {
  private:
    int counter_;
  public:
    Counter() : counter_(0) {}
      int Incr();
      int value();
};

#endif /* COUNTER_HPP_ */
