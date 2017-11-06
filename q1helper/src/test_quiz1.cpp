//#include <iostream>
//#include <sstream>
//#include "ics46goody.hpp"
//#include "gtest/gtest.h"
//#include "q1solution.hpp"
//
//class SetTest : public ::testing::Test {
//protected:
//    virtual void SetUp()    {}
//    virtual void TearDown() {}
//};
//
//
//TEST_F(SetTest, swap) {
//  ics::ArrayMap<std::string,ics::ArraySet<std::string>> names_set;
//  ics::ArraySet<std::string> boy_names, girl_names;
//  boy_names.insert("Mary");    boy_names.insert("Betty");    boy_names.insert("Mimsi");
//  girl_names.insert("Peter");  girl_names.insert("Joey");    girl_names.insert("Joe"); girl_names.insert("Carl");
//  names_set["Boy"]  = boy_names;
//  names_set["Girl"] = girl_names;
//  //Oops, got the names backwards; let's swap values mapped to/from "Boy" and "Girl"
//  swap(names_set, std::string("Boy"), std::string("Girl"));
//
//  ASSERT_EQ(names_set["Boy"], girl_names);
//  ASSERT_EQ(names_set["Girl"],boy_names);
//}
//
//
//TEST_F(SetTest, values_set_to_queue) {
//  ics::ArrayMap<std::string,ics::ArraySet<std::string>> names_set;
//  ics::ArraySet<std::string> boy_names, girl_names;
//  boy_names.insert("Mary");    boy_names.insert("Betty");    boy_names.insert("Mimsi");
//  girl_names.insert("Peter");  girl_names.insert("Joey");    girl_names.insert("Joe"); girl_names.insert("Carl");
//  names_set["Boy"]  = boy_names;
//  names_set["Girl"] = girl_names;
//  //Oops, got the names backwards; let's swap values mapped to/from "Boy" and "Girl"
//  swap(names_set, std::string("Boy"), std::string("Girl"));
//
//  ics::ArrayMap<std::string,ics::ArrayQueue<std::string>> names_queue;
//  values_set_to_queue(names_set, names_queue);
//
//  ASSERT_EQ(4,names_set["Boy"].size());
//  for (auto bn : names_set["Boy"])
//    ASSERT_TRUE(names_set["Boy"].contains(bn));
//  ASSERT_EQ(3,names_set["Girl"].size());
//  for (auto gn : names_set["Girl"])
//    ASSERT_TRUE(names_set["Girl"].contains(gn));
//}
//
//
//TEST_F(SetTest, reachable) {
//  ics::ArrayMap<std::string,std::string> f;
//  f["a"] = "c";
//  f["b"] = "b";
//  f["c"] = "d";
//  f["d"] = "c";
//
//  ASSERT_EQ(ics::ArraySet<std::string>({"a","c","d"}), reachable(f,std::string("a")));
//  ASSERT_EQ(ics::ArraySet<std::string>({"b"}), reachable(f,std::string("b")));
//  ASSERT_EQ(ics::ArraySet<std::string>({"c","d"}), reachable(f,std::string("c")));
//  ASSERT_EQ(ics::ArraySet<std::string>({"d","c"}), reachable(f,std::string("d")));
//
//  f["a"] = "b";
//  f["b"] = "c";
//  f["c"] = "d";
//  f["d"] = "a";
//
//  ASSERT_EQ(ics::ArraySet<std::string>({"a","b","c","d"}), reachable(f,std::string("a")));
//  ASSERT_EQ(ics::ArraySet<std::string>({"a","b","c","d"}), reachable(f,std::string("b")));
//  ASSERT_EQ(ics::ArraySet<std::string>({"a","b","c","d"}), reachable(f,std::string("c")));
//  ASSERT_EQ(ics::ArraySet<std::string>({"a","b","c","d"}), reachable(f,std::string("d")));
//}
//
//
//TEST_F(SetTest, follows) {
//  ics::ArrayMap<std::string,ics::ArraySet<std::string>> follow_map =
//    follows(ics::ArrayQueue<std::string>({"b","o","o","k","k","e","e","p","e","r",}));
//  ASSERT_EQ(5,follow_map.size());
//  ASSERT_EQ(ics::ArraySet<std::string>({"o"}), follow_map["b"]);
//  ASSERT_EQ(ics::ArraySet<std::string>({"e","p","r"}), follow_map["e"]);
//  ASSERT_EQ(ics::ArraySet<std::string>({"e","k"}), follow_map["k"]);
//  ASSERT_EQ(ics::ArraySet<std::string>({"o","k"}), follow_map["o"]);
//  ASSERT_EQ(ics::ArraySet<std::string>({"e"}), follow_map["p"]);
//
//  follow_map = follows(ics::ArrayQueue<std::string>({"b","o","b","b","l","e","b","o","y"}));
//  ASSERT_EQ(4,follow_map.size());
//  ASSERT_EQ(ics::ArraySet<std::string>({"o","b","l"}), follow_map["b"]);
//  ASSERT_EQ(ics::ArraySet<std::string>({"b"}), follow_map["e"]);
//  ASSERT_EQ(ics::ArraySet<std::string>({"e"}), follow_map["l"]);
//  ASSERT_EQ(ics::ArraySet<std::string>({"b","y"}), follow_map["o"]);
//}
//
//
//TEST_F(SetTest, reverse) {
//  ics::ArrayMap<std::string,ics::ArraySet<std::string>> skills;
//  skills["Rich"] = ics::ArraySet<std::string>({"computers","Excel", "cooking"});
//  skills["Ellen"] = ics::ArraySet<std::string>({"Ipad","yardwork"});
//  skills["Steve"] = ics::ArraySet<std::string>({"yardwork","Excel", "cooking"});
//  skills["Nancy"] = ics::ArraySet<std::string>({"yardwork","Ipad", "cooking"});
//
//  ics::ArrayMap<std::string,ics::ArraySet<std::string>> reverse_skills = reverse(skills);
//
//  ASSERT_EQ(5,reverse_skills.size());
//  ASSERT_EQ(ics::ArraySet<std::string>({"Rich"}), reverse_skills["computers"]);
//  ASSERT_EQ(ics::ArraySet<std::string>({"Rich","Steve"}), reverse_skills["Excel"]);
//  ASSERT_EQ(ics::ArraySet<std::string>({"Rich","Steve","Nancy"}), reverse_skills["cooking"]);
//  ASSERT_EQ(ics::ArraySet<std::string>({"Ellen","Nancy"}), reverse_skills["Ipad"]);
//  ASSERT_EQ(ics::ArraySet<std::string>({"Ellen","Steve","Nancy"}), reverse_skills["yardwork"]);
//
//  skills.clear();
//
//  skills["Angie"] = ics::ArraySet<std::string>({"plumbing","yardwork"});
//  skills["Bob"] = ics::ArraySet<std::string>({"computers","carpentry"});
//  skills["Charlie"] = ics::ArraySet<std::string>({"yardwork","carpentry", "roofing"});
//  skills["Denise"] = ics::ArraySet<std::string>({"yardwork","computers", "roofing"});
//  skills["Egon"] = ics::ArraySet<std::string>({"computers"});
//  skills["Frances"] = ics::ArraySet<std::string>({"plumbing","carpentry", "roofing"});
//
//  reverse_skills = reverse(skills);
//
//  ASSERT_EQ(5,reverse_skills.size());
//  ASSERT_EQ(ics::ArraySet<std::string>({"Frances","Bob","Charlie"}), reverse_skills["carpentry"]);
//  ASSERT_EQ(ics::ArraySet<std::string>({"Egon","Bob","Denise"}), reverse_skills["computers"]);
//  ASSERT_EQ(ics::ArraySet<std::string>({"Frances","Angie"}), reverse_skills["plumbing"]);
//  ASSERT_EQ(ics::ArraySet<std::string>({"Frances","Denise","Charlie"}), reverse_skills["roofing"]);
//  ASSERT_EQ(ics::ArraySet<std::string>({"Angie","Denise","Charlie"}), reverse_skills["yardwork"]);
//}
//
//
//
//int main(int argc, char **argv) {
//  ::testing::InitGoogleTest(&argc, argv);
//  return RUN_ALL_TESTS();
//}
