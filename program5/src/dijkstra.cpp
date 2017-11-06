// /* Aaron Lam (UniqueID: 90777)*/
//
//#include <string>
//#include <iostream>
//#include <fstream>
//#include "ics46goody.hpp"
//#include "array_queue.hpp"
//#include "hash_graph.hpp"
//#include "dijkstra.hpp"
//
//
//std::string get_node_in_graph(const ics::HashGraph<int>& g, std::string prompt, bool allow_QUIT) {
//  std::string node;
//  for(;;) {
//    node = ics::prompt_string(prompt + " (must be in graph" + (allow_QUIT ? " or QUIT" : "") + ")");
//    if ((allow_QUIT && node == "QUIT") || g.has_node(node))
//      break;
//  }
//  return node;
//}
//
//void get_graph(ics::HashGraph<int>& g){
//	std::ifstream in_file;
//	ics::safe_open(in_file, "Enter file name to read graph from ", "flightdist.txt");
//	g.load(in_file, ";");
//	std::cout << g << std::endl;
//}
//
//ics::HashMap<std::string,ics::Info> get_dijkstra (const ics::HashGraph<int>& g, std::string start_node){
//	return extended_dijkstra (g, start_node);
//}
//
//void get_shortest_path (ics::HashMap<std::string,ics::Info> disjkstra, std::string stop_node){
//	recover_path (std::cout, disjkstra, stop_node);
//}
//
//int main() {
//  try {
//	  ics::HashGraph<int> g;
//	  std::string start_node, stop_node = "";
//	  ics::HashMap<std::string,ics::Info> disjkstra (ics::hash);
//	  get_graph(g);
//	  start_node = get_node_in_graph(g, "Enter start node ", false);
//	  disjkstra = get_dijkstra(g, start_node);
//	  std::cout << disjkstra << std::endl;
//	  for (;;){
//		  stop_node = get_node_in_graph (g, "Enter stop node ", true);
//		  if (stop_node == "QUIT")
//			  break ;
//		  get_shortest_path(disjkstra, stop_node);
//	  }
//  } catch (ics::IcsError& e) {
//    std::cout << e.what() << std::endl;
//  }
//
//  return 0;
//}
