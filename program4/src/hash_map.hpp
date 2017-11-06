 /* Aaron Lam (UniqueID: 90777)*/

#ifndef HASH_MAP_HPP_
#define HASH_MAP_HPP_

#include <string>
#include <iostream>
#include <sstream>
#include <initializer_list>
#include "ics_exceptions.hpp"
#include "iterator.hpp"
#include "pair.hpp"
#include "map.hpp"
#include "array_queue.hpp"   //For traversal

namespace ics {

template<class KEY, class T> class HashMap: public Map<KEY, T> {
public:
	typedef ics::pair<KEY, T> Entry;
	HashMap() = delete;
	HashMap(int (*ahash)(const KEY& k), double the_load_factor = 1.0);
	HashMap(int initial_bins, int (*ahash)(const KEY& k), double the_load_factor = 1.0);
	HashMap(const HashMap<KEY, T>& to_copy);
	HashMap(std::initializer_list<Entry> il, int (*ahash)(const KEY& k), double the_load_factor = 1.0);
	HashMap(ics::Iterator<Entry>& start, const ics::Iterator<Entry>& stop, int (*ahash)(const KEY& k), double the_load_factor = 1.0);
	virtual ~HashMap();

	virtual bool empty() const;
	virtual int size() const;
	virtual bool has_key(const KEY& key) const;
	virtual bool has_value(const T& value) const;
	virtual std::string str() const;

	virtual T put(const KEY& key, const T& value);
	virtual T erase(const KEY& key);
	virtual void clear();

	virtual int put(ics::Iterator<Entry>& start,const ics::Iterator<Entry>& stop);

	virtual T& operator [](const KEY&);
	virtual const T& operator [](const KEY&) const;
	virtual HashMap<KEY, T>& operator =(const HashMap<KEY, T>& rhs);
	virtual bool operator ==(const Map<KEY, T>& rhs) const;
	virtual bool operator !=(const Map<KEY, T>& rhs) const;

	template<class KEY2, class T2>
	friend std::ostream& operator <<(std::ostream& outs,
			const HashMap<KEY2, T2>& m);

	virtual ics::Iterator<Entry>& ibegin() const;
	virtual ics::Iterator<Entry>& iend() const;

private:
	class LN;

public:
	class Iterator: public ics::Iterator<Entry> {
	public:
		//KLUDGE should be callable only in begin/end
		Iterator(HashMap<KEY, T>* iterate_over, bool begin);
		Iterator(const Iterator& i);
		virtual ~Iterator();
		virtual Entry erase();
		virtual std::string str() const;
		virtual const ics::Iterator<Entry>& operator ++();
		virtual const ics::Iterator<Entry>& operator ++(int);
		virtual bool operator ==(const ics::Iterator<Entry>& rhs) const;
		virtual bool operator !=(const ics::Iterator<Entry>& rhs) const;
		virtual Entry& operator *() const;
		virtual Entry* operator ->() const;
	private:
		ics::pair<int, LN*> current; //Bin Index/Cursor; stop: LN* == nullptr
		HashMap<KEY, T>* ref_map;
		int expected_mod_count;
		bool can_erase = true;
		void advance_cursors();
	};

	virtual Iterator begin() const;
	virtual Iterator end() const;
	//KLUDGE: define
	//virtual ics::Iterator<KEY>&  begin_key   () const;
	//virtual ics::Iterator<KEY>&  end_key     () const;
	//virtual ics::Iterator<T>&    begin_value () const;
	//virtual ics::Iterator<T>&    end_value   () const;

private:
	class LN {
	public:
		LN() : next(nullptr) {}
		LN(const LN& ln) : value(ln.value), next(ln.next) {}
		LN(Entry v, LN* n = nullptr) : value(v), next(n) {}

		Entry value;
		LN* next;
	};

	LN** map = nullptr;
	int (*hash)(const KEY& k);
	double load_factor;    //used/bins <= load_factor
	int bins = 1; //# bins available in the array
	int used = 0; //# of key->value pairs in the hash table
	int mod_count = 0; //For sensing concurrent modification
	int hash_compress(const KEY& key) const;
	void ensure_load_factor(int new_used);
	LN* find_key(int bin, const KEY& key) const;
	bool find_value(const T& value) const;
	LN* copy_list(LN* l) const;
	LN** copy_hash_table(LN** ht, int bins) const;
	void delete_hash_table(LN**& ht, int bins);
};

//Hash table helper functions

template<class KEY, class T>
int HashMap<KEY, T>::hash_compress(const KEY& key) const {
	unsigned int index = hash(key); //get has value
	return index % bins; // return index
}

template<class KEY, class T>
void HashMap<KEY, T>::ensure_load_factor(int new_used) {
	if (new_used / bins <= load_factor)
		return;
	bins *= 2; //update #of bins
	LN** new_map = new LN*[bins]; //create new_map with 2x the #of bins
	for (int i = 0; i < bins; ++i)
		new_map[i] = new LN(); //add trailer for bins
	for (int i = 0; i < bins/2; ++i)
		for (LN* bin_ptr = map[i]; bin_ptr->next != nullptr; ) {
			LN* to_copy = bin_ptr;
			LN* next = bin_ptr->next;
			int index = hash_compress(to_copy->value.first);
			to_copy->next = new_map[index];
			new_map[index] = to_copy;
			bin_ptr = next;
		}
	delete[] map;
	map = new_map;
}

template<class KEY, class T>
typename HashMap<KEY, T>::LN* HashMap<KEY, T>::find_key(int bin,
		const KEY& key) const {
	for (LN* c = map[bin]; c->next != nullptr; c = c->next)
		if (key == c->value.first)
			return c;

	return nullptr;
}

template<class KEY, class T>
bool HashMap<KEY, T>::find_value(const T& value) const {
	if (used == 0) //if map empty, return false
		return false;
	for (int i = 0; i < bins; ++i) //loop the map
		for (LN* bin = map[i]; bin->next != nullptr; bin = bin->next) //loop the table
			if (bin->value.second == value) //look of value in all the values in the linked list
				return true; //found, return true
	return false; //default return false | not found
}

template<class KEY, class T>
typename HashMap<KEY, T>::LN* HashMap<KEY, T>::copy_list(LN* l) const {
	return l == nullptr ? nullptr : new LN(l->value, copy_list(l->next));
}

template<class KEY, class T>
typename HashMap<KEY, T>::LN** HashMap<KEY, T>::copy_hash_table(LN** ht,
		int bins) const {
	LN** new_ht = new LN*[bins];
	for (int i = 0; i < bins; ++i)
		new_ht[i] = copy_list(ht[i]);
	return new_ht;
}

template<class KEY, class T>
void HashMap<KEY, T>::delete_hash_table(LN**& ht, int bins) {
	for (int i = 0; i < bins; ++i)
		for (LN* bin = ht[i]; bin != nullptr;) {
			LN* temp = bin->next;
			delete bin;
			bin = temp;
		}
	delete [] ht;
	ht = nullptr;
}

//End Hash table helper functions!

//HashMap Constructors

template<class KEY, class T>
HashMap<KEY, T>::HashMap(int (*ahash)(const KEY& k), double the_load_factor) :
		hash(ahash), load_factor(the_load_factor) {
	map = new LN*[bins];
	for (int i = 0; i < bins; ++i)
		map[i] = new LN();
}

template<class KEY, class T>
HashMap<KEY, T>::HashMap(int initial_bins, int (*ahash)(const KEY& k),
		double the_load_factor) :
		bins(initial_bins), hash(ahash), load_factor(the_load_factor) {
	map = new LN*[bins];
	for (int i = 0; i < bins; ++i)
		map[i] = new LN();
}

template<class KEY, class T>
HashMap<KEY, T>::HashMap(const HashMap<KEY, T>& to_copy) :
		hash(to_copy.hash), load_factor(to_copy.load_factor), bins(
				to_copy.bins), used(to_copy.used) {
	map = copy_hash_table(to_copy.map, bins);
}

template<class KEY, class T>
HashMap<KEY, T>::HashMap(ics::Iterator<Entry>& start,
		const ics::Iterator<Entry>& stop, int (*ahash)(const KEY& k),
		double the_load_factor) :
		hash(ahash), load_factor(the_load_factor) {
	map = new LN*[bins];
	for (int i = 0; i < bins; ++i)
		map[i] = new LN();
	put(start, stop);
}

template<class KEY, class T>
HashMap<KEY, T>::HashMap(std::initializer_list<Entry> il,
		int (*ahash)(const KEY& k), double the_load_factor) :
		hash(ahash), load_factor(the_load_factor) {
	map = new LN*[bins];
	for (int i = 0; i < bins; ++i)
		map[i] = new LN();
	for (Entry m_entry : il)
		put(m_entry.first, m_entry.second);
}
//End HashMap Constructors!

//HashMap Destructor
template<class KEY, class T>
HashMap<KEY, T>::~HashMap() {
	delete_hash_table(map, bins);
}
//End HashMap Destructor!

//HashMap functions:

template<class KEY, class T>
inline bool HashMap<KEY, T>::empty() const {
	return used == 0;
}

template<class KEY, class T>
int HashMap<KEY, T>::size() const {
	return used;
}

template<class KEY, class T>
bool HashMap<KEY, T>::has_key(const KEY& key) const {
	int bin = hash_compress(key);
	return find_key(bin, key) != nullptr;
}

template<class KEY, class T>
bool HashMap<KEY, T>::has_value(const T& value) const {
	return find_value(value);
}

template<class KEY, class T>
std::string HashMap<KEY, T>::str() const {
	std::ostringstream answer;
	answer << std::endl;
	for (int i = 0; i < bins; ++i){
		answer << "bin[" << i << "]: ";
		for (LN* bin_ptr = map[i]; bin_ptr != nullptr; bin_ptr = bin_ptr->next){
			if (bin_ptr->next == nullptr)
				answer << "#";
			else
				answer << "pair[" << bin_ptr->value.first << "," << bin_ptr->value.second << "] -> ";
		}
		answer << std::endl;
	}
	return answer.str();
}

template<class KEY, class T>
T HashMap<KEY, T>::put(const KEY& key, const T& value) {
	int bin = hash_compress(key);
	LN* found = find_key(bin, key);
	if (found != nullptr) {
		T to_return = found->value.second;
		found->value.second = value;
		return to_return;
	}
	++used;
	++mod_count;
	ensure_load_factor(used);
	int index = hash_compress(key);
	Entry entry (key, value);
	map[index] = new LN(entry, map[index]);

	return map[index]->value.second;
}

template<class KEY, class T>
T HashMap<KEY, T>::erase(const KEY& key) {
	int bin = hash_compress(key);
	LN* found = find_key(bin, key);
	if (found == nullptr) {
		std::ostringstream answer;
		answer << "HashMap::erase: key(" << key << ") not in Map";
		throw KeyError(answer.str());
	} else {
		--used;
		++mod_count;
		LN* to_delete = found->next;
		T to_return = found->value.second;
		found->value = found->next->value;
		found->next = found->next->next;
		delete to_delete;
		return to_return;
	}
}

template<class KEY, class T>
void HashMap<KEY, T>::clear() {
	delete_hash_table(map, bins);
	used = 0;
	bins = 1;
	map = new LN* [bins];
	map[0] = new LN ();
	++mod_count;
}

template<class KEY, class T>
int HashMap<KEY, T>::put(ics::Iterator<Entry>& start,
		const ics::Iterator<Entry>& stop) {
	int count = 0;
	for (; start != stop; ++start) {
		++count;
		put(start->first, start->second);
	}

	return count;
}

template<class KEY, class T>
T& HashMap<KEY, T>::operator [](const KEY& key) {
	int bin = hash_compress(key);
	LN* found = find_key(bin, key);
	if (found != nullptr)
		return found->value.second;

	++used;
	++mod_count;
	ensure_load_factor(used);
	int index = hash_compress(key);
	map[index] = new LN(Entry (key, T()), map[index]);
	return map[index]->value.second;
}

template<class KEY, class T>
const T& HashMap<KEY, T>::operator [](const KEY& key) const {
	int bin = hash_compress(key);
	LN* found = find_key(bin, key);
	if (found != nullptr)
		return found->value.second;

	std::ostringstream answer;
	answer << "HashMap::operator []: key(" << key << ") not in Map";
	throw KeyError(answer.str());
}

template<class KEY, class T>
bool HashMap<KEY, T>::operator ==(const Map<KEY, T>& rhs) const {
	if (this == &rhs)
		return true;
	if (used != rhs.size())
		return false;
	for (int i = 0; i < bins; ++i)
		for (LN* bin_ptr = map[i]; bin_ptr->next != nullptr; bin_ptr = bin_ptr->next)
			if (!rhs.has_key(bin_ptr->value.first) || !(bin_ptr->value.second == rhs[bin_ptr->value.first]))
				return false;
	return true;
}

template<class KEY, class T>
HashMap<KEY, T>& HashMap<KEY, T>::operator =(const HashMap<KEY, T>& rhs) {
	if (this == &rhs)
		return *this;
	ensure_load_factor(rhs.used);
	used = rhs.used;
	map = copy_hash_table(rhs.map, bins);
	++mod_count;
	return *this;
}

template<class KEY, class T>
bool HashMap<KEY, T>::operator !=(const Map<KEY, T>& rhs) const {
	return !(*this == rhs);
}

template<class KEY, class T>
std::ostream& operator <<(std::ostream& outs, const HashMap<KEY, T>& m) {
	outs << "map[";

	if (!m.empty()) {
		bool first = true;
		for (int i = 0; i < m.bins; ++i) {
			for (auto bin_ptr = m.map[i]; bin_ptr->next != nullptr;
					bin_ptr = bin_ptr->next){
				if (first){
					outs << bin_ptr->value.first << "->" << bin_ptr->value.second;
					first = false;
				}
				else
					outs <<"," << bin_ptr->value.first << "->" << bin_ptr->value.second;
			}
		}
	}
	outs << "]";
	return outs;
}

//End HashMap functions!

//Iterator

//Iterator helper function
template<class KEY, class T>
void HashMap<KEY, T>::Iterator::advance_cursors() {
	if (current.first == -1)
		return;

	if (current.second->next->next != nullptr) {
		current.second = current.second->next;
		return;
	}
	++current.first;
	if (current.first >= ref_map->bins) {
		current.first = -1;
		current.second = nullptr;
		return;
	}
	for (;current.first < ref_map->bins &&ref_map->map[current.first]->next == nullptr; ++current.first)
		if (current.first >= ref_map->bins) {
			current.first = -1;
			current.second = nullptr;
			return;
		}
	if (current.first >= ref_map->bins) {
		current.first = -1;
		current.second = nullptr;
		return;
	}
	current.second = ref_map->map[current.first];

}
//End Iterator helper functions!

//KLUDGE: memory-leak
template<class KEY, class T>
auto HashMap<KEY, T>::ibegin() const -> ics::Iterator<Entry>& {
	return *(new Iterator(const_cast<HashMap<KEY, T>*>(this), true));
}

//KLUDGE: memory-leak
template<class KEY, class T>
auto HashMap<KEY, T>::iend() const -> ics::Iterator<Entry>& {
	return *(new Iterator(const_cast<HashMap<KEY, T>*>(this), false));
}

template<class KEY, class T>
auto HashMap<KEY, T>::begin() const -> HashMap<KEY,T>::Iterator {
	return Iterator(const_cast<HashMap<KEY, T>*>(this), true);
}

template<class KEY, class T>
auto HashMap<KEY, T>::end() const -> HashMap<KEY,T>::Iterator {
	return Iterator(const_cast<HashMap<KEY, T>*>(this), false);
}

//Iterator Constructors:

template<class KEY, class T>
HashMap<KEY, T>::Iterator::Iterator(HashMap<KEY, T>* iterate_over, bool begin) :
		ref_map(iterate_over) {
	expected_mod_count = ref_map->mod_count;
	if (begin) {
		for (int i = 0; i < ref_map->used; ++i)
			if (ref_map->map[i]->next != nullptr) {
				current.first = i;
				current.second = ref_map->map[i];
				break;
			}
	}
	else{
		current.first = -1;
		current.second = nullptr;
	}
}

template<class KEY, class T>
HashMap<KEY, T>::Iterator::Iterator(const Iterator& i) :
		current(i.current), ref_map(i.ref_map), expected_mod_count(
				i.expected_mod_count), can_erase(i.can_erase) {
}
//End Iterator Constructors!

//Iterator Destructor
template<class KEY, class T>
HashMap<KEY, T>::Iterator::~Iterator() {
}
//End Iterator Destructor!

//Iterator functions:

template<class KEY, class T>
auto HashMap<KEY, T>::Iterator::erase() -> Entry {
	if (expected_mod_count != ref_map->mod_count)
		throw ConcurrentModificationError("HashMap::Iterator::erase");
	if (!can_erase)
		throw CannotEraseError(
				"HashMap::Iterator::erase Iterator cursor already erased");
	if (current.second == nullptr)
		throw CannotEraseError(
				"HashMap::Iterator::erase Iterator cursor beyond data structure");

	can_erase = false;
	Entry to_return = current.second->value;
	advance_cursors();
	ref_map->erase(to_return.first);
	expected_mod_count = ref_map->mod_count;
	return to_return;
}

template<class KEY, class T>
std::string HashMap<KEY, T>::Iterator::str() const {
	std::ostringstream answer;
	answer << ref_map->str() << "(current=" << current.first << "/"
			<< current.second << ",expected_mod_count=" << expected_mod_count
			<< ",can_erase=" << can_erase << ")";
	return answer.str();
}

//KLUDGE: cannot use Entry
template<class KEY, class T>
auto HashMap<KEY, T>::Iterator::operator ++() -> const ics::Iterator<ics::pair<KEY,T>>& {
	if (expected_mod_count != ref_map->mod_count)
		throw ConcurrentModificationError("HashMap::Iterator::operator ++");

	if (current.first >= ref_map->bins)
		return *this;
	if (!can_erase)
		can_erase = true;
	else
		advance_cursors();
	return *this;
}

//KLUDGE: creates garbage! (can return local value!)
template<class KEY, class T>
auto HashMap<KEY, T>::Iterator::operator ++(int) -> const ics::Iterator<ics::pair<KEY,T>>& {
	if (expected_mod_count != ref_map->mod_count)
		throw ConcurrentModificationError(
				"HashMap::Iterator::operator ++(int)");

	if (current.first == -1)
		return *this;
	Iterator* to_return = new Iterator(this->ref_map, true);
	if (!can_erase)
		can_erase = true;
	else {
		to_return->current.first = current.first;
		to_return->current.second = current.second;
		advance_cursors();
	}
	return *to_return;
}

template<class KEY, class T>
bool HashMap<KEY, T>::Iterator::operator == (
		const ics::Iterator<Entry>& rhs) const {
	const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
	if (rhsASI == 0)
		throw IteratorTypeError("HashMap::Iterator::operator ==");
	if (expected_mod_count != ref_map->mod_count)
		throw ConcurrentModificationError("HashMap::Iterator::operator ==");
	if (ref_map != rhsASI->ref_map)
		throw ComparingDifferentIteratorsError(
				"HashMap::Iterator::operator ==");

	return current.first == rhsASI->current.first && current.second == rhsASI->current.second;
}

template<class KEY, class T>
bool HashMap<KEY, T>::Iterator::operator !=(
		const ics::Iterator<Entry>& rhs) const {
	const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
	if (rhsASI == 0)
		throw IteratorTypeError("HashMap::Iterator::operator !=");
	if (expected_mod_count != ref_map->mod_count)
		throw ConcurrentModificationError("HashMap::Iterator::operator !=");
	if (ref_map != rhsASI->ref_map)
		throw ComparingDifferentIteratorsError(
				"HashMap::Iterator::operator !=");

	return current.first != rhsASI->current.first || current.second != rhsASI->current.second;
}

template<class KEY, class T>
ics::pair<KEY, T>& HashMap<KEY, T>::Iterator::operator *() const {
	if (expected_mod_count != ref_map->mod_count)
		throw ConcurrentModificationError("HashMap::Iterator::operator *");
	if (!can_erase || current.second == nullptr)
		throw IteratorPositionIllegal(
				"HashMap::Iterator::operator * Iterator illegal: exhausted");

	return current.second->value;
}

template<class KEY, class T>
ics::pair<KEY, T>* HashMap<KEY, T>::Iterator::operator ->() const {
	if (expected_mod_count != ref_map->mod_count)
		throw ConcurrentModificationError("HashMap::Iterator::operator *");
	if (!can_erase || current.second == nullptr)
		throw IteratorPositionIllegal(
				"HashMap::Iterator::operator -> Iterator illegal: exhausted");

	return &current.second->value;
}

}

#endif /* HASH_MAP_HPP_ */
