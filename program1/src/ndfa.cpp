//# Aaron Lam
//# Vikram Sai Mohan
//# We certify that we worked cooperatively on this programming
//#   assignment, according to the rules for pair programming:
//#   primarily that both partners worked on all parts together.

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "ics46goody.hpp"
#include "array_queue.hpp"
#include "array_priority_queue.hpp"
#include "array_set.hpp"
#include "array_map.hpp"

typedef ics::ArraySet<std::string> States;
typedef ics::ArrayQueue<std::string> InputsQueue;
typedef ics::ArrayMap<std::string, States> InputStatesMap;

typedef ics::ArrayMap<std::string, InputStatesMap> NDFA;
typedef ics::pair<std::string, InputStatesMap> NDFAEntry;
typedef ics::ArrayPriorityQueue<NDFAEntry> NDFAPQ;

typedef ics::pair<std::string, States> Transitions;
typedef ics::ArrayQueue<Transitions> TransitionsQueue;

//Read an open file describing the non-deterministic finite automaton (each
//  line starts with a state name followed by pairs of transitions from that
//  state: (input followed by a new state) all separated by semicolons), and
//  return a Map whose keys are states and whose associated values are another
//  Map with each input in that state (keys) and the resulting set of states it
//  can lead to.
const NDFA read_ndfa(std::ifstream &file) {
	NDFA ndfa;
	std::string line;
	ics::safe_open(file, "Enter file name of Non_Deterministic Finite Automaton and input ", "ndfaendin01.txt");
	while (getline(file, line)) {
		std::vector<std::string> text = ics::split(line, ";");
		ndfa.put(text[0], InputStatesMap());
		if (text.size() >= 3)
			for (unsigned int i = 1; i < text.size(); i += 2)
				ndfa[text[0]][text[i]].insert(text[i + 1]);
	}
	file.close();
	return ndfa;
}


//Print a label and all the entries in the finite automaton Map, in
//  alphabetical order of the states: each line has a state, the text
//  "transition:" and the Map of its transitions.
void print_ndfa(const NDFA& ndfa) {
	std::cout << "\nNon_Deterministic Finite Automaton Description\n";
	NDFAPQ printQ(ndfa.ibegin(), ndfa.iend(),
			[](const NDFAEntry& i,const NDFAEntry& j) {
				return i.first < j.first;});
	for (auto it : printQ) {
		std::cout << "  " << it.first << " transitions: " << it.second << std::endl;
	}
}


//Return a queue of the calculated transition pairs, based on the non-deterministic
//  finite automaton, initial state, and queue of inputs; each pair in the returned
//  queue is of the form: input, set of new states.
//The first pair contains "" as the input and the initial state.
//If any input i is illegal (does not lead to any state in the non-deterministic finite
//  automaton, ignore it).
TransitionsQueue process(const NDFA& ndfa, std::string state,
		const InputsQueue& inputs) {
	TransitionsQueue tq;
	tq.enqueue(Transitions("", States({state})));
	InputsQueue current ({state});
	for (auto itQ : inputs){
		States temp;
		while (!current.empty()){
			state = current.dequeue();
			if(ndfa[state].has_key(itQ))
				temp.insert(ndfa[state][itQ].ibegin(), ndfa[state][itQ].iend());
		}
		tq.enqueue(Transitions(itQ, temp));
		current.enqueue(temp.ibegin(), temp.iend());
	}
	return tq;
}


//Print a TransitionsQueue (the result of calling process) in a nice form.
//Print the Start state on the first line; then print each input and the
//  resulting new states indented on subsequent lines; on the last line, print
//  the Stop state.
void interpret(TransitionsQueue& tq) {  //or TransitionsQueue or TransitionsQueue&&
	Transitions value = tq.dequeue();
	std::cout << "Start state = " << value.second << std::endl;
	States endState;
	while (!tq.empty()) {
		value = tq.dequeue();
		std::cout << "Input = " << value.first << "; new state = "
				<< (endState = value.second) << std::endl;
	}
	std::cout << "Stop  state = " << endState << std::endl << std::endl;
}



//Prompt the user for a file, create a finite automaton Map, and print it.
//Prompt the user for a file containing any number of simulation descriptions
//  for the finite automaton to process, one description per line; each
//  description contains a start state followed by inputs, all separated by
//  semicolons.
//Repeatedly read a description, print that description, put each input in a
//  Queue, process the Queue and print the results in a nice form.
int main() {
	std::ifstream file;
	NDFA ndfa;
	try {
		ndfa = read_ndfa(file);
		print_ndfa(ndfa);
		std::string line;
		ics::safe_open(file, "\nEnter file with start-state and input:",
				"ndfainputendin01.txt");
		while (getline(file, line)) {
			std::cout << "Starting new simulation with description: " << line
					<< std::endl;
			std::string state;
			InputsQueue input;
			std::vector<std::string> text = ics::split(line, ";");
			state = text[0];
			for (unsigned int i = 1; i < text.size(); ++i)
				input.enqueue(text[i]);
			TransitionsQueue tq = process(ndfa, state, input);
			interpret(tq);
		}
		file.close();
	} catch (ics::IcsError& e) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}

