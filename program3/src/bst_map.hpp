/*
 * Aaron Lam (UniqueID: 90777)
 * Fei Fei Zhang (UniqueID: 126860)
 *
 * We certify that we worked cooperatively on this programming
 * assignment, according to the rules for pair programming
 */

#ifndef BST_MAP_HPP_
#define BST_MAP_HPP_

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

template<class KEY, class T> class BSTMap: public Map<KEY, T> {
public:
	typedef ics::pair<KEY, T> Entry;
	BSTMap();
	BSTMap(const BSTMap<KEY, T>& to_copy);
	BSTMap(std::initializer_list<Entry> il);
	BSTMap(ics::Iterator<Entry>& start, const ics::Iterator<Entry>& stop);
	virtual ~BSTMap();

	virtual bool empty() const;
	virtual int size() const;
	virtual bool has_key(const KEY& key) const;
	virtual bool has_value(const T& value) const;
	virtual std::string str() const;

	virtual T put(const KEY& key, const T& value);
	virtual T erase(const KEY& key);
	virtual void clear();

	virtual int put(ics::Iterator<Entry>& start,
			const ics::Iterator<Entry>& stop);

	virtual T& operator [](const KEY&);
	virtual const T& operator [](const KEY&) const;
	virtual BSTMap<KEY, T>& operator = (const BSTMap<KEY, T>& rhs);
	virtual bool operator ==(const Map<KEY, T>& rhs) const;
	virtual bool operator !=(const Map<KEY, T>& rhs) const;

	template<class KEY2, class T2>
	friend std::ostream& operator <<(std::ostream& outs,
			const BSTMap<KEY2, T2>& m);

	virtual ics::Iterator<Entry>& ibegin() const;
	virtual ics::Iterator<Entry>& iend() const;

private:
	class TN;

public:
	class Iterator: public ics::Iterator<Entry> {
	public:
		//KLUDGE should be callable only in begin/end
		Iterator(BSTMap<KEY, T>* iterate_over, bool begin);
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
		ics::ArrayQueue<Entry> it;          //Iterator (as a Queue) for Map
		BSTMap<KEY, T>* ref_map;
		int expected_mod_count;
		bool can_erase = true;
	};

	virtual Iterator begin() const;
	virtual Iterator end() const;
	//KLUDGE: define
	//virtual ics::Iterator<KEY>&  begin_key   () const;
	//virtual ics::Iterator<KEY>&  end_key     () const;
	//virtual ics::Iterator<T>&    begin_value () const;
	//virtual ics::Iterator<T>&    end_value   () const;

private:
	class TN {
	public:
		TN() : left(nullptr), right(nullptr) {}
		TN(const TN& tn) : value(tn.value), left(tn.left), right(tn.right) {}
		TN(Entry v, TN* l = nullptr, TN* r = nullptr) : value(v), left(l), right(r) {}
		Entry value;
		TN* left;
		TN* right;
	};

	TN* map = nullptr;
	int used = 0; //Number of nodes in the tree
	int mod_count = 0; //For sensing concurrent modification
	TN* find_key(TN* root, const KEY& key) const;
	bool find_value(TN* root, const T& value) const;
	T& insert(TN*& root, const KEY& key, const T& value);
	ics::pair<KEY, T> remove_closest(TN*& root);
	T remove(TN*& root, const KEY& key);
	TN* copy(TN* root) const;
	void copy_to_queue(TN* root, ArrayQueue<Entry>& q) const;
	void delete_BST(TN*& root);
	bool equals(TN* root, const Map<KEY, T>& other) const;
	std::string string_rotated(TN* root, std::string indent) const;
};

//BST helper functions:

//Find key in BST root
//If key found, return the pointer to the node contain the key
//Else return nullptr
template<class KEY, class T>
typename BSTMap<KEY, T>::TN* BSTMap<KEY, T>::find_key(TN* root, const KEY& key) const {
	if (root == nullptr)
		return nullptr;
	if (key == root->value.first)
		return root;
	else if (key < root->value.first)
		return find_key(root->left, key);
	else
		return find_key(root->right, key);
}

