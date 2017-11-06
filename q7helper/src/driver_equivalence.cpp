//#include <string>
//#include <iostream>
//#include <fstream>
//#include "set.hpp"
//#include "ics46goody.hpp"
//#include "ics_exceptions.hpp"
//#include "array_set.hpp"
//#include "array_equivalence.hpp"
//
//
//int main() {
//    ics::ArrayEquivalence<std::string> e;
//
//    std::cout << "Enter singletons first; then other operations are allowed." << std::endl;
//    for (;;) try {
//
//      std::string x = ics::prompt_string("  Enter string for singleton (QUIT to quit)");
//      if (x == "QUIT")
//        break;
//      e.add_singleton(x);
//    } catch (ics::IcsError& e) {
//      std::cout << "  " << e.what() << std::endl;
//    }
//
//    for (;;) try {
//      std::cout << std::endl;
//      std::cout << "Accessors         Mutators             General"         << std::endl;
//      std::cout << "  c  - classes      i - in same class    s - show maps" << std::endl;
//      std::cout << "  1  - size         m - merge classes    q - quit"      << std::endl;
//      std::cout << "  2  - class count"                                     << std::endl;
//      std::string allowable[] = {"c","1","2","i","m","s","q",""};
//      std::string op = ics::prompt_string("\nEnter equivalence operation","",allowable);
//
//      if (op == "s") {
//        e.show_equivalence();
//      }else if (op == "c") {
//        std::cout << "  classes() = " << e.classes() << std::endl;
//      }else if (op == "1") {
//        std::cout << "  size() = " << e.size() << std::endl;
//      }else if (op == "2") {
//        std::cout << "  class_count() = " << e.class_count() << std::endl;
//      }else{
//        if (op == "q")
//          break;
//        std::string x = ics::prompt_string("Enter a value in an equivalence class");
//        std::string y = ics::prompt_string("Enter a value in equivalence class");
//
//        if (op == "i")
//          std::cout << "  " << x << " and " << y << " in_same_class() = " << e.in_same_class(x, y) << std::endl;
//        else {
//          e.merge_classes_of(x,y);
//          std::cout << "  merged equivalence class of " << x << " and " << y << std::endl;
//        }
//      }
//    } catch (ics::IcsError& e) {
//      std::cout << "  " << e.what() << std::endl;
//    }
//}
//
