/*
 * Aaron Lam (UniqueID: 90777)
 * Fei Fei Zhang (UniqueID: 126860)
 *
 * We certify that we worked cooperatively on this programming
 * assignment, according to the rules for pair programming
 */

#ifndef HEAP_PRIORITY_QUEUE_HPP_
#define HEAP_PRIORITY_QUEUE_HPP_

#include <string>
#include <iostream>
#include <sstream>
#include <initializer_list>
#include "ics_exceptions.hpp"
#include "iterator.hpp"
#include "priority_queue.hpp"
#include <utility>              //For std::swap function
#include "array_stack.hpp"

namespace ics {

template<class T> class HeapPriorityQueue: public PriorityQueue<T> {
	using PriorityQueue<T>::gt;  //Required because of templated classes
public:
	HeapPriorityQueue() = delete;
	explicit HeapPriorityQueue(bool (*agt)(const T& a, const T& b));
	HeapPriorityQueue(int initialLength, bool (*agt)(const T& a, const T& b));
	HeapPriorityQueue(const HeapPriorityQueue<T>& to_copy);
	HeapPriorityQueue(std::initializer_list<T> il, bool (*agt)(const T& a, const T& b));
	HeapPriorityQueue(ics::Iterator<T>& start, const ics::Iterator<T>& stop, bool (*agt)(const T& a, const T& b));
	virtual ~HeapPriorityQueue();

	virtual bool empty() const;
	virtual int size() const;
	virtual T& peek() const;
	virtual std::string str() const;

	virtual int enqueue(const T& element);
	virtual T dequeue();
	virtual void clear();

	virtual int enqueue(ics::Iterator<T>& start, const ics::Iterator<T>& stop);

	virtual HeapPriorityQueue<T>& operator =(const HeapPriorityQueue<T>& rhs);
	virtual bool operator ==(const PriorityQueue<T>& rhs) const;
	virtual bool operator !=(const PriorityQueue<T>& rhs) const;

	template<class T2>
	friend std::ostream& operator <<(std::ostream& outs, const HeapPriorityQueue<T2>& p);

	class Iterator: public ics::Iterator<T> {
	public:
		//KLUDGE should be callable only in begin/end
		Iterator(HeapPriorityQueue<T>* iterate_over, bool begin);
		Iterator(const Iterator& i);
		virtual ~Iterator();
		virtual T erase();
		virtual std::string str() const;
		virtual const ics::Iterator<T>& operator ++();
		virtual const ics::Iterator<T>& operator ++(int);
		virtual bool operator ==(const ics::Iterator<T>& rhs) const;
		virtual bool operator !=(const ics::Iterator<T>& rhs) const;
		virtual T& operator *() const;
		virtual T* operator ->() const;
	private:
		HeapPriorityQueue<T> it;     //Copy of pq to use as iterator via dequeue
		HeapPriorityQueue<T>* ref_pq;
		int expected_mod_count;
		bool can_erase = true;
	};
	virtual ics::Iterator<T>& ibegin() const;
	virtual ics::Iterator<T>& iend() const;
	virtual Iterator begin() const;
	virtual Iterator end() const;

private:
	//See base class PriorityQueue
	//bool (*gt)(const T& a, const T& b);// gt(a,b) = true iff a has higher priority than b
	T* pq;
	int length = 0;            //Physical length of the array (must be > .size()
	int used = 0;                  //Amount of array used
	int mod_count = 0;                  //For sensing concurrent modification
	void ensure_length(int new_length);
	int left_child(int i);         //Useful abstractions for heaps as arrays
	int right_child(int i);
	int parent(int i);
	bool is_root(int i);
	bool in_heap(int i);
	void percolate_up(int i);
	void percolate_down(int i);
};

//Heap helper functions:

//Make sure the length to the Max Heap array is enough to store a new element
//If not double the side of the array
//Copy the old array to the new array, delete the old array.
template<class T>
void HeapPriorityQueue<T>::ensure_length(int new_length) {
	if (length >= new_length)
		return;
	T* old_pq = pq;
	length = std::max(new_length, 2 * length);
	pq = new T[length];
	for (int i = 0; i < used; ++i)
		pq[i] = old_pq[i];
	delete[] old_pq;
}
//return index of i's left child, !no boundary check!
template<class T>
int HeapPriorityQueue<T>::left_child(int i) {
	return (2 * i) + 1;
}
//return index of i's right child, !no boundary check!
template<class T>
int HeapPriorityQueue<T>::right_child(int i) {
	return (2 * i) + 2;
}
//return index of i's parent, !no boundary check!
template<class T>
int HeapPriorityQueue<T>::parent(int i) {
	return (i - 1) / 2;
}
//return if i is root, !didn't check if i > 0!
template<class T>
bool HeapPriorityQueue<T>::is_root(int i) {
	return left_child(i) < used;
}
//return if i is in heap, 0 < i < used
template<class T>
bool HeapPriorityQueue<T>::in_heap(int i) {
	return i < used && i >= 0;
}
//percolate up, use for insert new value to heap
//Max Heap, if [i] > [i's parent], swap the 2
template<class T>
void HeapPriorityQueue<T>::percolate_up(int i) {
	for (; i > 0; i = parent(i)) {
		if (gt(pq[i], pq[parent(i)]))
			std::swap(pq[i], pq[parent(i)]);
	}
}
//percolate down, use for delete value in a heap
//Max Heap, if [i] < [i's children], swap [i] with the greater child.
//Note: If i has right child->mean it has left & right children, else, i only has left child
template<class T>
void HeapPriorityQueue<T>::percolate_down(int i) {
	if (!is_root(i))
		return;

	if (in_heap(right_child(i))){
		int max = gt(pq[left_child(i)], pq[right_child(i)]) ? left_child(i) : right_child(i);
		if (gt(pq[max], pq[i])){
			std::swap (pq[max], pq[i]);
			percolate_down(max);
		}
	}
	else{
		if (gt(pq[left_child(i)], pq[i]))
			std::swap (pq[left_child(i)], pq[i]);
	}
}
//End Heap helper functions!


//HeapPriorityQueue Constructors:

template<class T>
HeapPriorityQueue<T>::HeapPriorityQueue(bool (*agt)(const T& a, const T& b)) :
		PriorityQueue<T>(agt) {
	pq = new T[length];
}

template<class T>
HeapPriorityQueue<T>::HeapPriorityQueue(int initial_length,
		bool (*agt)(const T& a, const T& b)) :
		PriorityQueue<T>(agt), length(initial_length) {
	if (length < 0)
		length = 0;
	pq = new T[length];
}

template<class T>
HeapPriorityQueue<T>::HeapPriorityQueue(const HeapPriorityQueue<T>& to_copy) :
		PriorityQueue<T>(to_copy.gt), length(to_copy.length), used(to_copy.used) {
	pq = new T[length];
	for (int i = 0; i < to_copy.used; ++i)
		pq[i] = to_copy.pq[i];
}

template<class T>
HeapPriorityQueue<T>::HeapPriorityQueue(ics::Iterator<T>& start,
		const ics::Iterator<T>& stop, bool (*agt)(const T& a, const T& b)) :
		PriorityQueue<T>(agt) {
	pq = new T[length];
	enqueue(start, stop);
}

template<class T>
HeapPriorityQueue<T>::HeapPriorityQueue(std::initializer_list<T> il,
		bool (*agt)(const T& a, const T& b)) :
		PriorityQueue<T>(agt) {
	pq = new T[length];
	for (T pq_elem : il)
		enqueue(pq_elem);
}
//End HeapPriorityQueue Constructors!

//HeapPriorityQueue Destructor:
template<class T>
HeapPriorityQueue<T>::~HeapPriorityQueue() {
	delete[] pq;
}
//End HeapPriorityQueue Destructor!

//HeapPriorityQueue functions:

//check if queue is empty
template<class T>
inline bool HeapPriorityQueue<T>::empty() const {
	return used == 0;
}

//return used
template<class T>
int HeapPriorityQueue<T>::size() const {
	return used;
}

//return 1st node of the Max Heap
template<class T>
T& HeapPriorityQueue<T>::peek() const {
	if (empty())
		throw EmptyError("HeapPriorityQueue::peek");
	return pq[0];
}

//print out a string contain information about the queue
template<class T>
std::string HeapPriorityQueue<T>::str() const {
	std::ostringstream answer;
	answer << *this << "(length=" << length << ",used=" << used << ",mod_count="
			<< mod_count << ")";
	return answer.str();
}

//insert at the end of the Max Heap [used+1], and the percolate up
template<class T>
int HeapPriorityQueue<T>::enqueue(const T& element) {
	ensure_length(used + 1);
	pq[used] = element;
	++used;
	percolate_up(used - 1);
	++mod_count;
	return 1;
}

//enqueue from start -> stop
template<class T>
int HeapPriorityQueue<T>::enqueue(ics::Iterator<T>& start, const ics::Iterator<T>& stop) {
	int count = 0;
	for (; start != stop; ++start)
		count += enqueue(*start);
	return count;
}

//return the 1st node in the Max Heap, copy the last node to the 1st node
//decrease used (== delete last node), percolate down from the 1st node
template<class T>
T HeapPriorityQueue<T>::dequeue() {
	if (empty())
		throw EmptyError("HeapPriorityQueue::dequeue");
	T to_return = pq[0];
	pq[0] = pq[used - 1];
	--used;
	percolate_down(0);
	++mod_count;
	return to_return;
}

//set used = 0 -->make all the element in the Max Heap array available for write (enqueue)
//and make empty() return true
template<class T>
void HeapPriorityQueue<T>::clear() {
	used = 0;
	++mod_count;
}

//assign a HeapPriorityQueue from rhs to this.
//if assigning the same object->do nothing
//else ensure this length, copy gt and used, then copy all the elements in rhs.pq to this.pq
//return *this
//note: overwrite this.pq, so no need to clear it before overwriting
template<class T>
HeapPriorityQueue<T>& HeapPriorityQueue<T>::operator =( const HeapPriorityQueue<T>& rhs) {
	if (this == &rhs)
		return *this;
	ensure_length(rhs.used);
	gt = rhs.gt;  //gt is in the base class
	used = rhs.used;
	for (int i = 0; i < used; ++i)
		pq[i] = rhs.pq[i];
	++mod_count;
	return *this;
}


//compare rhs PriorityQueue to this PriorityQueue (HeapPriorityQueue)
//CANNOT compare this.pq with rhs.pq because order in Max Heap array maybe different
//make a copy of this object to a HeapPriorityQueue<T> test
//compare test.dequeue with Iterator rhs_i
template<class T>
bool HeapPriorityQueue<T>::operator ==(const PriorityQueue<T>& rhs) const {
	if (this == &rhs)
		return true;
	if (used != rhs.size())
		return false;
	if (gt != rhs.gt)
		return false;
	ics::Iterator<T>& rhs_i = rhs.ibegin();
	HeapPriorityQueue<T> test(gt);
	test = *this;
	for (int i = 0; i < used; ++i, ++rhs_i) {
		// Uses ! and ==, so != on T need not be defined
		if (!(test.dequeue() == *rhs_i))
			return false;
	}
	return true;
}

//return ! ( == )
template<class T>
bool HeapPriorityQueue<T>::operator !=(const PriorityQueue<T>& rhs) const {
	return !(*this == rhs);
}

//print out information of the priority queue from lowest to highest
template<class T>
std::ostream& operator <<(std::ostream& outs, const HeapPriorityQueue<T>& p) {
	HeapPriorityQueue<T> temp = p;
	ArrayStack<T> to_print;
	while (!temp.empty())
		to_print.push(temp.dequeue());

	outs << "priority_queue[";

	if (!p.empty()) {
		outs << to_print.pop();
		for (int i = 1; i < p.used; ++i)
			outs << "," << to_print.pop();
	}

	outs << "]:highest";
	return outs;
}

//End HeapPriorityQueue functions!


//HeapPriorityQueue Iterator:

//HeapPriorityQueue Iterator Constructors

//Initialize HeapPriorityQueue<T> it with nullptr, ref_pq = iterate_over
//if begin == true, make it a copy of the ref_pq, use it for iterate purposes
//else do nothing.
template<class T>
HeapPriorityQueue<T>::Iterator::Iterator(HeapPriorityQueue<T>* iterate_over,bool begin)
	:it(nullptr), ref_pq(iterate_over) {
	expected_mod_count = ref_pq->mod_count;
	if (begin)
		it = *ref_pq;
}

//Construct an Iterator object from an Iterator object i
template<class T>
HeapPriorityQueue<T>::Iterator::Iterator(const Iterator& i) :
		it(i.it), ref_pq(i.ref_pq), expected_mod_count(i.expected_mod_count) {
}
//End Iterator Constructors!

//Iterator destructor
template<class T>
HeapPriorityQueue<T>::Iterator::~Iterator() {
	it.clear();
	ref_pq = nullptr;
}
//End HeapPriorityQueue Iterator destructor!

//HeapPriorityQueue Iterator functions:

//Print out a string contain information about the Iterator (ref_pq)
template<class T>
std::string HeapPriorityQueue<T>::Iterator::str() const {
	std::ostringstream answer;
	answer << ref_pq->str() << "/expected_mod_count=" << expected_mod_count
			<< "/can_erase=" << can_erase;
	return answer.str();
}

//begin-end:

template<class T>
auto HeapPriorityQueue<T>::ibegin() const -> ics::Iterator<T>& {
	return *(new Iterator(const_cast<HeapPriorityQueue<T>*>(this), true));
}

template<class T>
auto HeapPriorityQueue<T>::iend() const -> ics::Iterator<T>& {
	return *(new Iterator(const_cast<HeapPriorityQueue<T>*>(this), false));
}

template<class T>
auto HeapPriorityQueue<T>::begin() const -> HeapPriorityQueue<T>::Iterator {
	return Iterator(const_cast<HeapPriorityQueue<T>*>(this), true);
}

template<class T>
auto HeapPriorityQueue<T>::end() const -> HeapPriorityQueue<T>::Iterator {
	return Iterator(const_cast<HeapPriorityQueue<T>*>(this), false);
}
//End begin-end!

//increment ++it
//use it.dequeue to move forward then return the current Iterator
template<class T>
const ics::Iterator<T>& HeapPriorityQueue<T>::Iterator::operator ++() {
	if (expected_mod_count != ref_pq->mod_count)
		throw ConcurrentModificationError(
				"HeapPriorityQueue::Iterator::operator ++");
	if (it.empty())
		return *this;
	if (!can_erase)
		can_erase = true;
	else
		it.dequeue();
	return *this;
}

//increment it++
//return the current Iterator then use it.dequeue to move forward
template<class T>
const ics::Iterator<T>& HeapPriorityQueue<T>::Iterator::operator ++(int) {
	if (expected_mod_count != ref_pq->mod_count)
		throw ConcurrentModificationError(
				"HeapPriorityQueue::Iterator::operator ++(int)");

	if (it.empty())
		return *this;
	Iterator* to_return = new Iterator(this->ref_pq, true);
	if (!can_erase)
		can_erase = true;
	else {
		to_return->it = it;
		it.dequeue();
	}
	return *to_return;
}

//If it.empty, rhsASI.empty ? true (end) : false
//Else if rhsASI.empty, false
//Else Compare it.peek vs rhsASI->it.peek (compare 2 current values)
template<class T>
bool HeapPriorityQueue<T>::Iterator::operator ==(
		const ics::Iterator<T>& rhs) const {
	const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
	if (rhsASI == 0)
		throw IteratorTypeError("HeapPriorityQueue::Iterator::operator ==");
	if (expected_mod_count != ref_pq->mod_count)
		throw ConcurrentModificationError(
				"HeapPriorityQueue::Iterator::operator ==");
	if (ref_pq != rhsASI->ref_pq)
		throw ComparingDifferentIteratorsError(
				"HeapPriorityQueue::Iterator::operator ==");

	if (it.empty())
		return rhsASI->it.empty();
	if (rhsASI->it.empty())
		return false;
	else
		return it.peek() == rhsASI->it.peek();
}

//If it.empty, false (end)
//Else if rhsASI->it.empty, true (!empty != empty)
//Else compare it.peek vs rhsASI->it.peek (compare 2 current values)
template<class T>
bool HeapPriorityQueue<T>::Iterator::operator !=(
		const ics::Iterator<T>& rhs) const {
	const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
	if (rhsASI == 0)
		throw IteratorTypeError("HeapPriorityQueue::Iterator::operator !=");
	if (expected_mod_count != ref_pq->mod_count)
		throw ConcurrentModificationError(
				"HeapPriorityQueue::Iterator::operator !=");
	if (ref_pq != rhsASI->ref_pq)
		throw ComparingDifferentIteratorsError(
				"HeapPriorityQueue::Iterator::operator !=");

	if (it.empty())
		return false;
	else if (rhsASI->it.empty())
		return true;
	else
		return it.peek() != rhsASI->it.peek();
}

//return it.peek() (current value)
template<class T>
T& HeapPriorityQueue<T>::Iterator::operator *() const {
	if (expected_mod_count != ref_pq->mod_count)
		throw ConcurrentModificationError(
				"HeapPriorityQueue::Iterator::operator *");
	if (!can_erase || it.empty()) {
		std::ostringstream where;
		where << " when size = " << ref_pq->size();
		throw IteratorPositionIllegal(
				"HeapPriorityQueue::Iterator::operator * Iterator illegal: "
						+ where.str());
	}
	return it.peek();
}

//return &it.peek() (address of current value)
template<class T>
T* HeapPriorityQueue<T>::Iterator::operator ->() const {
	if (expected_mod_count != ref_pq->mod_count)
		throw ConcurrentModificationError(
				"HeapPriorityQueue::Iterator::operator ->");
	if (!can_erase || it.empty()) {
		std::ostringstream where;
		where << " when size = " << ref_pq->size();
		throw IteratorPositionIllegal(
				"HeapPriorityQueue::Iterator::operator -> Iterator illegal: "
						+ where.str());
	}

	return &it.peek();
}

//Erase the current element that the Iterator is pointing at
//use it.dequeue to get the current value, store the value into to return
//find the index of the value in the ref_pq->pq (Max Heap), record the index
//compare the value with parent, if gt (value, parent) [value > parent] percolate_up
//else percolate_down, then return the value
template<class T>
T HeapPriorityQueue<T>::Iterator::erase()  {
	if (expected_mod_count != ref_pq->mod_count)
		throw ConcurrentModificationError(
				"ArrayPriorityQueue::Iterator::erase");
	if (!can_erase)
		throw CannotEraseError(
				"ArrayPriorityQueue::Iterator::erase Iterator cursor already erased");
	if (it.empty())
		throw CannotEraseError(
				"ArrayPriorityQueue::Iterator::erase Iterator cursor beyond data structure");

	can_erase = false;
	T to_return = it.dequeue();
	int index;
	for (int i = 0; i < ref_pq->used; ++i)
		if (ref_pq->pq[i] == to_return)
			index = i;
	ref_pq->pq[index] = ref_pq->pq[--ref_pq->used];
	int parent = ref_pq->parent(index);
	if (ref_pq->gt(ref_pq->pq[index], ref_pq->pq[parent]))
		ref_pq->percolate_up(index);
	else
		ref_pq->percolate_down(index);

	expected_mod_count = ref_pq->mod_count;
	return to_return;
}
//End HeapPriorityQueue Iterator

}
#endif /* HEAP_PRIORITY_QUEUE_HPP_ */