//Find value in BST root
//Traverse the entire BST root to find value
//If value found, return true, else return false
template<class KEY, class T>
bool BSTMap<KEY, T>::find_value(TN* root, const T& value) const {
	if (root == nullptr)
		return false;
	if (root->value.second == value)
		return true;
	return find_value(root->left, value) && find_value(root->right, value);
}

//Insert Entry (pair) key->value into BST root
//Find the right location for key and insert Entry (similar to find_key(key))
//NOTE: If find_key(key) = true, overwrite OLD value with value, return NEW value
//Call find_key(key) and save the OLD value, if OLD value should be returned
template<class KEY, class T>
T& BSTMap<KEY, T>::insert(TN*& root, const KEY& key, const T& value) {
	if (root == nullptr){
		root = new TN (Entry(key, value));
		return root->value.second;
	}
	if (key == root->value.first)
		return root->value.second = value;
	else if (key < root->value.first)
		return insert(root->left, key, value);
	else
		return insert(root->right, key, value);
}

//Use in remove to keep the BST balance
//Delete the most right leaf node (max value in  BST root), return it value
template<class KEY, class T>
ics::pair<KEY, T> BSTMap<KEY, T>::remove_closest(TN*& root) {
	if (root->right == nullptr) {
		ics::pair<KEY, T> to_return = root->value;
		TN* to_delete = root;
		root = root->left;
		delete to_delete;
		return to_return;
	} else
		return remove_closest(root->right);
}

//Remove the node contain key in BST root.
//If key not find, throw KeyError.
//Else, copy the max value of the subtree of node contain key into node, delete the max node.
template<class KEY, class T>
T BSTMap<KEY, T>::remove(TN*& root, const KEY& key) {
	if (root == nullptr) {
		std::ostringstream answer;
		answer << "BSTMap::erase: key(" << key << ") not in Map";
		throw KeyError(answer.str());
	} else if (key == root->value.first) {
		T to_return = root->value.second;
		if (root->left == nullptr) {
			TN* to_delete = root;
			root = root->right;
			delete to_delete;
		} else if (root->right == nullptr) {
			TN* to_delete = root;
			root = root->left;
			delete to_delete;
		} else
			root->value = remove_closest(root->left);
		return to_return;
	} else
		return remove((key < root->value.first ? root->left : root->right), key);
}

//Make a copy of BST root and then return the copied BST
//Assume root is a BST, !no BST check!, so this will be O(N), and not O(N Log N)
template<class KEY, class T>
typename BSTMap<KEY, T>::TN* BSTMap<KEY, T>::copy(TN* root) const {
	if (root == nullptr)
		return nullptr;

	TN* to_return = new TN (root->value, copy(root->left), copy(root->right));

	return to_return;
}

//copy BST root to ArrayQueue q, using inorder traversal
//Use in Iterator Constructor, q will have order from low to high
template<class KEY, class T>
void BSTMap<KEY, T>::copy_to_queue(TN* root, ArrayQueue<Entry>& q) const {
	if(root){
		copy_to_queue(root->left, q);
		q.enqueue(root->value);
		copy_to_queue(root->right, q);
	}
}

//Delete the entire BST root
//Using inorder traversal to go through all the node in root and delete it
template<class KEY, class T>
void BSTMap<KEY, T>::delete_BST(TN*& root) {
	if (root){
		delete_BST(root->left);
		delete root;
		delete_BST(root->right);
	}
	root = nullptr;
}

//Check if 2 Maps (this vs other) are equal
//Check if 2 Maps have the same size, same keys, and that keys are pointed to same values
template<class KEY, class T>
bool BSTMap<KEY, T>::equals(TN* root, const Map<KEY, T>& other) const {
	if (other.size() != used)
		return false;
	for (auto &it = other.ibegin(); it != other.iend(); ++it){
		TN* entry = find_key(root, it->first);
		if (entry == nullptr || entry->value.second != it->second)
			return false;
	}
	return true;
}

