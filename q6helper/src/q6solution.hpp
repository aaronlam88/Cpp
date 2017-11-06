#ifndef Q6SOLUTION_HPP_
#define Q6SOLUTION_HPP_

#include <iostream>
#include <exception>
#include <fstream>
#include <sstream>
#include <algorithm>                 // use std::swap
#include "ics46goody.hpp"
#include "array_queue.hpp"

////////////////////////////////////////////////////////////////////////////////

//Problem #1

template<class T>
class LN {
public:

	LN() : value(), next() {}
	LN(const LN<T>& l) : value(l.value), next(l.next) {}
	LN(T value, LN* n = nullptr) : value(value), next(n) {}

	T value;
	LN* next;
};

//Write this function
template<class T>
void selection_sort(LN<T>* l) {
	for (LN<T>* to_change = l; to_change != nullptr; to_change = to_change->next){
		LN<T>* pointer_to_min = to_change;
		for (LN<T>* curr = to_change->next; curr != nullptr; curr = curr->next)
			if (curr->value < pointer_to_min->value)
				pointer_to_min = curr;
		std::swap(to_change->value, pointer_to_min->value);
	}
}

////////////////////////////////////////////////////////////////////////////////

//Problem #2

//Precondition : Array values with indexes left_low  to left_high  are sorted
//Precondition : Array values with indexes right_low to right_high are sorted
//Note that left_high+1 = right_low (I pass both to make the code a bit simpler)
//Postcondition: Array values with indexes left_low  to right_high are sorted
//Hint: Merge into a temporary array (declared to be just big enough to store
//  all the needed values) and then copy that temporary array back into
//  the parameter array (between left_low to right_high inclusively)
//See the quiz for pseudocode for this method

//Write this function
void merge(int a[], int left_low, int left_high, int right_low,int right_high) {
	int low = left_low;
	int high = right_high;
	int size = right_high - left_low + 1;
	int temp [size];

	for (int i = 0; i <  size; ++i){
		if (left_low > left_high)
			temp[i] = a[right_low++];
		else if (right_low > right_high)
			temp[i] = a[left_low++];
		else if (a[left_low] <= a[right_low])
			temp[i] = a[left_low++];
		else
			temp[i] = a[right_low++];
	}

	int j = 0;
	for (int i = low; i <  high+1; ++i)
		a[i] = temp[j++];
}

////////////////////////////////////////////////////////////////////////////////

//Problem #3

int select_digit(int number, int place) {
	return number / place % 10;
}

//Write this function
void radix_sort(int a[], int length) {

	ics::ArrayQueue<int> temp [10];
	for (int i = 1; i < 1000000; i = i * 10){
		for (int j = 0; j < length; ++j){
			temp[select_digit(a[j], i)].enqueue(a[j]);
		}
		int j = -1;
		for (int i = 0; i < 10; ++i)
			while (!temp[i].empty())
				a[++j] = temp[i].dequeue();
	}
}

#endif /* Q6SOLUTION_HPP_ */
