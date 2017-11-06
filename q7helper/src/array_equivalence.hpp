/* Name: Aaron Lam
 * Class: ICS 46
 * UniqueID: 90777
 */

#ifndef EQUIVALENCE_HPP_
#define ARRAY_EQUIVALENCE_HPP_

#include <sstream>
#include "ics_exceptions.hpp"
#include "array_map.hpp"
#include "array_set.hpp"


namespace ics {


template<class T>
class ArrayEquivalence {
  public:
    //Fundamental constructors/methods
    ArrayEquivalence();
    void add_singleton    (const T& a);
    bool in_same_class    (const T& a, const T& b);
    void merge_classes_of (const T& a, const T& b);

    //Other methods
    int size        () const;
    int class_count () const;
    ics::ArraySet<ics::ArraySet<T>> classes ();

    //Useful for debugging (bassed on the implementation)
    int max_height  () const;
    ics::ArrayMap<T,int> heights () const;
    void show_equivalence () const;
  private:
    ics::ArrayMap<T,T>   parent;
    ics::ArrayMap<T,int> root_size;
    T compress_to_root (T a);
};



template<class T>
ArrayEquivalence<T>::ArrayEquivalence () : parent(), root_size() {
}


//Throw an EquivalenceError (with a descriptive message) if the parameter a
//  already a value in the Equivalence (was previously added as a singleton)
template<class T>
void ArrayEquivalence<T>::add_singleton(const T& a) {
	if (parent.has_key(a)) {
		std::ostringstream exc;
		exc << "ArrayEquivalence.add_singleton: a(" << a
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
T ArrayEquivalence<T>::compress_to_root(T a) {
	if (!parent.has_key(a)) {
		std::ostringstream exc;
		exc << "ArrayEquivalence.compress_to_root: a(" << a<< ") is not already a value in the Equivalence";
		throw IcsError (exc.str());; //throw EquivalenceError(exc.str());
	}
	ics::ArraySet<T> set;
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
bool ArrayEquivalence<T>::in_same_class(const T& a, const T& b) {
	if (!parent.has_key(a) || !parent.has_key(b)) {
		std::ostringstream exc;
		exc << "ArrayEquivalence.in_same_class: a(" << a
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
void ArrayEquivalence<T>::merge_classes_of (const T& a, const T& b) {
	if (!parent.has_key(a) || !parent.has_key(b)) {
		std::ostringstream exc;
		exc << "ArrayEquivalence.merge_classes_of: a(" << a
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
int ArrayEquivalence<T>::size () const{
  return parent.size();
}


template<class T>
int ArrayEquivalence<T>::class_count () const{
  return root_size.size();
}


template<class T>
int ArrayEquivalence<T>::max_height () const{
  //Compute all root heights, then locate/return the largest
  int mh = 0;
  for (auto h : heights())
    if (h.second > mh)
      mh = h.second;
  return mh;
}


template<class T>
ics::ArraySet<ics::ArraySet<T>> ArrayEquivalence<T>::classes () {
	ics::ArraySet<ics::ArraySet<T>> to_return;
	ics::ArrayMap<T, ics::ArraySet<T>> eclass;
	for (auto it : parent)
		eclass[it.second].insert(it.first);
	for (auto it : eclass)
		to_return.insert(it.second);
	return to_return;
}


template<class T>
ics::ArrayMap<T,int> ArrayEquivalence<T>::heights () const {
  //Compute the depth of every node by tracing a path to its root;
  //  update the answer/height of the root if it is larger
  ics::ArrayMap<T,int> answer;
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
void ArrayEquivalence<T>::show_equivalence () const {
  std::cout << "  parent map is: " << parent       << std::endl;
  std::cout << "  root_size map: " << root_size    << std::endl;
  std::cout << "  heights map:   " << heights()    << std::endl;
  std::cout << "  max height:    " << max_height() << std::endl;
}


}

#endif /* ARAY_EQUIVALENCE_HPP_ */
