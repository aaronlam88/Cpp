////# Aaron Lam
////# Vikram Sai Mohan
////# We certify that we worked cooperatively on this programming
////#   assignment, according to the rules for pair programming:
////#   primarily that both partners worked on all parts together.
//
//#include <string>
//#include <iostream>
//#include <fstream>
//#include <sstream>
//#include <vector>
//#include "ics46goody.hpp"
//#include "array_queue.hpp"
//#include "array_priority_queue.hpp"
//#include "array_set.hpp"
//#include "array_map.hpp"
//
//typedef ics::ArraySet<std::string> NodeSet;
//typedef ics::pair<std::string, NodeSet> GraphEntry;
//typedef ics::ArrayPriorityQueue<GraphEntry> GraphPQ;
//typedef ics::ArrayMap<std::string, NodeSet> Graph;
//
////Read an open file of edges (node names separated by semicolons, with an
////  edge going from the first node name to the second node name) and return a
////  Graph (Map) of each node name associated with the Set of all node names to
////  which there is an edge from the key node name.
//Graph read_graph(std::ifstream &file) {
//	Graph g;
//	std::string line;
//	ics::safe_open(file, "Enter graph file name ", "graph1.txt");
//	while (getline(file, line)) {
//		std::vector<std::string> node = ics::split(line, ";");
//		g[node[0]].insert(node[1]);
//	}
//	file.close();
//	return g;
//}
//
////Print a label and all the entries in the Graph, in alphabetical order.
////Use a "->" to separate the key node name from the Set of node names to
////  which it has an edge.
//void print_graph(const Graph& graph) {
//	std::cout << "\nGraph: source -> {destination} edges\n";
//
//	GraphPQ q(graph.ibegin(), graph.iend(),
//			[](const GraphEntry& i,const GraphEntry& j) {
//				return i.first < j.first;
//			});
//
//	for (auto it : q)
//		std::cout << "  " << it.first << " -> " << it.second << std::endl;
//}
//
////Return the Set of node names reaching in the Graph starting at the
////  specified (start) node.
////Use a local Set and a Queue to respectively store the reachable nodes and
////  the nodes that are being explored.
//NodeSet reachable(const Graph& graph, std::string start) {
//	NodeSet answer;
//	if (!graph.has_key(start))
//		return answer;
//	ics::ArrayQueue<std::string> q;
//	answer.insert(start);
//	q.enqueue(start);
//	while (!q.empty()) {
//		start = q.dequeue();
//		if (graph.has_key(start))
//			for (auto it : graph[start])
//				if (answer.insert(it))
//					q.enqueue(it);
//	}
//	return answer;
//}
//
////Prompt the user for a file, create a graph from its edges, print the graph,
////  and then repeatedly (until the user enters "quit") prompt the user for a
////  starting node name and then either print an error (if that the node name
////  is not a source node in the graph) or print the Set of node names
////  reachable from it using the edges in the Graph.
//int main() {
//	Graph g;
//	std::ifstream file;
//	std::string start;
//	try {
//		g = read_graph(file);
//		print_graph(g);
//		while (true) {
//			start = ics::prompt_string(
//					"Enter starting node (enter quit to quit)");
//			if (start == "quit")
//				break;
//			std::cout << "Reachable from node namereachable " << start << "="
//					<< reachable(g, start) << std::endl;
//		}
//	} catch (ics::IcsError& e) {
//		std::cout << e.what() << std::endl;
//	}
//
//	return 0;
//}
