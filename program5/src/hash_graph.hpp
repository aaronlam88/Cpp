 /* Aaron Lam (UniqueID: 90777)*/

#ifndef HASH_GRAPH_HPP_
#define HASH_GRAPH_HPP_

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <initializer_list>
#include "ics_exceptions.hpp"
#include "iterator.hpp"
#include "pair.hpp"
#include "heap_priority_queue.hpp"
#include "hash_set.hpp"
#include "hash_map.hpp"

namespace ics {

template<class T>
class HashGraph {

	//Forward declaration: see the return type of all_nodes
private:
	class LocalInfo;

public:
	//Commands
	HashGraph();
	HashGraph(HashGraph& g);
	virtual ~HashGraph() {}
	void add_node(std::string node_name);
	void add_edge(std::string origin, std::string destination, T value);
	void remove_node(std::string node_name);
	void remove_edge(std::string origin, std::string destination);
	void clear();
	void load(std::ifstream& in_file, std::string separator = ";");
	void store(std::ofstream& out_file, std::string separator = ";");

	//Queries
	bool empty() const;
	int node_count() const;
	int edge_count() const;
	bool has_node(std::string node_name) const;
	bool has_edge(std::string origin, std::string destination) const;
	T edge_value(std::string origin, std::string destination) const;
	int in_degree(std::string node_name) const;
	int out_degree(std::string node_name) const;
	int degree(std::string node_name) const;

	const ics::HashMap<std::string, LocalInfo>& all_nodes() const;
	const ics::HashMap<ics::pair<std::string, std::string>, T>& all_edges() const;

	const ics::HashSet<std::string>& out_nodes(std::string node_name) const;
	const ics::HashSet<std::string>& in_nodes(std::string node_name) const;
	const ics::HashSet<ics::pair<std::string, std::string>>& out_edges(std::string node_name) const;
	const ics::HashSet<ics::pair<std::string, std::string>>& in_edges(std::string node_name) const;

	//Operators
	HashGraph<T>& operator =(const HashGraph<T>& rhs);
	bool operator ==(const HashGraph<T>& rhs) const;
	bool operator !=(const HashGraph<T>& rhs) const;

private:
	//All methods and operators relating to LocalInfo are defined below, followed by
	//  friend functions for insertion onto output streams of HashGrah and LocalInfo
	class LocalInfo {
	public:
		LocalInfo () : from_graph(nullptr), out_nodes(hash_str), in_nodes(hash_str), out_edges(hash_pair_str), in_edges(hash_pair_str) {}
		LocalInfo (HashGraph<T>* g) : from_graph(g), out_nodes(hash_str), in_nodes(hash_str), out_edges(hash_pair_str), in_edges(hash_pair_str) {}

		void connect(HashGraph<T>* g) {from_graph = g;}

		bool operator == (const LocalInfo& rhs) const {
			return this->in_nodes == rhs.in_nodes
					&& this->out_nodes == rhs.out_nodes
					&& this->in_edges == rhs.in_edges
					&& this->out_edges == rhs.out_edges;
		}

		bool operator !=(const LocalInfo& rhs) const {
			return !(*this == rhs);
		}

		//from_graph should point to the HashGraph LocalInfo is in, so LocalInfo
		//  methods (see <<)
		HashGraph<T>* from_graph;
		ics::HashSet<std::string> out_nodes;
		ics::HashSet<std::string> in_nodes;
		ics::HashSet<ics::pair<std::string, std::string>> out_edges;
		ics::HashSet<ics::pair<std::string, std::string>> in_edges;
	};        //LocalInfo

			  //These two friend << operators are easier to define here because of
			  //  the nested LocalInfo in HashGraph, which both use; the nodes in HashGraph
			  //  appear in alphabetical order; the four sets
	template<class T2>
	friend std::ostream& operator <<(std::ostream& outs, const HashGraph<T2>& g) {
		if (g.empty()) {
			outs << "graph[]";
		} else {
			outs << "graph[\n";
			ics::HeapPriorityQueue<ics::pair<std::string, LocalInfo>> nodes( g.node_values.ibegin(), g.node_values.iend(), g.LocalInfo_gt);
			for (ics::pair<std::string, LocalInfo> x : nodes)
				outs << "  " << x << std::endl;
			outs << "]";
		}
		return outs;
	}

