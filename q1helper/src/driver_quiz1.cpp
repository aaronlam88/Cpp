#include <string>
#include <iostream>
#include <fstream>
#include "array_set.hpp"
#include "array_map.hpp"
#include "q1solution.hpp"


int main() {
  try{
    //Testing swap
    std::cout << "\nTesting swap" << std::endl;
    ics::ArrayMap<std::string,ics::ArraySet<std::string>> names_set;
    ics::ArraySet<std::string> boy_names, girl_names;
    boy_names.insert("Mary");    boy_names.insert("Betty");    boy_names.insert("Mimsi");
    girl_names.insert("Peter");  girl_names.insert("Joey");    girl_names.insert("Joe"); girl_names.insert("Carl");
    names_set["Boy"]  = boy_names;
    names_set["Girl"] = girl_names;
    std::cout << "Original Map = " << names_set << std::endl;
    //Oops, got the names backwards; let's swap values mapped to/from "Boy" and "Girl"
    swap(names_set, std::string("Boy"), std::string("Girl"));
    std::cout << "Swapped  Map = " << names_set << std::endl;


    //Testing values_set_to_queue
    std::cout << "\nTesting values_set_to_queue" << std::endl;
    std::cout << "Original Map = " << names_set << std::endl;
    ics::ArrayMap<std::string,ics::ArrayQueue<std::string>> names_queue;
    values_set_to_queue(names_set, names_queue);
    std::cout << "New Map = " << names_queue << std::endl;


    //Testing reachable
    std::cout << "\nTesting reachable" << std::endl;
    ics::ArrayMap<std::string,std::string> f;
    f["a"] = "c";
    f["b"] = "b";
    f["c"] = "d";
    f["d"] = "c";
    std::cout << "For map f = " << f << std::endl;

    std::cout << "reachable(f,\"a\") = " << reachable(f,std::string("a")) << std::endl;
    std::cout << "reachable(f,\"b\") = " << reachable(f,std::string("b")) << std::endl;
    std::cout << "reachable(f,\"c\") = " << reachable(f,std::string("c")) << std::endl;
    std::cout << "reachable(f,\"d\") = " << reachable(f,std::string("d")) << std::endl;

    f["a"] = "b";
    f["b"] = "c";
    f["c"] = "d";
    f["d"] = "a";
    std::cout << "For map f = " << f << std::endl;

    std::cout << "reachable(f,\"a\") = " << reachable(f,std::string("a")) << std::endl;
    std::cout << "reachable(f,\"b\") = " << reachable(f,std::string("b")) << std::endl;
    std::cout << "reachable(f,\"c\") = " << reachable(f,std::string("c")) << std::endl;
    std::cout << "reachable(f,\"d\") = " << reachable(f,std::string("d")) << std::endl;


    //Testing follows
    std::cout << "\nTesting follows" << std::endl;
    std::cout << follows(ics::ArrayQueue<std::string>({"b","o","o","k","k","e","e","p","e","r"})) << std::endl;
    std::cout << follows(ics::ArrayQueue<std::string>({"b","o","b","b","l","e","b","o","y"})) << std::endl;


    //Testing reverse
    std::cout << "\nTesting reverse" << std::endl;
    ics::ArrayMap<std::string,ics::ArraySet<std::string>> skills;
    skills["Rich"] = ics::ArraySet<std::string>({"computers","Excel", "cooking"});
    skills["Ellen"] = ics::ArraySet<std::string>({"Ipad","yardwork"});
    skills["Steve"] = ics::ArraySet<std::string>({"yardwork","Excel", "cooking"});
    skills["Nancy"] = ics::ArraySet<std::string>({"yardwork","Ipad", "cooking"});
    std::cout << "For skill map:\n  " << skills <<"\nreverse is\n  " << reverse(skills) << std::endl;

    skills.clear();
    skills["Angie"] = ics::ArraySet<std::string>({"plumbing","yardwork"});
    skills["Bob"] = ics::ArraySet<std::string>({"computer","carpentry"});
    skills["Charlie"] = ics::ArraySet<std::string>({"yardwork","carpentry", "roofing"});
    skills["Denise"] = ics::ArraySet<std::string>({"yardwork","computers", "roofing"});
    skills["Egon"] = ics::ArraySet<std::string>({"computers"});
    skills["Frances"] = ics::ArraySet<std::string>({"plumbing","carpentry", "roofing"});

    std::cout << "\nFor skill map:\n  " << skills <<"\nreverse is\n  " << reverse(skills) << std::endl;
  } catch (ics::IcsError& e) {
    std::cout << "  " << e.what() << std::endl;
  }


  return 0;
}



/*
Here is the output that the driver should produce:

Testing extract
Original  Set = set[set[a,b],set[b,c],set[b,x,y]]
Extracted Set = set[a,b,c,x,y]

Testing swap
Original Map = map[Boy->set[Mary,Betty,Mimsi],Girl->set[Peter,Joey,Joe,Carl]]
Swapped  Map = map[Boy->set[Peter,Joey,Joe,Carl],Girl->set[Mary,Betty,Mimsi]]

Testing values_set_to_queue
Original Map = map[Boy->set[Peter,Joey,Joe,Carl],Girl->set[Mary,Betty,Mimsi]]
New Map = map[Boy->queue[Peter,Joey,Joe,Carl]:rear,Girl->queue[Mary,Betty,Mimsi]:rear]

Testing reachable
For map f = map[a->c,b->b,c->d,d->c]
reachable(f,"a") = set[a,c,d]
reachable(f,"b") = set[b]
reachable(f,"c") = set[c,d]
reachable(f,"d") = set[d,c]
*/
