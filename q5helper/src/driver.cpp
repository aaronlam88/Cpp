#include "q5solution.hpp"

int main() {
  try {
    //Test maximum

    //Uncomment the statement below (and comment the load/at/build_NTN_tree
    //  to enter the tree by hand
    //NTN<int> temp = build_NTN_tree("","root");

    //Builds a tree with two children each having 3 children (see print_NTN_tree result)
    int load[] = {1,2,5,3,6,0,4,0,2,0,7,3,4,0,8,0,3,0};
    int at = 0;
    NTN<int> temp = build_NTN_tree(load,at);

    std::cout << "N-ary Tree of ints" << std::endl;
    print_NTN_tree(temp,"");
    std::cout << "Maximum in Tree: " << maximum(temp) << std::endl << std::endl;


    //Test longest_word
    //Preload with the words (enter others as prompted)
    DTN root_DTN;
    add_a_word(root_DTN,"ante");
    add_a_word(root_DTN,"anteater");
    add_a_word(root_DTN,"anthem");
    add_a_word(root_DTN,"antebellum");

    while (true) {
      std::string word = ics::prompt_string("Enter word to add (QUIT to quit)");
      if (word == "QUIT")
        break;
      add_a_word(root_DTN,word);
    }

    std::cout << "Digital Tree of words (marked with *)" << std::endl;
    print_DTN_tree(root_DTN,"");

//    while (true) {
//      std::string word = ics::prompt_string("Enter word to lookup (QUIT to quit)");
//      if (word == "QUIT")
//        break;
//      std::cout << (is_a_word(root_DTN,word) ? "  Present" : "  Absent") << std::endl;
//    }

    std::cout << "longest word = " << longest_word(root_DTN) << std::endl;
  } catch (ics::IcsError& e) {
    std::cout << e.what() << std::endl;
  }
  return 0;
}
