/* Aaron Lam (UniqueID: 90777)*/

#ifndef HASH_SET_HPP_
#define HASH_SET_HPP_

#include <string>
#include <iostream>
#include <sstream>
#include <initializer_list>
#include "ics_exceptions.hpp"
#include "pair.hpp"
#include "iterator.hpp"
#include "set.hpp"


namespace ics {

template<class T> class HashSet : public Set<T>	{
  public:
    HashSet() = delete;
    HashSet(int (*ahash)(const T& element), double the_load_factor = 1.0);
    HashSet(int initial_bins, int (*ahash)(const T& element), double the_load_factor = 1.0);
    HashSet(const HashSet<T>& to_copy);
    HashSet(std::initializer_list<T> il, int (*ahash)(const T& element), double the_load_factor = 1.0);
    HashSet(ics::Iterator<T>& start, const ics::Iterator<T>& stop, int (*ahash)(const T& element), double the_load_factor = 1.0);
    virtual ~HashSet();

    virtual bool empty      () const;
    virtual int  size       () const;
    virtual bool contains   (const T& element) const;
    virtual std::string str () const;

    virtual bool contains (ics::Iterator<T>& start, const ics::Iterator<T>& stop) const;

    virtual int  insert (const T& element);
    virtual int  erase  (const T& element);
    virtual void clear  ();

    virtual int insert (ics::Iterator<T>& start, const ics::Iterator<T>& stop);
    virtual int erase  (ics::Iterator<T>& start, const ics::Iterator<T>& stop);
    virtual int retain (ics::Iterator<T>& start, const ics::Iterator<T>& stop);

    virtual HashSet<T>& operator = (const HashSet<T>& rhs);
    virtual bool operator == (const Set<T>& rhs) const;
    virtual bool operator != (const Set<T>& rhs) const;
    virtual bool operator <= (const Set<T>& rhs) const;
    virtual bool operator <  (const Set<T>& rhs) const;
    virtual bool operator >= (const Set<T>& rhs) const;
    virtual bool operator >  (const Set<T>& rhs) const;

    template<class T2>
    friend std::ostream& operator << (std::ostream& outs, const HashSet<T2>& s);

    virtual ics::Iterator<T>& ibegin () const;
    virtual ics::Iterator<T>& iend   () const;

  private:
    class LN;

  public:
    class Iterator : public ics::Iterator<T> {
      public:
        //KLUDGE should be callable only in begin/end
        Iterator(HashSet<T>* iterate_over, bool begin);
        Iterator(const Iterator& i);
        virtual ~Iterator();
        virtual T           erase();
        virtual std::string str  () const;
        virtual const ics::Iterator<T>& operator ++ ();
        virtual const ics::Iterator<T>& operator ++ (int);
        virtual bool operator == (const ics::Iterator<T>& rhs) const;
        virtual bool operator != (const ics::Iterator<T>& rhs) const;
        virtual T& operator *  () const;
        virtual T* operator -> () const;
      private:
        ics::pair<int,LN*> current; //Bin Index/Cursor; stop: LN* == nullptr
        HashSet<T>*        ref_set;
        int                expected_mod_count;
        bool               can_erase = true;
        void advance_cursors();
    };

    virtual Iterator begin () const;
    virtual Iterator end   () const;

  private:
    class LN {
      public:
        LN ()                      : next(nullptr){}
        LN (const LN& ln)          : value(ln.value), next(ln.next){}
        LN (T v,  LN* n = nullptr) : value(v), next(n){}

        T   value;
        LN* next;
    };

    LN** set      = nullptr;
    int (*hash)(const T& element);
    double load_factor;//used/bins <= load_factor
    int bins      = 1; //# bins available in the array
    int used      = 0; //# of key->value pairs in the hash table
    int mod_count = 0; //For sensing concurrent modification
    int   hash_compress (const T& element) const;
    void  ensure_load_factor(int new_used);
    LN*   find_element (int bin, const T& element) const;
    LN*   copy_list(LN*   l) const;
    LN**  copy_hash_table(LN** ht, int bins) const;
    void  delete_hash_table(LN**& ht, int bins);
  };

//Hash Set helper functions:

template<class T>
int HashSet<T>::hash_compress (const T& element) const {
	unsigned int index = hash (element);
	return index % bins;
}

template<class T>
void HashSet<T>::ensure_load_factor(int new_used) {
	if (new_used / bins <= load_factor)
		return;
	bins *= 2; //update #of bins
	LN** new_set = new LN*[bins]; //create new_map with 2x the #of bins
	for (int i = 0; i < bins; ++i)
		new_set[i] = new LN(); //add trailer for bins
	for (int i = 0; i < bins/2; ++i)
		for (LN* bin_ptr = set[i]; bin_ptr->next != nullptr; ) {
			LN* to_copy = bin_ptr;
			LN* next = bin_ptr->next;
			int index = hash_compress(to_copy->value);
			to_copy->next = new_set[index];
			new_set[index] = to_copy;
			bin_ptr = next;
		}
	delete[] set;
	set = new_set;
}

template<class T>
typename HashSet<T>::LN* HashSet<T>::find_element(int bin, const T& element) const {
	for (LN* c = set[bin]; c->next != nullptr; c = c->next)
		if (element == c->value)
			return c;

	return nullptr;
}

template<class T>
typename HashSet<T>::LN* HashSet<T>::copy_list(LN* l) const {
	if (l == nullptr)
		return nullptr;
	else
		return new LN(l->value, copy_list(l->next));
}

template<class T>
typename HashSet<T>::LN** HashSet<T>::copy_hash_table (LN** ht, int bins) const {
	LN** new_ht = new LN*[bins];
	for (int i = 0; i < bins; ++i)
		new_ht[i] = copy_list(ht[i]);
	return new_ht;
}

template<class T>
void HashSet<T>::delete_hash_table (LN**& ht, int bins) {
	for (int i = 0; i < bins; ++i)
		for (LN* bin = ht[i]; bin != nullptr;) {
			LN* temp = bin->next;
			delete bin;
			bin = temp;
		}
	delete [] ht;
	ht = nullptr;
}

//End Hash Set helper functions!

//Hash Set constructors:

template<class T>
HashSet<T>::HashSet(int (*ahash)(const T& element), double the_load_factor)
    : hash(ahash), load_factor(the_load_factor) {
	set = new LN*[bins];
	for (int i = 0; i < bins; ++i)
		set[i] = new LN();
}

template<class T>
HashSet<T>::HashSet(int initial_bins, int (*ahash)(const T& element), double the_load_factor)
    : bins(initial_bins), hash(ahash), load_factor(the_load_factor) {
	set = new LN*[bins];
	for (int i = 0; i < bins; ++i)
		set[i] = new LN();
}

template<class T>
HashSet<T>::HashSet(const HashSet<T>& to_copy)
    : hash(to_copy.hash), load_factor(to_copy.load_factor), bins(to_copy.bins), used(to_copy.used) {
	set = copy_hash_table(to_copy.set, bins);
}

template<class T>
HashSet<T>::HashSet(ics::Iterator<T>& start, const ics::Iterator<T>& stop, int (*ahash)(const T& element), double the_load_factor)
    : hash(ahash), load_factor(the_load_factor) {
	set = new LN*[bins];
	for (int i = 0; i < bins; ++i)
		set[i] = new LN();
	insert(start, stop);
}

template<class T>
HashSet<T>::HashSet(std::initializer_list<T> il,int (*ahash)(const T& k), double the_load_factor)
    : hash(ahash), load_factor(the_load_factor) {
	set = new LN*[bins];
	for (int i = 0; i < bins; ++i)
		set[i] = new LN();
	for (T s_entry : il)
		insert(s_entry);
}
//End HashMap Constructors!

//HashMap Destructor
template<class T>
HashSet<T>::~HashSet() {
	delete_hash_table(set, bins);
}
//End Hash Set Destructor!

//Hash Set functions:

template<class T>
inline bool HashSet<T>::empty() const {
	return used == 0;
}

template<class T>
int HashSet<T>::size() const {
	return used;
}

template<class T>
bool HashSet<T>::contains (const T& element) const {
	int bin = hash_compress(element);
	return find_element(bin, element) != nullptr;
}

template<class T>
std::string HashSet<T>::str() const {
	std::ostringstream answer;
	answer << std::endl;
	for (int i = 0; i < bins; ++i){
		answer << "bin[" << i << "]: ";
		for (LN* bin_ptr = set[i]; bin_ptr != nullptr; bin_ptr = bin_ptr->next){
			if (bin_ptr->next == nullptr)
				answer << "#";
			else
				answer << "[" << bin_ptr->value  << "] -> ";
		}
		answer << std::endl;
	}
	return answer.str();
}

template<class T>
bool HashSet<T>::contains(ics::Iterator<T>& start, const ics::Iterator<T>& stop) const {
	for (;start != stop; ++start){
		int bin = hash_compress(*start);
		if (find_element (bin, *start) == nullptr)
			return false;
	}
	return true;
}

template<class T>
int HashSet<T>::insert(const T& element) {
	int bin = hash_compress(element);
	LN* found = find_element(bin, element);
	if (found != nullptr)
		return 0;

	++used;
	++mod_count;
	ensure_load_factor(used);
	int index = hash_compress(element);
	set[index] = new LN(element, set[index]);

	return 1;
}

template<class T>
int HashSet<T>::erase(const T& element) {
	int bin = hash_compress(element);
	LN* found = find_element(bin, element);
	if (found == nullptr) {
		return 0;
	} else {
		--used;
		++mod_count;
		LN* to_delete = found->next;
		found->value = found->next->value;
		found->next = found->next->next;
		delete to_delete;
		return 1;
	}
}

template<class T>
void HashSet<T>::clear() {
	delete_hash_table(set, bins);
	used = 0;
	bins = 1;
	set = new LN* [bins];
	set[0] = new LN ();
	++mod_count;
}

template<class T>
int HashSet<T>::insert(ics::Iterator<T>& start, const ics::Iterator<T>& stop) {
	int count = 0;
	for (; start != stop; ++start) {
		++count;
		insert(*start);
	}

	return count;
}

template<class T>
int HashSet<T>::erase(ics::Iterator<T>& start, const ics::Iterator<T>& stop) {
	int count = 0;
	for (; start != stop; ++start) {
		++count;
		erase(*start);
	}

	return count;
}

template<class T>
int HashSet<T>::retain(ics::Iterator<T>& start, const ics::Iterator<T>& stop) {
	HashSet<T> s(start, stop, hash, load_factor);
	int count = 0;
	for (int i = 0; i < bins; i++)
		for (LN* bin = set[i]; bin->next != nullptr;) {
			if (!s.contains(bin->value)) {
				erase(bin->value);
			} else{
				++count;
				bin = bin->next;
			}
		}

	return count;
}

template<class T>
HashSet<T>& HashSet<T>::operator = (const HashSet<T>& rhs) {
	if (this == &rhs)
		return *this;
	ensure_load_factor(rhs.used);
	used = rhs.used;
	set = copy_hash_table(rhs.set, bins);
	++mod_count;
	return *this;
}

template<class T>
bool HashSet<T>::operator == (const Set<T>& rhs) const {
	if (this == &rhs)
		return true;
	if (used != rhs.size())
		return false;
	for (int i = 0; i < bins; ++i)
		for (LN* bin_ptr = set[i]; bin_ptr->next != nullptr; bin_ptr = bin_ptr->next)
			if (!rhs.contains(bin_ptr->value))
				return false;
	return true;
}

template<class T>
bool HashSet<T>::operator != (const Set<T>& rhs) const {
	return !(*this == rhs);
}

template<class T>
bool HashSet<T>::operator <=(const Set<T>& rhs) const {
	if (this == &rhs)
		return true;
	if (used > rhs.size())
		return false;
	for (int i = 0; i < bins; ++i)
		for (LN* bin_ptr = set[i]; bin_ptr->next != nullptr; bin_ptr = bin_ptr->next)
			if (!rhs.contains(bin_ptr->value))
				return false;

	return true;
}

template<class T>
bool HashSet<T>::operator < (const Set<T>& rhs) const {
	if (this == &rhs)
		return false;
	if (used >= rhs.size())
		return false;
	for (int i = 0; i < bins; ++i)
		for (LN* bin_ptr = set[i]; bin_ptr->next != nullptr; bin_ptr = bin_ptr->next)
			if (!rhs.contains(bin_ptr->value))
				return false;

	return true;
}

template<class T>
bool HashSet<T>::operator >= (const Set<T>& rhs) const {
	return rhs <= *this;
}

template<class T>
bool HashSet<T>::operator > (const Set<T>& rhs) const {
	return rhs < *this;
}

template<class T>
std::ostream& operator << (std::ostream& outs, const HashSet<T>& s) {
	outs << "set[";

	if (!s.empty()) {
		bool first = true;
		for (int i = 0; i < s.bins; ++i) {
			for (auto bin_ptr = s.set[i]; bin_ptr->next != nullptr; bin_ptr = bin_ptr->next){
				if (first){
					outs << bin_ptr->value;
					first = false;
				}
				else
					outs <<"," << bin_ptr->value;
			}
		}
	}
	outs << "]";
	return outs;
}

//End HashMap functions!

//Iterator

//Iterator helper function
template<class T>
void HashSet<T>::Iterator::advance_cursors() {
	if (current.first == -1 || current.first > ref_set->bins)
		return;

	if (current.second->next->next != nullptr) {
		current.second = current.second->next;
		return;
	}
	++current.first;
	if (current.first >= ref_set->bins) {
		current.first = -1;
		current.second = nullptr;
		return;
	}

	for (; current.first < ref_set->bins && ref_set->set[current.first]->next == nullptr; ++current.first)
		if (current.first >= ref_set->bins) {
			current.first = -1;
			current.second = nullptr;
			return;
		}

	if (current.first >= ref_set->bins) {
		current.first = -1;
		current.second = nullptr;
		return;
	}

	current.second = ref_set->set[current.first];
}
//End Iterator helper functions!

//KLUDGE: memory-leak
template<class T>
auto HashSet<T>::ibegin () const -> ics::Iterator<T>& {
  return *(new Iterator(const_cast<HashSet<T>*>(this),true));
}

//KLUDGE: memory-leak
template<class T>
auto HashSet<T>::iend () const -> ics::Iterator<T>& {
  return *(new Iterator(const_cast<HashSet<T>*>(this),false));
}

template<class T>
auto HashSet<T>::begin () const -> HashSet<T>::Iterator {
  return Iterator(const_cast<HashSet<T>*>(this),true);
}

template<class T>
auto HashSet<T>::end () const -> HashSet<T>::Iterator {
  return Iterator(const_cast<HashSet<T>*>(this),false);
}

//Iterator Constructors:

template<class T>
HashSet<T>::Iterator::Iterator(HashSet<T>* iterate_over, bool begin) : ref_set(iterate_over) {
	expected_mod_count = ref_set->mod_count;
	if (ref_set->empty()){
		current.first = -1;
		current.second = nullptr;
	}
	if (begin) {
		for (int i = 0; i < ref_set->used; ++i)
			if (ref_set->set[i]->next != nullptr) {
				current.first = i;
				current.second = ref_set->set[i];
				break;
			}
	}
	else{
		current.first = -1;
		current.second = nullptr;
	}
}


template<class T>
HashSet<T>::Iterator::Iterator(const Iterator& i) :
    current(i.current), ref_set(i.ref_set), expected_mod_count(i.expected_mod_count), can_erase(i.can_erase) {}
//End Iterator Constructors!

//Iterator Destructor
template<class T>
HashSet<T>::Iterator::~Iterator() {}
//End Iterator Destructor!

template<class T>
T HashSet<T>::Iterator::erase() {
	if (expected_mod_count != ref_set->mod_count)
		throw ConcurrentModificationError("HashSet::Iterator::erase");
	if (!can_erase)
		throw CannotEraseError(
				"HashSet::Iterator::erase Iterator cursor already erased");
	if (current.second == nullptr)
		throw CannotEraseError(
				"HashSet::Iterator::erase Iterator cursor beyond data structure");

	can_erase = false;
	T to_return = current.second->value;
	ref_set->erase(to_return);
	if (current.second->next == nullptr){
		++current.first;
		if (current.first < ref_set->bins)
			current.second = ref_set->set[current.first];
		else{
			current.first = -1;
			current.second = nullptr;
		}
	}
	expected_mod_count = ref_set->mod_count;
	return to_return;
}

template<class T>
std::string HashSet<T>::Iterator::str() const {
	std::ostringstream answer;
	answer << ref_set->str() << "(current=" << current.first << "/"
			<< current.second << ",expected_mod_count=" << expected_mod_count
			<< ",can_erase=" << can_erase << ")";
	return answer.str();
}

template<class T>
const ics::Iterator<T>& HashSet<T>::Iterator::operator ++() {
	if (expected_mod_count != ref_set->mod_count)
		throw ConcurrentModificationError("HashSet::Iterator::operator ++");

	if (current.first == -1)
		return *this;
	if (!can_erase)
		can_erase = true;
	else
		advance_cursors();
	return *this;
}

//KLUDGE: creates garbage! (can return local value!)
template<class T>
const ics::Iterator<T>& HashSet<T>::Iterator::operator ++ (int) {
  if (expected_mod_count != ref_set->mod_count)
    throw ConcurrentModificationError("HashSet::Iterator::operator ++(int)");

	if (current.first == -1)
		return *this;
	Iterator* to_return = new Iterator(ref_set, true);
	if (!can_erase)
		can_erase = true;
	else {
		to_return->current.first = current.first;
		to_return->current.second = current.second;
		advance_cursors();
	}
	return *to_return;
}

template<class T>
bool HashSet<T>::Iterator::operator == (const ics::Iterator<T>& rhs) const {
  const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
  if (rhsASI == 0)
    throw IteratorTypeError("HashSet::Iterator::operator ==");
  if (expected_mod_count != ref_set->mod_count)
    throw ConcurrentModificationError("HashSet::Iterator::operator ==");
  if (ref_set != rhsASI->ref_set)
    throw ComparingDifferentIteratorsError("HashSet::Iterator::operator ==");

  return current.first == rhsASI->current.first && current.second == rhsASI->current.second;
}


template<class T>
bool HashSet<T>::Iterator::operator != (const ics::Iterator<T>& rhs) const {
  const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
  if (rhsASI == 0)
    throw IteratorTypeError("HashSet::Iterator::operator !=");
  if (expected_mod_count != ref_set->mod_count)
    throw ConcurrentModificationError("HashSet::Iterator::operator !=");
  if (ref_set != rhsASI->ref_set)
    throw ComparingDifferentIteratorsError("HashSet::Iterator::operator !=");

  return current.first != rhsASI->current.first || current.second != rhsASI->current.second;
}

template<class T>
T& HashSet<T>::Iterator::operator *() const {
  if (expected_mod_count !=
      ref_set->mod_count)
    throw ConcurrentModificationError("HashSet::Iterator::operator *");
  if (!can_erase || current.second == nullptr)
    throw IteratorPositionIllegal("HashSet::Iterator::operator * Iterator illegal: exhausted");

  return current.second->value;
}

template<class T>
T* HashSet<T>::Iterator::operator ->() const {
  if (expected_mod_count !=
      ref_set->mod_count)
    throw ConcurrentModificationError("HashSet::Iterator::operator *");
  if (!can_erase || current.second == nullptr)
    throw IteratorPositionIllegal("HashSet::Iterator::operator * Iterator illegal: exhausted");

  return &current.second->value;
}

}

#endif /* HASH_SET_HPP_ */
