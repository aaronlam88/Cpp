//Name: Aaron Lam
//Class: ICS 46
//program2
//	linked_set.hpp
#ifndef LINKED_SET_HPP_
#define LINKED_SET_HPP_

#include <string>
#include <iostream>
#include <sstream>
#include <initializer_list>
#include "ics_exceptions.hpp"
#include "iterator.hpp"
#include "set.hpp"

namespace ics {

template<class T> class LinkedSet: public Set<T> {
public:
	LinkedSet();
	LinkedSet(const LinkedSet<T>& to_copy);
	LinkedSet(std::initializer_list<T> il);
	LinkedSet(ics::Iterator<T>& start, const ics::Iterator<T>& stop);
	virtual ~LinkedSet();

	virtual bool empty() const;
	virtual int size() const;
	virtual bool contains(const T& element) const;
	virtual std::string str() const;

	virtual bool contains(ics::Iterator<T>& start, const ics::Iterator<T>& stop) const;

	virtual int insert(const T& element);
	virtual int erase(const T& element);
	virtual void clear();

	virtual int insert(ics::Iterator<T>& start, const ics::Iterator<T>& stop);
	virtual int erase(ics::Iterator<T>& start, const ics::Iterator<T>& stop);
	virtual int retain(ics::Iterator<T>& start, const ics::Iterator<T>& stop);

	virtual LinkedSet<T>& operator =(const LinkedSet<T>& rhs);
	virtual bool operator ==(const Set<T>& rhs) const;
	virtual bool operator !=(const Set<T>& rhs) const;
	virtual bool operator <=(const Set<T>& rhs) const;
	virtual bool operator <(const Set<T>& rhs) const;
	virtual bool operator >=(const Set<T>& rhs) const;
	virtual bool operator >(const Set<T>& rhs) const;

	template<class T2>
	friend std::ostream& operator <<(std::ostream& outs,
			const LinkedSet<T2>& s);

private:
	class LN;

public:
	class Iterator: public ics::Iterator<T> {
	public:
		//KLUDGE should be callable only in begin/end
		Iterator(LinkedSet<T>* fof, LN* initial);
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
		LN* current;  //if can_erase is false, this value is unusable
		LinkedSet<T>* ref_set;
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

	int used = 0;             //Cache of # values in set
	LN* front = new LN();
	LN* trailer = front;
	int mod_count = 0;             //For sensing concurrent modification
	int erase_at(LN* p);
	void delete_list(LN*& front); //Recycle storage, set front's argument to nullptr;
};

//See code in array_set.hpp and linked_queue.hpp and linked_priority_queue.hpp

//Write the constructors, methods, and operators here for LinkedSet

//Write the constructor, destructor, methods, and operators here for
//  LinkedSet's Iterator

template<class T>
void LinkedSet<T>::delete_list(LN*& front) {
	front != nullptr ? delete_list(front->next) : delete front;
	front = trailer = nullptr;
	used = 0;
}

template<class T>
LinkedSet<T>::LinkedSet() {
}

template<class T>
LinkedSet<T>::LinkedSet(const LinkedSet<T>& to_copy) {
	for (auto it : to_copy)
		insert(it);
}

template<class T>
LinkedSet<T>::LinkedSet(ics::Iterator<T>& start, const ics::Iterator<T>& stop) {
	insert(start, stop);
}

template<class T>
LinkedSet<T>::LinkedSet(std::initializer_list<T> il) {
	for (T s_elem : il)
		insert(s_elem);
}

template<class T>
LinkedSet<T>::~LinkedSet() {
	delete_list(front);
}

template<class T>
inline bool LinkedSet<T>::empty() const {
	return used == 0;
}

template<class T>
int LinkedSet<T>::size() const {
	return used;
}

template<class T>
bool LinkedSet<T>::contains(const T& element) const {
	for (LN* temp = front; temp != trailer; temp = temp->next)
		if (temp->value == element)
			return true;

	return false;
}

template<class T>
std::string LinkedSet<T>::str() const {
	std::ostringstream answer;
	answer << *this << "(used=" << used << ",mod_count=" << mod_count << ")";
	return answer.str();
}

template<class T>
bool LinkedSet<T>::contains(ics::Iterator<T>& start,
		const ics::Iterator<T>& stop) const {
	for (; start != stop; ++start)
		if (!contains(*start))
			return false;

	return true;
}

template<class T>
int LinkedSet<T>::insert(const T& element) {
	if (contains(element))
		return 0;
	front = new LN(element, front);
	++used;
	++mod_count;
	return 1;
}

template<class T>
int LinkedSet<T>::erase(const T& element) {
	for (auto it = begin(); it != end(); ++it)
		if (*it == element){
			it.erase();
			return 1;
		}

	return 0;
}

template<class T>
void LinkedSet<T>::clear() {
	delete_list(front);
	used = 0;
	++mod_count;
}

template<class T>
int LinkedSet<T>::insert(ics::Iterator<T>& start,
		const ics::Iterator<T>& stop) {
	int count = 0;
	for (; start != stop; ++start)
		count += insert(*start);

	return count;
}

template<class T>
int LinkedSet<T>::erase(ics::Iterator<T>& start, const ics::Iterator<T>& stop) {
	int count = 0;
	for (; start != stop; start++)
		count += erase(*start);
	return count;
}

template<class T>
int LinkedSet<T>::retain(ics::Iterator<T>& start,
		const ics::Iterator<T>& stop) {
	LinkedSet<T> s(start, stop);
	int count = 0;
	for (auto& it = ibegin(); it != iend(); ++it)
		if (!s.contains(*it))
			it.erase();
		else
			++count;

	return count;
}

template<class T>
LinkedSet<T>& LinkedSet<T>::operator = (const LinkedSet<T>& rhs) {
	if (this == &rhs)
		return *this;
	delete_list(front);
	insert(rhs.ibegin(), rhs.iend());
	++mod_count;
	return *this;
}

template<class T>
bool LinkedSet<T>::operator == (const Set<T>& rhs) const {
	if (this == &rhs)
		return true;
	if (used != rhs.size())
		return false;
	for (LN* temp = front; temp != trailer; temp = temp->next)
		if (!rhs.contains(temp->value))
			return false;

	return true;
}

template<class T>
bool LinkedSet<T>::operator !=(const Set<T>& rhs) const {
	return !(*this == rhs);
}


template<class T>
bool LinkedSet<T>::operator <=(const Set<T>& rhs) const {
	if (this == &rhs)
		return true;
	if (used > rhs.size())
		return false;
	for (LN* temp = front; temp != trailer; temp = temp->next)
		if (!rhs.contains(temp->value))
			return false;

	return true;
}


template<class T>
bool LinkedSet<T>::operator <(const Set<T>& rhs) const {
	if (this == &rhs)
		return false;
	if (used >= rhs.size())
		return false;
	for (LN* temp = front; temp != trailer; temp = temp->next)
		if (!rhs.contains(temp->value))
			return false;

	return true;
}


template<class T>
bool LinkedSet<T>::operator >= (const Set<T>& rhs) const {
  return rhs <= *this;
}


template<class T>
bool LinkedSet<T>::operator > (const Set<T>& rhs) const {
  return rhs < *this;
}


template<class T>
std::ostream& operator <<(std::ostream& outs, const LinkedSet<T>& s) {
	outs << "set[";

	if (!s.empty()) {
		outs << s.front->value;
		for (auto temp = s.front->next; temp != s.trailer; temp = temp->next)
			outs << "," << temp->value;
	}

	outs << "]";
	return outs;
}

//KLUDGE: memory-leak
template<class T>
auto LinkedSet<T>::ibegin() const -> ics::Iterator<T>& {
	return *(new Iterator(const_cast<LinkedSet<T>*>(this), front));
}

//KLUDGE: memory-leak
template<class T>
auto LinkedSet<T>::iend() const -> ics::Iterator<T>& {
	return *(new Iterator(const_cast<LinkedSet<T>*>(this), trailer));
}

template<class T>
auto LinkedSet<T>::begin() const -> LinkedSet<T>::Iterator {
	return Iterator(const_cast<LinkedSet<T>*>(this), front);
}

template<class T>
auto LinkedSet<T>::end() const -> LinkedSet<T>::Iterator {
	return Iterator(const_cast<LinkedSet<T>*>(this), trailer);
}

template<class T>
int LinkedSet<T>::erase_at(LN* p) {
  LN* prev = front;
  LN* curr = front;
  for (; curr!= p; prev = curr, curr = curr->next);
  if (curr == front)
	  front = front->next;
  else
	  prev->next = curr->next;
  delete p;
  --used;
  ++mod_count;
  return 1;
}

template<class T>
LinkedSet<T>::Iterator::Iterator(LinkedSet<T>* iterate_over, LN* initial)
	: current(initial), ref_set(iterate_over) {
	expected_mod_count = ref_set->mod_count;
}

template<class T>
LinkedSet<T>::Iterator::~Iterator() {
}

template<class T>
T LinkedSet<T>::Iterator::erase() {
	if (expected_mod_count != ref_set->mod_count)
		throw ConcurrentModificationError("LinkedSet::Iterator::erase");
	if (!can_erase)
		throw CannotEraseError(
				"LinkedSet::Iterator::erase Iterator cursor already erased");
	if (current == ref_set->trailer)
		throw CannotEraseError(
				"LinkedSet::Iterator::erase Iterator cursor beyond data structure");

	can_erase = false;
	T to_return = current->value;
	LN* temp = current->next;
	ref_set->erase_at(current);
	current = temp;
	expected_mod_count = ref_set->mod_count;
	return to_return;
}

template<class T>
std::string LinkedSet<T>::Iterator::str() const {
	std::ostringstream answer;
	answer << ref_set->str() << "(current=" << current << ",expected_mod_count="
			<< expected_mod_count << ",can_erase=" << can_erase << ")";
	return answer.str();
}

template<class T>
const ics::Iterator<T>& LinkedSet<T>::Iterator::operator ++() {
	if (expected_mod_count != ref_set->mod_count)
		throw ConcurrentModificationError("LinkedSet::Iterator::operator ++");

	if (current == ref_set->trailer)
		return *this;

	if (!can_erase)
		can_erase = true;
	else
		current = current->next;

	return *this;
}

//KLUDGE: can create garbage! (can return local value!)
template<class T>
const ics::Iterator<T>& LinkedSet<T>::Iterator::operator ++(int) {
	if (expected_mod_count != ref_set->mod_count)
		throw ConcurrentModificationError(
				"LinkedSet::Iterator::operator ++(int)");

	if (current == ref_set->trailer)
		return *this;

	Iterator* to_return = new Iterator(this->ref_set, current - 1);
	if (!can_erase)
		can_erase = true;
	else {
		++to_return->current;
		current = current->next;
	}

	return *to_return;
}

template<class T>
bool LinkedSet<T>::Iterator::operator ==(const ics::Iterator<T>& rhs) const {
	const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
	if (rhsASI == 0)
		throw IteratorTypeError("LinkedSet::Iterator::operator ==");
	if (expected_mod_count != ref_set->mod_count)
		throw ConcurrentModificationError("LinkedSet::Iterator::operator ==");
	if (ref_set != rhsASI->ref_set)
		throw ComparingDifferentIteratorsError(
				"LinkedSet::Iterator::operator ==");

	return current == rhsASI->current;
}

template<class T>
bool LinkedSet<T>::Iterator::operator !=(const ics::Iterator<T>& rhs) const {
	const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
	if (rhsASI == 0)
		throw IteratorTypeError("LinkedSet::Iterator::operator !=");
	if (expected_mod_count != ref_set->mod_count)
		throw ConcurrentModificationError("LinkedSet::Iterator::operator !=");
	if (ref_set != rhsASI->ref_set)
		throw ComparingDifferentIteratorsError(
				"LinkedSet::Iterator::operator !=");

	return current != rhsASI->current;
}

template<class T>
T& LinkedSet<T>::Iterator::operator *() const {
	if (expected_mod_count != ref_set->mod_count)
		throw ConcurrentModificationError("LinkedSet::Iterator::operator *");
	if (!can_erase || current == nullptr) {
		std::ostringstream where;
		where << current << " when size = " << ref_set->size();
		throw IteratorPositionIllegal(
				"LinkedSet::Iterator::operator * Iterator illegal: "
						+ where.str());
	}

	return current->value;
}

template<class T>
T* LinkedSet<T>::Iterator::operator ->() const {
	if (expected_mod_count != ref_set->mod_count)
		throw ConcurrentModificationError("LinkedSet::Iterator::operator ->");
	if (!can_erase || current == nullptr) {
		std::ostringstream where;
		where << current << " when size = " << ref_set->size();
		throw IteratorPositionIllegal(
				"LinkedSet::Iterator::operator -> Iterator illegal: "
						+ where.str());
	}

	return &current->value;
}

}

#endif /* LINKED_SET_HPP_ */