//print out the BST, and show the structure of the BST
//No test case for this function!?
//NOTE: Added function call in clear, print out the BST before delete, PASS!
template<class KEY, class T>
std::string BSTMap<KEY, T>::string_rotated(TN* root, std::string indent) const {
	std::string to_return = "";
	if (root == nullptr)
		return to_return;
	else {
		to_return += indent;
		string_rotated(root->right, indent + "..");
		std::cout << to_return << root->value.first << "->" << root->value.second << std::endl;
		string_rotated(root->left, indent + "..");
	}
	return to_return;
}

//End BST helper functions!

//BSTMap Constructor:

template<class KEY, class T>
BSTMap<KEY, T>::BSTMap() {
}

template<class KEY, class T>
BSTMap<KEY, T>::BSTMap(const BSTMap<KEY, T>& to_copy)
	:used(to_copy.used) {
	delete_BST(map);
	map = copy (to_copy.map);
}

template<class KEY, class T>
BSTMap<KEY, T>::BSTMap(ics::Iterator<Entry>& start, const ics::Iterator<Entry>& stop) {
	put (start, stop);
}

template<class KEY, class T>
BSTMap<KEY, T>::BSTMap(std::initializer_list<Entry> il) {
	for (Entry it : il)
		put (it.first, it.second);
}

//End BSTMap Constructor!

//BSTMap Destructor:
template<class KEY, class T>
BSTMap<KEY, T>::~BSTMap() {
	delete_BST(map);
}
//End BSTMap Destructor!

//BSTMap functions:

template<class KEY, class T>
inline bool BSTMap<KEY, T>::empty() const {
	return used == 0;
}

template<class KEY, class T>
int BSTMap<KEY, T>::size() const {
	return used;
}

template<class KEY, class T>
bool BSTMap<KEY, T>::has_key(const KEY& element) const {
	return find_key(map, element) != nullptr;
}

template<class KEY, class T>
bool BSTMap<KEY, T>::has_value(const T& element) const {
	return find_value (map, element);
}

template<class KEY, class T>
std::string BSTMap<KEY, T>::str() const {
	std::ostringstream answer;
	answer << *this << "(used=" << used << ",mod_count=" << mod_count << ")";
	return answer.str();
}

//Put Entry (key, value) into this map
//Check if this map contain key, if so, overwrite the value, do not increment used or mod_cout
//If key not fount, used insert to insert key and value to map
template<class KEY, class T>
T BSTMap<KEY, T>::put(const KEY& key, const T& value) {
	TN* found = find_key (map, key);
	if (found != nullptr){
		T to_return = found->value.second;
		found->value.second = value;
		return to_return;
	}
	++used;
	++mod_count;
	return insert(map, key, value);
}

//Call put to put all the Entry value from start to stop into map
template<class KEY, class T>
int BSTMap<KEY, T>::put(ics::Iterator<Entry>& start, const ics::Iterator<Entry>& stop) {
	for (; start != stop; ++start)
		put (start->first, start->second);
	return used;
}

//Erase the node contain key
//If key is not in BST, throw KeyError
//Use remove the remove key node from map
template<class KEY, class T>
T BSTMap<KEY, T>::erase(const KEY& key) {
	if (find_key(map, key)) {
		--used;
		++mod_count;
		return remove(map, key);
	}

	std::ostringstream answer;
	answer << "BSTMap::erase: key(" << key << ") not in Map";
	throw KeyError(answer.str());
}

//Delete BSTMap => delete map, set used = 0
//Used delete_BST to delete map, then set used = 0
template<class KEY, class T>
void BSTMap<KEY, T>::clear() {
	//string_rotated(map, " ");//Test string_rotated
	delete_BST(map);
	used = 0;
	++mod_count;
}

//Look for key in map (using find_key)
//If found, return address of the value at node key, and allow overwrite
//If not found, insert key with empty value into map, return address of the value at node key for overwrite
template<class KEY, class T>
T& BSTMap<KEY, T>::operator [](const KEY& key) {
	TN* node = find_key(map, key);
	if(node != nullptr)
		return node->value.second;

	++used;
	++mod_count;
	return insert (map, key, T());
}

