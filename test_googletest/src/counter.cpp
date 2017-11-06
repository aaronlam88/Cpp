#include <stdio.h>
#include "counter.hpp"

int Counter::Incr() {
  return counter_++;
}

int Counter::value() {
  return counter_;
}
