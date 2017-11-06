//#include <string>
//#include <iostream>
//#include <fstream>
//#include <vector>
//#include <algorithm>    // std::random_shuffle
//#include "q2solution.hpp"
//
//void test_relation(const std::string& s1, const std::string& s2) {
//  std::cout << "  \"" + s1 + "\" " <<  relation(s1,s2) << " \"" + s2 + "\"" << std::endl;
//}
//
//int main() {
//  try{
//    //Testing relation
//    std::cout << "Testing relation" << std::endl;
//    test_relation("","");
//    test_relation("a","");
//    test_relation("","a");
//    test_relation("ant","ant");
//    test_relation("ant","anteater");
//    test_relation("anteater","ant");
//    test_relation("x","ant");
//
//
//    int* init_values = new int[10]{5, 1, 3, 8, 7, 6, 2, 4, 0, 9};
//    std::vector<int> values;
//    for (int i=0; i<10; ++i)
//      values.push_back(init_values[i]);
//
//
//    //Testing add_ordered_i
//    std::cout << "\nTesting add_ordered_i" << std::endl;
//
//    for (int test=1; test<=3; ++test) {
//      std::cout << "Test " << test << std::endl;
//      LN<int>* ll = nullptr;
//      for (unsigned i=0; i<values.size(); ++i) {
//        add_ordered_i(ll,values[i]);
//        std::cout << "  add " << values[i] << ": " << ll << std::endl;
//      }
//      std::random_shuffle(values.begin(),values.end());
//    }
//
//
//    //Testing add_ordered_r
//    values.clear();
//    for (int i=0; i<10; ++i)
//      values.push_back(init_values[i]);
//    std::cout << "\nTesting add_ordered_r" << std::endl;
//    for (int test=1; test<=3; ++test) {
//      std::cout << "Test " << test << std::endl;
//      LN<int>* ll = nullptr;
//      for (unsigned i=0; i<values.size(); ++i) {
//        add_ordered_r(ll,values[i]);
//        std::cout << "  add " << values[i] << ": " << ll << std::endl;
//      }
//      std::random_shuffle(values.begin(),values.end());
//    }
//
//
//  } catch (const std::exception& e) {
//    std::cout << e.what() << std::endl;
//  }
//
//  return 0;
//}
//
//
//
///*
//Here is the output that the driver should produce (although Tests 2-3 are random:
//Testing relation
//  "" = ""
//  "a" > ""
//  "" < "a"
//  "ant" = "ant"
//  "ant" < "anteater"
//  "anteater" > "ant"
//  "x" > "ant"
//
//Testing add_ordered_i
//Test 1
//  add 5: 5->nullptr
//  add 1: 1->5->nullptr
//  add 3: 1->3->5->nullptr
//  add 8: 1->3->5->8->nullptr
//  add 7: 1->3->5->7->8->nullptr
//  add 6: 1->3->5->6->7->8->nullptr
//  add 2: 1->2->3->5->6->7->8->nullptr
//  add 4: 1->2->3->4->5->6->7->8->nullptr
//  add 0: 0->1->2->3->4->5->6->7->8->nullptr
//  add 9: 0->1->2->3->4->5->6->7->8->9->nullptr
//Test 2
//  add 0: 0->nullptr
//  add 1: 0->1->nullptr
//  add 9: 0->1->9->nullptr
//  add 3: 0->1->3->9->nullptr
//  add 5: 0->1->3->5->9->nullptr
//  add 6: 0->1->3->5->6->9->nullptr
//  add 4: 0->1->3->4->5->6->9->nullptr
//  add 8: 0->1->3->4->5->6->8->9->nullptr
//  add 7: 0->1->3->4->5->6->7->8->9->nullptr
//  add 2: 0->1->2->3->4->5->6->7->8->9->nullptr
//Test 3
//  add 4: 4->nullptr
//  add 5: 4->5->nullptr
//  add 2: 2->4->5->nullptr
//  add 8: 2->4->5->8->nullptr
//  add 3: 2->3->4->5->8->nullptr
//  add 0: 0->2->3->4->5->8->nullptr
//  add 1: 0->1->2->3->4->5->8->nullptr
//  add 7: 0->1->2->3->4->5->7->8->nullptr
//  add 6: 0->1->2->3->4->5->6->7->8->nullptr
//  add 9: 0->1->2->3->4->5->6->7->8->9->nullptr
//
//Testing add_ordered_i
//Test 1
//  add 5: 5->nullptr
//  add 1: 1->5->nullptr
//  add 3: 1->3->5->nullptr
//  add 8: 1->3->5->8->nullptr
//  add 7: 1->3->5->7->8->nullptr
//  add 6: 1->3->5->6->7->8->nullptr
//  add 2: 1->2->3->5->6->7->8->nullptr
//  add 4: 1->2->3->4->5->6->7->8->nullptr
//  add 0: 0->1->2->3->4->5->6->7->8->nullptr
//  add 9: 0->1->2->3->4->5->6->7->8->9->nullptr
//Test 2
//  add 6: 6->nullptr
//  add 2: 2->6->nullptr
//  add 9: 2->6->9->nullptr
//  add 8: 2->6->8->9->nullptr
//  add 3: 2->3->6->8->9->nullptr
//  add 1: 1->2->3->6->8->9->nullptr
//  add 0: 0->1->2->3->6->8->9->nullptr
//  add 7: 0->1->2->3->6->7->8->9->nullptr
//  add 5: 0->1->2->3->5->6->7->8->9->nullptr
//  add 4: 0->1->2->3->4->5->6->7->8->9->nullptr
//Test 3
//  add 6: 6->nullptr
//  add 9: 6->9->nullptr
//  add 7: 6->7->9->nullptr
//  add 4: 4->6->7->9->nullptr
//  add 0: 0->4->6->7->9->nullptr
//  add 1: 0->1->4->6->7->9->nullptr
//  add 3: 0->1->3->4->6->7->9->nullptr
//  add 2: 0->1->2->3->4->6->7->9->nullptr
//  add 8: 0->1->2->3->4->6->7->8->9->nullptr
//  add 5: 0->1->2->3->4->5->6->7->8->9->nullptr
//
//*/