//Return the address of node that contain key, no modification allow!
//If key is not in map, throw KeyError
template<class KEY, class T>
const T& BSTMap<KEY, T>::operator [](const KEY& key) const {
	TN* node = find_key(map, key);
	if(node != nullptr)
		return node->value.second;

	std::ostringstream answer;
	answer << "ArrayMap::operator []: key(" << key << ") not in Map";
	throw KeyError(answer.str());
}

//Check if Map rhs == this, use equals to check
template<class KEY, class T>
bool BSTMap<KEY, T>::operator ==(const Map<KEY, T>& rhs) const {
	return equals(map, rhs);
}
//Check if Map rhs != this, use equals to check
template<class KEY, class T>
bool BSTMap<KEY, T>::operator !=(const Map<KEY, T>& rhs) const {
	return !equals(map, rhs);
}

//Assign the BSTMap rhs to this
//Delete this.map, copy rhs size, use copy to copy rhs.map to this.map
template<class KEY, class T>
BSTMap<KEY, T>& BSTMap<KEY, T>::operator = (const BSTMap<KEY, T>& rhs) {
	delete_BST(map);
	used = rhs.used;
	++mod_count;
	map = copy(rhs.map);
	return *this;
}

//print BSTMap
template<class KEY, class T>
std::ostream& operator <<(std::ostream& outs, const BSTMap<KEY, T>& m) {
	outs << "map[";

	if (!m.empty()) {
		for (auto &it = m.ibegin(); it != m.iend();){
			outs << it->first << "->" << it->second;
			if(++it != m.end())
				outs << ",";
		}
	}

	outs << "]";
	return outs;
}

//End BSTMap functions!

//BSTMap Iterator:

//BSTMap Iterator Constructor:

//If begin == true, copy BSTMap iterate_over to ArrayQueue it
//Use it to iterate over the BSTMap
template<class KEY, class T>
BSTMap<KEY, T>::Iterator::Iterator(BSTMap<KEY, T>* iterate_over, bool begin)
	: ref_map(iterate_over) {
	expected_mod_count = ref_map->mod_count;
	if(begin)
		ref_map->copy_to_queue(ref_map->map, it);
}

template<class KEY, class T>
BSTMap<KEY, T>::Iterator::Iterator(const Iterator& i)
	:it(i.it), ref_map(i.ref_map), expected_mod_count(i.expected_mod_count), can_erase(i.can_erase) {

}
//End BSTMap Iterator Constructor!

//BSTMap Iterator Destructor:
template<class KEY, class T>
BSTMap<KEY, T>::Iterator::~Iterator() {
	it.clear();
	ref_map = nullptr;
}
//End BSTMap Iterator Destructor!

//BSTMap Iterator begin-end:
template<class KEY, class T>
auto BSTMap<KEY, T>::ibegin() const -> ics::Iterator<Entry>& {
	return *(new Iterator(const_cast<BSTMap<KEY, T>*>(this), true));
}

template<class KEY, class T>
auto BSTMap<KEY, T>::iend() const -> ics::Iterator<Entry>& {
	return *(new Iterator(const_cast<BSTMap<KEY, T>*>(this), false));
}

template<class KEY, class T>
auto BSTMap<KEY, T>::begin() const -> BSTMap<KEY,T>::Iterator {
	return Iterator(const_cast<BSTMap<KEY, T>*>(this), true);
}

template<class KEY, class T>
auto BSTMap<KEY, T>::end() const -> BSTMap<KEY,T>::Iterator {
	return Iterator(const_cast<BSTMap<KEY, T>*>(this), false);
}
//End BSTMap Iterator begin-end!

//BSTMap Iterator functions:

//Dequeue it to get key needed to erase (also move forward)
//Call remove to remove node key form ref_map->map (iterate over object)
//Return the dequeue value
template<class KEY, class T>
auto BSTMap<KEY, T>::Iterator::erase() -> Entry {
	if (expected_mod_count != ref_map->mod_count)
		throw ConcurrentModificationError("BSTMap::Iterator::erase");
	if (!can_erase)
		throw CannotEraseError("BSTMap::Iterator::erase Iterator cursor already erased");
	if (it.empty())
		throw CannotEraseError("BSTMap::Iterator::erase Iterator cursor beyond data structure");

	can_erase = false;
	Entry to_return = it.dequeue();
	ref_map->remove(ref_map->map, to_return.first);
	--ref_map->used;
	++ref_map->mod_count;
	expected_mod_count = ref_map->mod_count;
	return to_return;
}

