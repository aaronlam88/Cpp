#include <list>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "ics46goody.hpp"
#include "iterator.hpp"
#include "array_stack.hpp"
#include "array_queue.hpp"
#include "array_priority_queue.hpp"
#include "array_set.hpp"
#include "array_map.hpp"


int main() {
  try {

    //Example: simple test for queues
    ics::ArrayQueue<std::string> q;
    q.enqueue("a");
    q.enqueue("b");
    q.enqueue("c");

    for (auto x : q)
        std::cout << x << std::endl;
  } catch (ics::IcsError& e) {
    std::cout << e.what() << std::endl;
  }

  return 0;
}
