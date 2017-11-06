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
#include <limits>                           //I used std::numeric_limits<int>::max()
#include "ics46goody.hpp"
#include "array_queue.hpp"
#include "array_priority_queue.hpp"
#include "array_set.hpp"
#include "array_map.hpp"

typedef ics::ArrayQueue<std::string> WordQueue;
typedef ics::ArraySet<std::string> FollowSet;
typedef ics::pair<WordQueue, FollowSet> CorpusEntry;
typedef ics::ArrayPriorityQueue<CorpusEntry> CorpusPQ;
typedef ics::ArrayMap<WordQueue, FollowSet> Corpus;

//Return a random word in the words set (use in produce_text)
std::string random_in_set(const FollowSet& words) {
	int index = ics::rand_range(1, words.size());
	int i = 0;
	for (std::string s : words)
		if (++i == index)
			return s;
	return "?";
}

//Read an open file of lines of words (separated by spaces) and return a
//  Corpus (Map) of each sequence (Queue) of os (Order-Statistic) words
//  associated with the Set of all words that follow them somewhere in the
//  file.
//You may assume the first line contains at least Order-Statstic words.
Corpus read_corpus(int os, std::ifstream &file) {
	Corpus corpus;
	WordQueue wordQ;
	std::string line;
	ics::safe_open(file, "Enter file with finite automaton ", "wginput1.txt");
	while (getline(file, line)) {
		std::vector<std::string> str = ics::split(line, " ");
		for (unsigned int i = 0; i < str.size(); ++i) {
			if (wordQ.size() < 2)
				wordQ.enqueue(str[i]);
			if (wordQ.size() == 2) {
				if (i < str.size() - 1)
					corpus[wordQ].insert(str[i + 1]);
				wordQ.dequeue();
			}
		}
	}
	return corpus;
}

//Print "Corpus" and all entries in the Corpus, in lexical alphabetical order
//  (with the minimum and maximum set sizes at the end).
//Use a "can be followed by any of" to separate the key word from the Set of words
//  that can follow it.

//One queue is lexically greater than another, if its first value is smaller; or if
//  its first value is the same and its second value is smaller; or if its first
//  and second values are the same and its third value is smaller...
//If any of its values is less than the corresponding value in the other queue,
//  the first queue is not greater.
//Note that the queues sizes are the same: each stores Order-Statistic words
//Important: Use iterators for examining the queue values: DO NOT CALL DEQUEUE.

bool queue_gt(const CorpusEntry& a, const CorpusEntry& b) {
	auto &itA = a.first.ibegin();
	auto &itB = b.first.ibegin();
	for (; itA != a.first.iend() || itB != b.first.iend(); ++itA, ++itB)
		if (*itA != *itB)
			return *itA < *itB;
	return a.first.size() < b.first.size();
}

void print_corpus(const Corpus& corpus) {
	CorpusPQ cpq(corpus.ibegin(), corpus.iend(), *queue_gt);
	int min = cpq.ibegin()->second.size();
	int max = min;
	std::cout << "\nCorpus\n";
	for (auto it : cpq) {
		std::cout << "  " << it.first << " -> " << it.second << std::endl;
		if (it.second.size() < min)
			min = it.second.size();
		if (it.second.size() > max)
			max = it.second.size();
	}
	std::cout << "min/max = " << min << "/" << max << std::endl;
}

//Return a Queue of words, starting with those in start and including count more
//  randomly selected words using corpus to decide which word comes next.
//If there is no word that follows the previous ones, put "None" into the queue
//  and return immediately this list (whose size is <= start.size() + count).
WordQueue produce_text(const Corpus& corpus, const WordQueue& start, int count) {
	WordQueue produce(start);
	WordQueue key(start);
	for (int i = 0; i < count; ++i) {
		if (corpus.has_key(key)) {
			std::string str = random_in_set(corpus[key]);
			produce.enqueue(str);
			key.enqueue(str);
			key.dequeue();
		} else {
			produce.enqueue("None");
			return produce;
		}
	}
	return produce;
}

//Prompt the user for (a) the order statistic and (b) the file storing the text.
//Read the text as a Corpus and print it appropriately.
//Prompt the user for order statistic words from the text.
//Prompt the user for number of random words to generate
//Call the functions above to solve the problem, and print the appropriate information
int main() {
	std::ifstream file;
	Corpus corpus;
	try {
		int os = ics::prompt_int("Enter order statistic", 2);
		corpus = read_corpus(os, file);
		print_corpus(corpus);
		std::cout << "\nEnter " << os << " to start with\n";
		WordQueue start;
		start.enqueue(ics::prompt_string("Enter word 1"));
		start.enqueue(ics::prompt_string("Enter word 2"));
		int count = ics::prompt_int("Enter # of words to generate");
		WordQueue produce = produce_text(corpus, start, count);
		std::cout << "Random text = " << produce << std::endl;
	} catch (ics::IcsError& e) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}

