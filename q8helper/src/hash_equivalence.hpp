#ifndef HASH_EQUIVALENCE_HPP_
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
    HashEquivalence(int (*ahash)(const T& element));
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
    int (*hash)(const T& element);
    int set_T_hash(const ics::HashSet<T>& element);
    ics::HashMap<T,T>   parent;
    ics::HashMap<T,int> root_size;

    int max_cs = 0;
    int call_ctr = 0;
    HashMap<int, int> histogram;
    T compress_to_root (T a);
};


template<class T>
int HashEquivalence<T>::set_T_hash (const ics::HashSet<T>& element) {
  int result = 1;
  for (auto x : element)
    result *= hash(x);
  std::cout << "returning " << result << std::endl;
  return result;
}

template<class T>
HashEquivalence<T>::HashEquivalence (int (*ahash)(const T& element)) : hash(ahash), parent(ahash), root_size(ahash) , histogram(ahash){
}


//Throw an EquivalenceError (with a descriptive message) if the parameter a
//  already a value in the Equivalence (was previously added as a singleton)
template<class T>
void HashEquivalence<T>::add_singleton (const T& a) {
  if (parent.has_key(a)) {
    std::ostringstream exc;
    exc << "HashEquivalence.add_singleton: a(" << a << ") already in an equivalence class";
    throw EquivalenceError(exc.str());
  }
  parent[a] = a;    //its own parent
  root_size[a] = 1; //its equivalence class has 1 value in it
}


//Use compress_to_root in in_same_class and merge_classes_of
//When finished, a and all its ancestors should refer
//  (in the parent map) directly to the root of a's equivalence class
//Throw an EquivalenceError (with a descriptive message) if the parameter a
//  is not already a value in the Equivalence (was never added as a singleton)
template<class T>
T HashEquivalence<T>::compress_to_root (T a) {
  if (!parent.has_key(a)) {
    std::ostringstream exc;
    exc << "HashEquivalence.compress_to_root: a(" << a << ") not in an equivalence class";
    throw EquivalenceError(exc.str());
  }

  ics::HashSet<T> compress_set(hash);
  T to_root;
  while ( (to_root=parent[a]) != a) {
    compress_set.insert(a);
    a = to_root;
  }

  for (auto x : compress_set)
    parent[x] = to_root;

  //add for q8
  ++call_ctr;
  int size = compress_set.size();
  size > max_cs ? max_cs = size : max_cs;
  ++histogram[size];
  //end

  return to_root;
}


//Two values are in the same class if their equivalence trees have the
//  same roots
//In the process of finding the roots, compress all the values on the
//  path to the root: make the parents of a and all its ancestors directly
//  refer to the root of a's equivalance class (same for b).
//Throw an EquivalenceError (with a descriptive message) if the parameter a or b
//  is not already a value in the Equivalence (were never added as singletons)
template<class T>
bool HashEquivalence<T>::in_same_class (const T& a, const T& b) {
  if (!parent.has_key(a)) {
    std::ostringstream exc;
    exc << "HashEquivalence.in_same_class: a(" << a << ") not in an equivalence class";
    throw EquivalenceError(exc.str());
  }
  if (!parent.has_key(b)) {
    std::ostringstream exc;
    exc << "HashEquivalence.in_same_class: b(" << b << ") not in an equivalence class";
    throw EquivalenceError(exc.str());
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
  if (!parent.has_key(a)) {
    std::ostringstream exc;
    exc << "HashEquivalence.merge_classes_of: a(" << a << ") not in an equivalence class";
    throw EquivalenceError(exc.str());
  }
  if (!parent.has_key(b)) {
    std::ostringstream exc;
    exc << "HashEquivalence.merge_classes_of: b(" << b << ") not in an equivalence class";
    throw EquivalenceError(exc.str());
  }

  T a_root = compress_to_root(a);
  T b_root = compress_to_root(b);
  if (a_root == b_root)
    return;   //Already in same equivalence class! Don't execute code below

  if (root_size[a_root] < root_size[b_root]) {
    parent[a_root] = b_root;
    root_size[b_root] = root_size[a_root]+root_size[b_root];
    root_size.erase(a_root);
  }else{
    parent[b_root] = a_root;
    root_size[a_root] = root_size[a_root]+root_size[b_root];
    root_size.erase(b_root);
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
  //For every value in the Equivalence, compress it to its root and
  //  insert it into the set associated with the root
  ics::HashMap<T,ics::HashSet<T>> answer_map(hash);
  for (auto np : parent) {
    T root = compress_to_root(np.first);
    answer_map[root].insert(np.first);
  }

  //Now, insert all the sets collected previously in the map, into a set
  ics::HashSet<ics::HashSet<T>> answer(set_T_hash);
  for (auto rs : answer_map)
    answer.insert(rs.second);

  return answer;
}


template<class T>
ics::HashMap<T,int> HashEquivalence<T>::heights () const {
  //Compute the depth of every node by tracing a path to its root;
  //  update the answer/height of the root if it is larger
  ics::HashMap<T,int> answer(hash);
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
//  std::cout << "  parent map:    " << parent       << std::endl;
//  std::cout << "  root_size map: " << root_size    << std::endl;
//  std::cout << "  heights map:   " << heights()    << std::endl;
//  std::cout << "  max height:    " << max_height() << std::endl;

	//add for q8
	double sum = 0;
	double count = 0;
	for (int i = 0; i < max_cs; ++i){
		sum += i * histogram[i];
		count += histogram[i];
	}

	std::cout << "  max size of compress_set:      " << max_cs   << std::endl;
	std::cout << "  # of call to compress_to_root: " << call_ctr << std::endl;
	std::cout << "  histogram:                     " << histogram << std::endl;
	std::cout << "  average size of compress_set:  " << sum/count << std::endl;
	//end
}


}

#endif /* HASH_EQUIVALENCE_HPP_ */
