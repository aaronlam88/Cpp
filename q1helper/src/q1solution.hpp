//#Name:  Aaron Lam
//#SID:   92329177
//#Class: ICS46

#ifndef SOLUTION_HPP_
#define SOLUTION_HPP_

#include <string>
#include <iostream>
#include <fstream>
#include "ics46goody.hpp"
#include "ics_exceptions.hpp"
#include "iterator.hpp"
#include "array_queue.hpp"
#include "array_set.hpp"
#include "array_map.hpp"



template<class KEY,class T>
void swap (ics::ArrayMap<KEY,T>& m, KEY key1, KEY key2) {
	m[key2] = m.put(key1, m[key2]);
}


template<class KEY,class T>
void values_set_to_queue (const ics::ArrayMap<KEY,ics::ArraySet<T>>& m1,
                          ics::ArrayMap<KEY,ics::ArrayQueue<T>>& m2) {
	for(auto itMap : m1)
		m2.put(itMap.first, ics::ArrayQueue<T>((itMap.second).ibegin(), (itMap.second).iend()));

}



template<class KEY>
ics::ArraySet<KEY> reachable (const ics::ArrayMap<KEY,KEY>& m, KEY k) {
	ics::ArraySet <KEY> reached;
	while (reached.insert(k)){
		if(m.has_key(k))
			k = m[k];
	}
	return reached;
}


template<class T>
ics::ArrayMap<T,ics::ArraySet<T>> follows (const ics::ArrayQueue<T>& q) {
	ics::ArrayMap<T,ics::ArraySet<T>> followMap;

//	T key;
//	for(auto &test = q.ibegin(); test != q.iend(); ++test){
//		if(test != q.ibegin()){
//			if(followMap.has_key(key))
//				followMap[key].insert(*test);
//			else{
//				followMap.put(key, ics::ArraySet<T>());
//				followMap[key].insert(*test);
//			}
//		}
//		key = *test;
//	}
//
//	return followMap;

	ics::ArrayMap<T, ics::ArraySet<T>> answer;
	ics::Iterator<T>& i = q.ibegin();
	T prev = *(i++);
	while (i != q.iend()) {
		T next = *(i++);
		answer[prev].insert(next);
		prev = next;
	}
	return answer;
}


template<class T>
ics::ArrayMap<T,ics::ArraySet<T>> reverse (const ics::ArrayMap<T,ics::ArraySet<T>>& m) {
//	ics::ArrayMap<T,ics::ArraySet<T>> reversed;
//	for(auto itMap : m){
//		for (auto itSet : itMap.second){
//			if(reversed.has_key(itSet))
//				reversed[itSet].insert(itMap.first);
//			else{
//				reversed.put(itSet, ics::ArraySet<T>());
//				reversed[itSet].insert(itMap.first);
//			}
//		}
//	}
//
//	return reversed;

	ics::ArrayMap<T,ics::ArraySet<T>> reversed;
		for(auto itMap : m)
			for (auto itSet : itMap.second)
				reversed[itSet].insert(itMap.first);

		return reversed;
}

#endif /* SOLUTION_HPP_ */


