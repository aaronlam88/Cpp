 /* Aaron Lam (UniqueID: 90777)*/

#ifndef DIJKSTRA_HPP_
#define DIJKSTRA_HPP_

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>                    //Biggest int: std::numeric_limits<int>::max()
#include "array_stack.hpp"
#include "hash_graph.hpp"

namespace ics {

int maximum = std::numeric_limits<int>::max(); //use for compare

int hash(const std::string& s) {std::hash<std::string> str_hash; return str_hash(s);}


class Info {
  public:
    Info(){}
    int          cost     = std::numeric_limits<int>::max();
    std::string from      = "?";
    bool operator == (const Info& rhs) const {
      return cost == rhs.cost && from == rhs.from;
    }
    bool operator != (const Info& rhs) const {
      return !(*this == rhs);
    }
    friend std::ostream& operator << (std::ostream& outs, const Info& i) {
      outs << "Info[" << i.cost << "," << i.from << "]";
      return outs;
    }

};


//Return the final_map as specified in the description of extended Dijkstra algorithm
ics::HashMap<std::string,Info> extended_dijkstra(const ics::HashGraph<int>& g, std::string start_node) {

	int size = g.node_count();
	ics::HashMap<std::string,Info> final_map(size, hash);
	ics::HashMap<std::string,Info> info_map(size, hash);

	for (auto it : g.all_nodes())
		info_map[it.first] = Info();
	info_map[start_node].cost = 0;

	while (!info_map.empty()){//Loop so long as the info_map is not empty
		int minimum = maximum;
		std::string min;
		/*
		a) Find the key in info_map with the smallest associated Info cost -initally
        the start node; if this cost is infinity, then no more nodes in info_map
        are reachable, so terminate.
      	Call this the "min" node. We are now guaranteed to know the least costly
        path from the start node to min.
		 */
		for (auto it : info_map)
			if(it.second.cost < minimum){
				minimum = it.second.cost;
				min = it.first;
			}
		if (info_map[min].cost == maximum)
			break; //cost is infinity, then no more nodes in info_map are reachable, so terminate.

		/*
		b) Remove this key->value from the info_map and put it in the answer_map.
		 */
		final_map.put(min, info_map.erase(min));
		/*
		c) For every node d that is a destination from the min node and not already
        in the answer_map, get d's Info using info_map and see if the cost is
        infinite or greater than the cost of the path from the start node to
        min, plus the cost of the edge from min to d.
        If it is infinite or this sum is smaller,
          (1) update the cost in Info to this smaller number,
          (2) update the predecessor of d to be min,
		 */
		for (auto it : g.out_nodes(min)){
			if (!final_map.has_key(it)) {
				int sum = final_map[min].cost + g.edge_value(min, it);
				if (info_map[it].cost == maximum || info_map[it].cost > sum){
					info_map[it].cost = sum;
					info_map[it].from = min;
				}
			}
		}
	}

	return final_map;
}


//Return a string from the start node (implicit in answer_map) to the end node
//  (left to right, separated by a space)
std::ostream& recover_path (std::ostream& to_return, ics::HashMap<std::string,Info> answer_map, std::string end_node) {
	ics::ArrayStack<std::string> node_stack;
	to_return << "Cost is " << answer_map[end_node].cost << "; ";
	node_stack.push(end_node);
	for(;answer_map[end_node].from != "?"; end_node = answer_map[end_node].from)
		node_stack.push(answer_map[end_node].from);

	to_return << "Path is " << node_stack.pop();
	while(!node_stack.empty()){
		to_return << " " << node_stack.pop();
	}
	to_return << std::endl;
	return to_return;
}

}

#endif /* DIJKSTRA_HPP_ */
