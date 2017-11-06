/* Name: Aaron Lam
 * Class: ICS 46
 * UniqueID: 90777
 */

#ifndef EQUIVALENCE_HPP_
#define HASH_EQUIVALENCE_HPP_

#include <sstream>
#include "ics_exceptions.hpp"
#include "hash_map.hpp"
#include "hash_set.hpp"


namespace ics {


template<class T>
class HashEquivalence {
  public:
    //Fundamental constructors/methods
	HashEquivalence(int (*ahash)(const T& k));
    void add_singleton    (const T& a);
    bool in_same_class    (const T& a, const T& b);
    void merge_classes_of (const T& a, const T& b);

    //Other methods
    int size        () const;
    int class_count () const;
    ics::HashSet<ics::HashSet<T>> classes ();

    //Useful for debugging (bassed on the implementation)
    int max_height  () const;
    ics::HashMap<T,int> heights () const;
    void show_equivalence () const;
  private:
    ics::HashMap<T,T>   parent;
    ics::HashMap<T,int> root_size;
    T compress_to_root (T a);
    int (*hash)(const T& k);

};



template<class T>
HashEquivalence<T>::HashEquivalence (int (*ahash)(const T& k)) : parent(ahash), root_size(ahash) , hash(ahash){
}


//Throw an EquivalenceError (with a descriptive message) if the parameter a
//  already a value in the Equivalence (was previously added as a singleton)
template<class T>
void HashEquivalence<T>::add_singleton(const T& a) {
	if (parent.has_key(a)) {
		std::ostringstream exc;
		exc << "HashEquivalence.add_singleton: a(" << a
				<< ") already in an equivalence class";
		throw IcsError(exc.str()); //throw EquivalenceError(exc.str());
	}
	parent[a] = a;    //acts like its own parent
	root_size[a] = 1; //its equivalence class has 1 value in it
}


//Use compress_to_root in in_same_class and merge_classes_of
//When finished, a and all its ancestors should refer
//  (in the parent map) directly to the root of a's equivalence class
//Throw an EquivalenceError (with a descriptive message) if the parameter a
//  is not already a value in the Equivalence (was never added as a singleton)
template<class T>
T HashEquivalence<T>::compress_to_root(T a) {
	if (!parent.has_key(a)) {
		std::ostringstream exc;
		exc << "HashEquivalence.compress_to_root: a(" << a<< ") is not already a value in the Equivalence";
		throw IcsError (exc.str());; //throw EquivalenceError(exc.str());
	}
	ics::HashSet<T> set(hash);
	T root;
	for (root = parent[a]; root != parent[root]; root = parent[root])
		set.insert(root);
	for (auto it: set)
		parent[it] = root;
	return root;
}


//Two values are in the same class if their equivalence trees have the
//  same roots
//In the process of finding the roots, compress all the values on the
//  path to the root: make the parents of a and all its ancestors directly
//  refer to the root of a's equivalance class (same for b).
//Throw an EquivalenceError (with a descriptive message) if the parameter a or b
//  is not already a value in the Equivalence (were never added as singletons)
template<class T>
bool HashEquivalence<T>::in_same_class(const T& a, const T& b) {
	if (!parent.has_key(a) || !parent.has_key(b)) {
		std::ostringstream exc;
		exc << "HashEquivalence.in_same_class: a(" << a
				<< ") or b(" << b << ") is not already a value in the Equivalence";
		throw IcsError (exc.str());; //throw EquivalenceError(exc.str());
	}
	return compress_to_root(a) == compress_to_root(b);
}


//Compress a and b to their roots.
//If they are in different equivalence classes, make the parent of the
//  root of the smaller equivalence class refer to the root of the larger
//  equivalence class; update the size of the root of the larger equivalence
//  class and remove the root of the smaller equivalence class from the root_size
//Throw an EquivalenceError (with a descriptive message) if the parameter a or b
//  is not already a value in the Equivalence (were never added as singletons)
template<class T>
void HashEquivalence<T>::merge_classes_of (const T& a, const T& b) {
	if (!parent.has_key(a) || !parent.has_key(b)) {
		std::ostringstream exc;
		exc << "HashEquivalence.merge_classes_of: a(" << a
				<< ") or b(" << b << ") is not already a value in the Equivalence";
		throw IcsError (exc.str());; //throw EquivalenceError(exc.str());
	}
	T roota = compress_to_root(a);
	T rootb = compress_to_root(b);
	if (roota == rootb)
		return;
	if (root_size[a] > root_size[b]){
		root_size[a] += root_size[b];
		root_size.erase(b);
		parent[b] = a;
	}
	else{
		root_size[b] += root_size[a];
		root_size.erase(a);
		parent[a] = b;
	}

}


template<class T>
int HashEquivalence<T>::size () const{
  return parent.size();
}


template<class T>
int HashEquivalence<T>::class_count () const{
  return root_size.size();
}


template<class T>
int HashEquivalence<T>::max_height () const{
  //Compute all root heights, then locate/return the largest
  int mh = 0;
  for (auto h : heights())
    if (h.second > mh)
      mh = h.second;
  return mh;
}


template<class T>
ics::HashSet<ics::HashSet<T>> HashEquivalence<T>::classes () {
	ics::HashSet<ics::HashSet<T>> to_return(hash);
	ics::HashMap<T, ics::HashSet<T>> eclass(hash);
	for (auto it : parent)
		eclass[it.second].insert(it.first);
	for (auto it : eclass)
		to_return.insert(it.second);
	return to_return;
}


template<class T>
ics::HashMap<T,int> HashEquivalence<T>::heights () const {
  //Compute the depth of every node by tracing a path to its root;
  //  update the answer/height of the root if it is larger
  ics::HashMap<T,int> answer (hash);
  for (auto np : parent) {
    T e = np.first;
    int depth = 0;
    while (parent[e] != e) {
      e = parent[e];
      depth++;
    }
    if ( answer[e] < depth)
      answer[e] = depth;
  }
  return answer;
}


template<class T>
void HashEquivalence<T>::show_equivalence () const {
  std::cout << "  parent map is: " << parent       << std::endl;
  std::cout << "  root_size map: " << root_size    << std::endl;
  std::cout << "  heights map:   " << heights()    << std::endl;
  std::cout << "  max height:    " << max_height() << std::endl;
}


}

#endif /* HASH_EQUIVALENCE_HPP_ */