template<class KEY, class T>
std::string BSTMap<KEY, T>::Iterator::str() const {
	std::ostringstream answer;
	answer << ref_map->str() << "(expected_mod_count=" << expected_mod_count
			<< ",can_erase=" << can_erase << ")";
	return answer.str();
}

//Dequeue it to move forward
template<class KEY, class T>
auto BSTMap<KEY, T>::Iterator::operator ++() -> const ics::Iterator<ics::pair<KEY,T>>& {
	if (expected_mod_count != ref_map->mod_count)
		throw ConcurrentModificationError("BSTMap::Iterator::operator ++");

	if (it.empty())
		return *this;

	if (!can_erase)
		can_erase = true;
	else
		it.dequeue();

	return *this;
}

//KLUDGE: creates garbage! (can return local value!)
template<class KEY, class T>
auto BSTMap<KEY, T>::Iterator::operator ++(int) -> const ics::Iterator<ics::pair<KEY,T>>& {
	if (expected_mod_count != ref_map->mod_count)
		throw ConcurrentModificationError("BSTMap::Iterator::operator ++(int)");

	if (it.empty())
		return *this;

	Iterator* to_return = new Iterator(this->ref_map, true);
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
template<class KEY, class T>
bool BSTMap<KEY, T>::Iterator::operator == (const ics::Iterator<Entry>& rhs) const {
	const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
	if (rhsASI == 0)
		throw IteratorTypeError("BSTMap::Iterator::operator ==");
	if (expected_mod_count != ref_map->mod_count)
		throw ConcurrentModificationError("BSTMap::Iterator::operator ==");
	if (ref_map != rhsASI->ref_map)
		throw ComparingDifferentIteratorsError("BSTMap::Iterator::operator ==");

	if (it.empty())
		return rhsASI->it.empty();
	else if (rhsASI->it.empty())
		return false;
	else
		return it.peek() == rhsASI->it.peek();

}

//If it.empty, false (end)
//Else if rhsASI->it.empty, true (!empty != empty)
//Else compare it.peek vs rhsASI->it.peek (compare 2 current values)
template<class KEY, class T>
bool BSTMap<KEY, T>::Iterator::operator != (const ics::Iterator<Entry>& rhs) const {
	const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
	if (rhsASI == 0)
		throw IteratorTypeError("BSTMap::Iterator::operator !=");
	if (expected_mod_count != ref_map->mod_count)
		throw ConcurrentModificationError("BSTMap::Iterator::operator !=");
	if (ref_map != rhsASI->ref_map)
		throw ComparingDifferentIteratorsError("BSTMap::Iterator::operator !=");

	if (it.empty())
		return false;
	else if (rhsASI->it.empty())
		return true;
	else
		return it.peek() != rhsASI->it.peek();
}

//return it.peek() (current value)
template<class KEY, class T>
ics::pair<KEY, T>& BSTMap<KEY, T>::Iterator::operator *() const {
	if (expected_mod_count != ref_map->mod_count)
		throw ConcurrentModificationError("BSTMap::Iterator::operator *");
	if (!can_erase || it.empty())
		throw IteratorPositionIllegal(
				"BSTMap::Iterator::operator * Iterator illegal: exhausted");

	return it.peek();
}

//return &it.peek() (address of current value)
template<class KEY, class T>
ics::pair<KEY, T>* BSTMap<KEY, T>::Iterator::operator ->() const {
	if (expected_mod_count != ref_map->mod_count)
		throw ConcurrentModificationError("BSTMap::Iterator::operator *");
	if (!can_erase || it.empty())
		throw IteratorPositionIllegal(
				"BSTMap::Iterator::operator -> Iterator illegal: exhausted");

	return &it.peek();
}
//End BSTMap Iterator

}

#endif /* BST_MAP_HPP_ */