	friend std::ostream& operator <<(std::ostream& outs, const LocalInfo& li) {
		outs << "LocalInfo[" << std::endl << "    out_nodes=" << li.out_nodes << std::endl;

		outs << "    out_edges=set[";
		if (li.out_edges.size() != 0) {
			ics::Iterator<ics::pair<std::string, std::string>>& i = li.out_edges.ibegin();
			outs << "->" << i->second << "(" << li.from_graph->edge_values[*i] << ")";
			++i;
			for (/*See above*/; i != li.out_edges.iend(); ++i)
				outs << ",->" << i->second << "(" << li.from_graph->edge_values[*i] << ")";
		}
		outs << "]" << std::endl;

		outs << "    in_nodes =" << li.in_nodes << std::endl;

		outs << "    in_edges =set[";
		if (li.in_edges.size() != 0) {
			ics::Iterator<ics::pair<std::string, std::string>>& i = li.in_edges.ibegin();
			outs << i->first << "->(" << li.from_graph->edge_values[*i] << ")";
			++i;
			for (/*See above*/; i != li.in_edges.iend(); ++i)
				outs << "," << i->first << "->(" << li.from_graph->edge_values[*i] << ")";
		}
		outs << "]" << std::endl << "  ]";
		return outs;
	}

private:
	ics::HashMap<std::string, LocalInfo> node_values;
	ics::HashMap<ics::pair<std::string, std::string>, T> edge_values;

	//Static methods for hashing (in the maps) and for printing in alphabetic
	//  order the nodes in a graph (see << for HashGraph<T>)
	static int hash_str(const std::string& s) {
		std::hash<std::string> str_hash;
		return str_hash(s);
	}

	static int hash_pair_str(const ics::pair<std::string, std::string>& s) {
		std::hash<std::string> str_hash;
		return str_hash(s.first) * str_hash(s.second);
	}

