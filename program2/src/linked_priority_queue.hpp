//Name: Aaron Lam
//Class: ICS 46
//program2
//	linked_priority_queue.hpp

#ifndef LINKED_PRIORITY_QUEUE_HPP_
#define LINKED_PRIORITY_QUEUE_HPP_

#include <string>
#include <iostream>
#include <sstream>
#include <initializer_list>
#include "ics_exceptions.hpp"
#include "iterator.hpp"
#include "priority_queue.hpp"
#include "array_stack.hpp"

namespace ics {

template<class T> class LinkedPriorityQueue: public PriorityQueue<T> {
	using PriorityQueue<T>::gt;  //Required because of templated classes
public:
	LinkedPriorityQueue() = delete;
	explicit LinkedPriorityQueue(bool (*agt)(const T& a, const T& b));
	LinkedPriorityQueue(const LinkedPriorityQueue<T>& to_copy);
	LinkedPriorityQueue(std::initializer_list<T> il, bool (*agt)(const T& a, const T& b));
	LinkedPriorityQueue(ics::Iterator<T>& start, const ics::Iterator<T>& stop, bool (*agt)(const T& a, const T& b));
	virtual ~LinkedPriorityQueue();

	virtual bool empty() const;
	virtual int size() const;
	virtual T& peek() const;
	virtual std::string str() const;

	virtual int enqueue(const T& element);
	virtual T dequeue();
	virtual void clear();

	virtual int enqueue(ics::Iterator<T>& start, const ics::Iterator<T>& stop);

	virtual LinkedPriorityQueue<T>& operator =(const LinkedPriorityQueue<T>& rhs);
	virtual bool operator ==(const PriorityQueue<T>& rhs) const;
	virtual bool operator !=(const PriorityQueue<T>& rhs) const;

	template<class T2>
	friend std::ostream& operator <<(std::ostream& outs, const LinkedPriorityQueue<T2>& s);

private:
	class LN;

public:
	class Iterator: public ics::Iterator<T> {
	public:
		//KLUDGE should be callable only in begin/end
		Iterator(LinkedPriorityQueue<T>* fof, LN* initial);
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
		LN* prev;     //if header, then current is at front of list
		LN* current;  //if can_erase is false, this value is unusable
		LinkedPriorityQueue<T>* ref_pq;
		int expected_mod_count;
		bool can_erase = true;
	};

	//For explicit use: Iterator<...>& it = c.ibegin(); ... or for (Iterator<...>& it = c.ibegin(); it != c.iend(); ++it)...
	virtual ics::Iterator<T>& ibegin() const;
	virtual ics::Iterator<T>& iend() const;

	//For implicit use: for (... i : c)...
	virtual Iterator begin() const;
	virtual Iterator end() const;

private:
	class LN {
	public:
		LN() {}
		LN(const LN& ln) : value(ln.value), next(ln.next) {}
		LN(T v, LN* n = nullptr) : value(v), next(n) {}

		T value;
		LN* next = nullptr;
	};

	//See base class PriorityQueue
	//bool (*gt)(const T& a, const T& b);// gt(a,b) = true iff a has higher priority than b
	int used = 0;
	LN* front = new LN();
	int mod_count = 0;                  //For sensing concurrent modification
	void delete_list(LN*& front); //Recycle storage, set front's argument to nullptr;
};

//See code in array_priority_queue.hpp and linked_queue.hpp

//Write the constructors, methods, and operators here for LinkedPriorityQueue
//Fill in the missing parts of the erase method and ++ operators
//  for LinkedPriorityQueue's Iterator
template<class T>
void LinkedPriorityQueue<T>::delete_list(LN*& front) {
	front == nullptr ? delete front : delete_list(front->next);
	used = 0;
}


template<class T>
LinkedPriorityQueue<T>::LinkedPriorityQueue(bool (*agt)(const T& a, const T& b))
	:PriorityQueue<T>(agt) {
	front = new LN ();
}


template<class T>
LinkedPriorityQueue<T>::LinkedPriorityQueue(const LinkedPriorityQueue<T>& to_copy)
	:PriorityQueue<T>(to_copy.gt) {
	front = new LN ();
	for (auto it : to_copy)
		enqueue (it);
}


template<class T>
LinkedPriorityQueue<T>::LinkedPriorityQueue(ics::Iterator<T>& start,
		const ics::Iterator<T>& stop, bool (*agt)(const T& a, const T& b)) :
		PriorityQueue<T>(agt) {
	front = new LN ();
	enqueue(start, stop);
}

template<class T>
LinkedPriorityQueue<T>::LinkedPriorityQueue(std::initializer_list<T> il,
		bool (*agt)(const T& a, const T& b)) :
		PriorityQueue<T>(agt) {
	for (T pq_elem : il)
		enqueue(pq_elem);
}

template<class T>
LinkedPriorityQueue<T>::~LinkedPriorityQueue() {
	delete_list(front);
	delete front;
}

template<class T>
inline bool LinkedPriorityQueue<T>::empty() const {
	return used == 0;
}

template<class T>
int LinkedPriorityQueue<T>::size() const {
	return used;
}

template<class T>
T& LinkedPriorityQueue<T>::peek() const {
	if (empty())
		throw EmptyError("LinkedPriorityQueue::peek");
	return front->next->value;
}

template<class T>
std::string LinkedPriorityQueue<T>::str() const {
	std::ostringstream answer;
	answer << *this << "(used=" << used << ",mod_count=" << mod_count << ")";
	return answer.str();
}

template<class T>
int LinkedPriorityQueue<T>::enqueue(const T& element) {
	LN* curr = front;
	for (; curr-> next != nullptr; curr = curr->next)
		if (gt (element, curr->next->value))
			break;
	curr->next = new LN (element, curr->next);
	++used;
	++mod_count;
	return 1;
}

template<class T>
T LinkedPriorityQueue<T>::dequeue() {
	if (this->empty())
		throw EmptyError("LinkedPriorityQueue::dequeue");
	T answer = front->next->value;
	LN* to_erase = front->next;
	front->next = to_erase->next;
	delete to_erase;
	--used;
	++mod_count;
	return answer;
}

template<class T>
void LinkedPriorityQueue<T>::clear() {
	delete_list (front);
	front = new LN ();
	used = 0;
	++mod_count;
}

template<class T>
int LinkedPriorityQueue<T>::enqueue(ics::Iterator<T>& start, const ics::Iterator<T>& stop) {
	int count = 0;
	for (; start != stop; ++start)
		count += enqueue(*start);
	return count;
}

template<class T>
LinkedPriorityQueue<T>& LinkedPriorityQueue<T>::operator = (const LinkedPriorityQueue<T>& rhs) {
	if (this == &rhs)
		return *this;
	this->clear();
	gt = rhs.gt;  //gt is in the base class
	enqueue (rhs.ibegin(), rhs.iend());
	return *this;
}

template<class T>
bool LinkedPriorityQueue<T>::operator == (const PriorityQueue<T>& rhs) const {
	if (this == &rhs)
		return true;
	if (used != rhs.size())
		return false;
	if (gt != rhs.gt)
		return false;
	if (used != rhs.size())
		return false;
	//KLUDGE: should check for same == function used to prioritize, but cannot unless
	//  it is made part of the PriorityQueue class (should it be? protected)?
	ics::Iterator<T>& rhs_i = rhs.ibegin();
	for (auto& it = this->ibegin(); it != this->iend(); ++it, ++rhs_i)
		// Uses ! and ==, so != on T need not be defined
		if (!(*it == *rhs_i))
			return false;

	return true;
}

template<class T>
bool LinkedPriorityQueue<T>::operator !=(const PriorityQueue<T>& rhs) const {
	return !(*this == rhs);
}

template<class T>
std::ostream& operator <<(std::ostream& outs, const LinkedPriorityQueue<T>& p) {
	ArrayStack<T> print (p.ibegin(), p.iend());
	outs << "priority_queue[";
	if (!p.empty()) {
		outs << print.pop();
		for (auto temp : print)
			outs << "," << temp;
	}
	outs << "]:highest";
	return outs;
}


//KLUDGE: memory-leak
template<class T>
auto LinkedPriorityQueue<T>::ibegin() const -> ics::Iterator<T>& {
	return *(new Iterator(const_cast<LinkedPriorityQueue<T>*>(this), front->next));
}

//KLUDGE: memory-leak
template<class T>
auto LinkedPriorityQueue<T>::iend() const -> ics::Iterator<T>& {
	return *(new Iterator(const_cast<LinkedPriorityQueue<T>*>(this), nullptr));
}

template<class T>
auto LinkedPriorityQueue<T>::begin() const -> LinkedPriorityQueue<T>::Iterator {
	return Iterator(const_cast<LinkedPriorityQueue<T>*>(this), front->next);
}

template<class T>
auto LinkedPriorityQueue<T>::end() const -> LinkedPriorityQueue<T>::Iterator {
	return Iterator(const_cast<LinkedPriorityQueue<T>*>(this), nullptr);
}

template<class T>
LinkedPriorityQueue<T>::Iterator::Iterator(LinkedPriorityQueue<T>* fof, LN* initial)
	: current(initial), ref_pq(fof) {
	prev = ref_pq->front;
	expected_mod_count = ref_pq->mod_count;
}

template<class T>
LinkedPriorityQueue<T>::Iterator::~Iterator() {
}

template<class T>
T LinkedPriorityQueue<T>::Iterator::erase() {
	if (expected_mod_count != ref_pq->mod_count)
		throw ConcurrentModificationError(
				"LinkedPriorityQueue::Iterator::erase");
	if (!can_erase)
		throw CannotEraseError(
				"LinkedPriorityQueue::Iterator::erase Iterator cursor already erased");
	if (current == nullptr)
		throw CannotEraseError(
				"LinkedPriorityQueue::Iterator::erase Iterator cursor beyond data structure");

	can_erase = false;
	T to_return = current->value;
	prev->next = current->next;
	delete current;
	current = prev->next;
	--ref_pq->used;
	expected_mod_count = ref_pq->mod_count;
	return to_return;
}

template<class T>
std::string LinkedPriorityQueue<T>::Iterator::str() const {
	std::ostringstream answer;
	answer << ref_pq->str() << "(current=" << current
			<< ",expected_mod_count=" << expected_mod_count << ",can_erase="
			<< can_erase << ")";
	return answer.str();
}

template<class T>
const ics::Iterator<T>& LinkedPriorityQueue<T>::Iterator::operator ++() {
	if (expected_mod_count != ref_pq->mod_count)
		throw ConcurrentModificationError(
				"LinkedPriorityQueue::Iterator::operator ++");

	if (current == nullptr)
		return *this;

	if (!can_erase)
		can_erase = true;
	else{
		prev = current;
		current = current->next;
	}

	return *this;
}

//KLUDGE: can create garbage! (can return local value!)
template<class T>
const ics::Iterator<T>& LinkedPriorityQueue<T>::Iterator::operator ++(int) {
	if (expected_mod_count != ref_pq->mod_count)
		throw ConcurrentModificationError(
				"LinkedPriorityQueue::Iterator::operator ++(int)");

	if (current == nullptr)
		return *this;

	Iterator* to_return = new Iterator(this->ref_pq, nullptr);
	if (!can_erase)
		can_erase = true;
	else {
		to_return->current = current;
		prev = current;
		current = current->next;
	}

	return *to_return;
}

template<class T>
bool LinkedPriorityQueue<T>::Iterator::operator ==(
		const ics::Iterator<T>& rhs) const {
	const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
	if (rhsASI == 0)
		throw IteratorTypeError("LinkedPriorityQueue::Iterator::operator ==");
	if (expected_mod_count != ref_pq->mod_count)
		throw ConcurrentModificationError(
				"LinkedPriorityQueue::Iterator::operator ==");
	if (ref_pq != rhsASI->ref_pq)
		throw ComparingDifferentIteratorsError(
				"LinkedPriorityQueue::Iterator::operator ==");

	return current == rhsASI->current;
}

template<class T>
bool LinkedPriorityQueue<T>::Iterator::operator !=(
		const ics::Iterator<T>& rhs) const {
	const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
	if (rhsASI == 0)
		throw IteratorTypeError("LinkedPriorityQueue::Iterator::operator !=");
	if (expected_mod_count != ref_pq->mod_count)
		throw ConcurrentModificationError(
				"LinkedPriorityQueue::Iterator::operator !=");
	if (ref_pq != rhsASI->ref_pq)
		throw ComparingDifferentIteratorsError(
				"LinkedPriorityQueue::Iterator::operator !=");

	return current != rhsASI->current;
}

template<class T>
T& LinkedPriorityQueue<T>::Iterator::operator *() const {
	if (expected_mod_count != ref_pq->mod_count)
		throw ConcurrentModificationError(
				"LinkedPriorityQueue::Iterator::operator *");
	if (!can_erase || current == nullptr) {
		std::ostringstream where;
		where << current << " when front = " << ref_pq->front;
		throw IteratorPositionIllegal(
				"LinkedPriorityQueue::Iterator::operator * Iterator illegal: "
						+ where.str());
	}

	return current->value;
}

template<class T>
T* LinkedPriorityQueue<T>::Iterator::operator ->() const {
	if (expected_mod_count != ref_pq->mod_count)
		throw ConcurrentModificationError(
				"LinkedPriorityQueue::Iterator::operator *");
	if (!can_erase || current == nullptr) {
		std::ostringstream where;
		where << current << " when front = " << ref_pq->front;
		throw IteratorPositionIllegal(
				"LinkedPriorityQueue::Iterator::operator * Iterator illegal: "
						+ where.str());
	}

	return &(current->value);
}

}

#endif /* LINKED_PRIORITY_QUEUE_HPP_ */
