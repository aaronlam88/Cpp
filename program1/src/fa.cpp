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

typedef ics::ArrayQueue<std::string> InputsQueue;
typedef ics::ArrayMap<std::string, std::string> InputStateMap;

typedef ics::ArrayMap<std::string, InputStateMap> FA;
typedef ics::pair<std::string, InputStateMap> FAEntry;
typedef ics::ArrayPriorityQueue<FAEntry> FAPQ;

typedef ics::pair<std::string, std::string> Transition;
typedef ics::ArrayQueue<Transition> TransitionQueue;

//Read an open file describing the finite automaton (each line starts with
//  a state name followed by pairs of transitions from that state: (input
//  followed by new state) all separated by semicolons), and return a Map
//  whose keys are states and whose associated values are another Map with
//  each input in that state (keys) and the resulting state it leads to.
const FA read_fa(std::ifstream &file) {
	FA fa;
	std::string line;
	ics::safe_open(file, "Enter file with finite automaton ", "faparity.txt");
	while (getline(file, line)) {
		std::vector<std::string> text = ics::split(line, ";");
		fa.put(text[0], InputStateMap());
		for (unsigned int i = 1; i < text.size(); i += 2)
			fa[text[0]].put(text[i], text[i + 1]);
	}
	file.close();
	return fa;
}

//Print a label and all the entries in the finite automaton Map, in
//  alphabetical order of the states: each line has a state, the text
//  "transition:" and the Map of its transitions.
void print_fa(const FA& fa) {
	std::cout << "\nFinite Automaton\n";
	for (auto it : fa) {
		std::cout << "  " << it.first << " transition: " << it.second
				<< std::endl;
	}
}

//Return a queue of the calculated transition pairs, based on the finite
//  automaton, initial state, and queue of inputs; each pair in the returned
//  queue is of the form: input, new state.
//The first pair contains "" as the input and the initial state.
//If any input i is illegal (does not lead to a state in the finite
//  automaton, then the last pair in the returned queue is i,"None".
TransitionQueue process(const FA& fa, std::string state,
		const InputsQueue& inputs) {
	TransitionQueue tq;
	tq.enqueue(Transition("", state));
	for (auto itQ : inputs) {
		if (fa[state].has_key(itQ)) {
			tq.enqueue(Transition(itQ, fa[state][itQ]));
			state = fa[state][itQ];
			if (!fa.has_key(state))
				break;
		} else {
			tq.enqueue(Transition("x", "None"));
			break;
		}
	}
	return tq;
}

//Print a TransitionQueue (the result of calling process) in a nice form.
//Print the Start state on the first line; then print each input and the
//  resulting new state (or "illegal input: terminated", if the state is
//  "None") indented on subsequent lines; on the last line, print the Stop
//  state (which might be "None").
void interpret(TransitionQueue& tq) {  //or TransitionQueue or TransitionQueue&&
	Transition value = tq.dequeue();
	std::cout << "Start state = " << value.second << std::endl;
	std::string endState;
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
	FA fa;
	try {
		fa = read_fa(file);
		print_fa(fa);
		std::string line;
		ics::safe_open(file, "\nEnter file with start-state and input:",
				"fainputparity.txt");
		while (getline(file, line)) {
			std::cout << "Starting new simulation with description: " << line
					<< std::endl;
			std::string state;
			InputsQueue input;
			std::vector<std::string> text = ics::split(line, ";");
			state = text[0];
			for (unsigned int i = 1; i < text.size(); ++i)
				input.enqueue(text[i]);
			TransitionQueue tq = process(fa, state, input);
			interpret(tq);
		}
		file.close();
	} catch (ics::IcsError& e) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}