	static bool LocalInfo_gt(const ics::pair<std::string, LocalInfo>& a, const ics::pair<std::string, LocalInfo>& b) {
		return a.first < b.first;
	}
};
//HashGraph

//Default constructor
template<class T>
HashGraph<T>::HashGraph() : node_values(hash_str), edge_values(hash_pair_str) {}

//Copy constructor
template<class T>
HashGraph<T>::HashGraph(HashGraph& g) : node_values(hash_str), edge_values(hash_pair_str) {
	//copy node_values and LocalInfo
	for (auto nodes : g.node_values){
		node_values[nodes.first].from_graph = this;
		node_values[nodes.first].out_nodes = g.node_values[nodes.first].out_nodes;
		node_values[nodes.first].in_nodes = g.node_values[nodes.first].in_nodes;
		node_values[nodes.first].out_edges = g.node_values[nodes.first].out_edges;
		node_values[nodes.first].in_edges = g.node_values[nodes.first].in_edges;
	}
	//copy edge_value
	for (auto edges : g.edge_values)
		edge_values.put(edges.first, edges.second);
}

//Add node_name to the graph if it is not already there.
//Ensure its from_graph refers to this graph.
template<class T>
void HashGraph<T>::add_node(std::string node_name) {
	if (node_values.has_key(node_name))
		return;

	node_values[node_name] = LocalInfo (this);
}

//Add an edge from origin to destination with value
//Add these node names and update edge_values and the LocalInfos of each node
template<class T>
void HashGraph<T>::add_edge(std::string origin, std::string destination, T value) {
	if(!node_values.has_key(origin))
		node_values[origin] = LocalInfo (this);
	if(!node_values.has_key(destination))
		node_values[destination] = LocalInfo (this);
	ics::pair <std::string, std::string> add_pair (origin, destination);
	//update node_values
	//for origin: update out_nodes, out_edges;
	node_values[origin].out_nodes.insert(destination);
	node_values[origin].out_edges.insert(add_pair);
	//for destination: update in_nodes, in_edges;
	node_values[destination].in_nodes.insert(origin);
	node_values[destination].in_edges.insert(add_pair);
	//update edge_values
	edge_values.put (add_pair, value);

}

//Remove all uses of node_name from the graph: update node_values, edge_values,
//  and all the LocalInfo in which it appears as an origin or destination node
//If the node_name is not in the graph, do nothing
//Hint: you cannot iterate over a set that you are changing, so you might have
// to copy a set and then iterate over it while removing values from the original set
template<class T>
void HashGraph<T>::remove_node(std::string node_name) {
	if (! node_values.has_key(node_name))
		return;

	//remove all the edges that associated with node_name in edges_value
	//update LocalInfo of node_name->node
	for(std::string outs : node_values[node_name].out_nodes){
		ics::pair <std::string, std::string> remove_pair (node_name, outs);
		node_values[outs].in_nodes.erase(node_name);
		node_values[outs].in_edges.erase(remove_pair);
		edge_values.erase(remove_pair);
	}
	//update LocalInfo of node->node_name
	for(std::string ins : node_values[node_name].in_nodes){
		ics::pair <std::string, std::string> remove_pair (ins, node_name);
		node_values[ins].out_nodes.erase(node_name);
		node_values[ins].out_edges.erase(remove_pair);
		edge_values.erase(remove_pair);
	}
	//remove node_name's LocalInfo
	node_values[node_name].from_graph = nullptr;
	node_values[node_name].out_nodes.clear();
	node_values[node_name].out_edges.clear();
	node_values[node_name].in_nodes.clear();
	node_values[node_name].in_edges.clear();
	//remove node_name
	node_values.erase(node_name);
}

//Remove all uses of this edge from the graph: update edge_values and all the
//  LocalInfo in which its origin and destination node appears
//If the edge is not in the graph, do nothing
//Hint: Simpler than remove_node: write and test this one first
template<class T>
void HashGraph<T>::remove_edge(std::string origin, std::string destination) {
	ics::pair<std::string, std::string> remove_pair (origin, destination);
	if (! edge_values.has_key(remove_pair))
		return;

	node_values[origin].out_nodes.erase(destination);
	node_values[origin].out_edges.erase(remove_pair);
	node_values[destination].in_nodes.erase(origin);
	node_values[destination].in_edges.erase(remove_pair);
	edge_values.erase(remove_pair);
}

//Clear the graph of all nodes and edges
template<class T>
void HashGraph<T>::clear() {
	//delete LocalInfo of nodes
	for (auto to_remove : node_values){
		std::string remove = to_remove.first;
		node_values[remove].from_graph = nullptr;
		node_values[remove].out_nodes.clear();
		node_values[remove].out_edges.clear();
		node_values[remove].in_nodes.clear();
		node_values[remove].in_edges.clear();
	}
	node_values.clear();
	edge_values.clear();
}

//Load the nodes and edges for a graph from a text file whose form is
// (a) a node name (one per line, one any number of lines)
// (b) the word "NODESABOVEEDGESBELOW"
// (c) an origin node, destination node, and value (one triple per line,
//       with the values separated by separator, on any number of lines)
//Hint: use ics::split and istringstream (the exaction dual of ostreamstring)
template<class T>
void HashGraph<T>::load(std::ifstream& in_file, std::string separator) {
	std::string line;
	bool add_nodes = true;
	while (getline(in_file, line)) {
		if(line == "NODESABOVEEDGESBELOW"){
			getline(in_file, line);
			add_nodes = false;
		}
		if(add_nodes)
			add_node(line);
		else{//add edges
			std::vector<std::string> input = ics::split(line, separator);
			std::istringstream value_str (input[2]);
			T value_T;
			value_str >> value_T;
			add_edge (input[0], input[1], value_T);
		}
	}
}

//Store the nodes and edges in a graph into a text file whose form is specified
//  above for the load method; files written by store should be readable by load
//Hint: this is the easier of the two methods: write and test it first
template<class T>
void HashGraph<T>::store(std::ofstream& out_file, std::string separator) {
	for (auto nodes : node_values)
		out_file << nodes.first << std::endl;
	out_file << "NODESABOVEEDGESBELOW" << std::endl;
	for (auto edges : edge_values)
		out_file << edges.first.first << separator << edges.first.second << separator << edges.second << std::endl;
}

//Returns whether a graph is empty
template<class T>
bool HashGraph<T>::empty() const {
 return node_values.empty();
}

//Returns the number of nodes in a graph
template<class T>
int HashGraph<T>::node_count() const {
	return node_values.size();
}

//Returns the number of edges in a graph
template<class T>
int HashGraph<T>::edge_count() const {
	return edge_values.size();
}

//Returns whether or not node_name is in the graph
template<class T>
bool HashGraph<T>::has_node(std::string node_name) const {
	return node_values.has_key(node_name);
}

//Returns whether or not the edge is in the graph
template<class T>
bool HashGraph<T>::has_edge(std::string origin, std::string destination) const {
	return edge_values.has_key(ics::pair <std::string, std::string> (origin, destination));
}

//Returns the value of the edge in the graph; if the edge is not in the graph,
//  throw a GraphError exception with appropriate descriptive text
template<class T>
T HashGraph<T>::edge_value(std::string origin, std::string destination) const {
	ics::pair <std::string, std::string> edge_pair (origin, destination);
	if (!edge_values.has_key(edge_pair)){
		std::ostringstream exc;
		exc << "HashGraph.edge_value: edge [" << origin << "] -> [" << destination << "] is not in the graph";
		throw ics::GraphError(exc.str());
	}
	return edge_values[edge_pair];
}

//Returns the in-degree of node_name; if that node is not in the graph,
//  throw a GraphError exception with appropriate descriptive text
template<class T>
int HashGraph<T>::in_degree(std::string node_name) const {
	if (!node_values.has_key(node_name)){
		std::ostringstream exc;
		exc << "HashGraph.in_degree: node [" << node_name << "]is not in the graph";
		throw ics::GraphError(exc.str());
	}
	return node_values[node_name].in_edges.size();
}

//Returns the out-degree of node_name; if that node is not in the graph,
//  throw a GraphError exception with appropriate descriptive text
template<class T>
int HashGraph<T>::out_degree(std::string node_name) const {
	if (!node_values.has_key(node_name)){
		std::ostringstream exc;
		exc << "HashGraph.in_degree: node [" << node_name << "]is not in the graph";
		throw ics::GraphError(exc.str());
	}
	return node_values[node_name].out_edges.size();
}

//Returns the degree of node_name; if that node is not in the graph,
//  throw a GraphError exception with appropriate descriptive text.
template<class T>
int HashGraph<T>::degree(std::string node_name) const {
	return in_degree(node_name) + out_degree(node_name);
}

//Returns a reference to the all_nodes map;
//  the user should not mutate its data structure: call Graph commands instead
template<class T>
auto HashGraph<T>::all_nodes() const -> const ics::HashMap<std::string,LocalInfo>& {
	return node_values;
}

//Returns a reference to the all_edges map;
//  the user should not mutate its data structure: call Graph commands instead
template<class T>
const ics::HashMap<ics::pair<std::string, std::string>, T>& HashGraph<T>::all_edges() const {
	return edge_values;
}

//Returns a reference to the out_nodes set in the LocalInfo for node_name;
//  the user should not mutate its data structure: call Graph commands instead; if
//  that node is not in the graph, throw a GraphError exception with appropriate
//  descriptive text
template<class T>
const ics::HashSet<std::string>& HashGraph<T>::out_nodes(std::string node_name) const {
	if (!node_values.has_key(node_name)){
		std::ostringstream exc;
		exc << "HashGraph.out_nodes: node [" << node_name << "]is not in the graph";
		throw ics::GraphError(exc.str());
	}

	return node_values[node_name].out_nodes;
}

//Returns a reference to the in_nodes set in the LocalInfo for node_name;
//  the user should not mutate its data structure: call Graph commands instead; if
//  that node is not in the graph, throw a GraphError exception with appropriate
//  descriptive text
template<class T>
const ics::HashSet<std::string>& HashGraph<T>::in_nodes(std::string node_name) const {
	if (!node_values.has_key(node_name)){
		std::ostringstream exc;
		exc << "HashGraph.out_nodes: node [" << node_name << "]is not in the graph";
		throw ics::GraphError(exc.str());
	}
	return node_values[node_name].in_nodes;
}

//Returns a reference to the out_edges set in the LocalInfo for node_name;
//  the user should not mutate its data structure: call Graph commands instead; if
//  that node is not in the graph, throw a GraphError exception with appropriate
//  descriptive text
template<class T>
const ics::HashSet<ics::pair<std::string, std::string>>& HashGraph<T>::out_edges(std::string node_name) const {
	if (!node_values.has_key(node_name)) {
		std::ostringstream exc;
		exc << "HashGraph.out_nodes: node [" << node_name << "]is not in the graph";
		throw ics::GraphError(exc.str());
	}
	return node_values[node_name].out_edges;
}

//Returns a reference to the in_edges set in the LocalInfo for node_name;
//  the user should not mutate its data structure: call Graph commands instead; if
//  that node is not in the graph, throw a GraphError exception with appropriate
//  descriptive text
template<class T>
const ics::HashSet<ics::pair<std::string, std::string>>& HashGraph<T>::in_edges(std::string node_name) const {
	if (!node_values.has_key(node_name)) {
		std::ostringstream exc;
		exc << "HashGraph.out_nodes: node [" << node_name << "]is not in the graph";
		throw ics::GraphError(exc.str());
	}
	return node_values[node_name].in_edges;
}

//Copy the specified graph into this and return the newly copied graph
//Hint: each copied LocalInfo object should reset from_graph to the this new graph
template<class T>
HashGraph<T>& HashGraph<T>::operator = (const HashGraph<T>& rhs) {
	if (*this == rhs)
		return *this;
	clear();
	//copy node_values and LocalInfo
	for (auto nodes : rhs.node_values){
		node_values[nodes.first].from_graph = this;
		node_values[nodes.first].out_nodes = rhs.node_values[nodes.first].out_nodes;
		node_values[nodes.first].in_nodes = rhs.node_values[nodes.first].in_nodes;
		node_values[nodes.first].out_edges = rhs.node_values[nodes.first].out_edges;
		node_values[nodes.first].in_edges = rhs.node_values[nodes.first].in_edges;
	}
	//copy edge_value
	for (auto edges : rhs.edge_values)
		edge_values.put(edges.first, edges.second);
	return *this;
}

//Return whether two graphs are the same
template<class T>
bool HashGraph<T>::operator == (const HashGraph<T>& rhs) const {
	if (this == &rhs)
		return true;
	if (node_values.size() != rhs.node_values.size() || edge_values.size() != rhs.edge_values.size())
		return false;
//	for (auto it : rhs.edge_values)
//		if (edge_values.has_key(it.first))
//			if(edge_values[it.first] != it.second)
//				return false;
//
//	for (auto it : rhs.node_values)
//		if (!node_values.has_key(it.first))
//			return false;
//		//the LocalInfo should be equal if edge_value == rhs.edge_value and node_values == rhs.node_values
	if (node_values != rhs.node_values)
		return false;
	if (edge_values != rhs.edge_values)
		return false;

	return true;
}

//Return whether two graphs are different
template<class T>
bool HashGraph<T>::operator !=(const HashGraph<T>& rhs) const {
	return !(*this == rhs);
}

}

#endif /* HASH_GRAPH_HPP_ */
