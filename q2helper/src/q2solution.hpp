#ifndef SOLUTION_HPP_
#define SOLUTION_HPP_

#include <string>
#include <iostream>
#include <fstream>


template<class T>
class LN {
  public:
    LN ()                        : next(nullptr){}
    LN (const LN<T>& ln)         : value(ln.value), next(ln.next){}
    LN (T v, LN<T>* n = nullptr) : value(v), next(n){}
    T      value;
    LN<T>* next;
};


template<class T>
std::ostream& operator << (std::ostream& outs, LN<T>* l) {
  for (LN<T>* p = l; p != nullptr; p = p->next)
    std::cout << p->value << "->";
  std::cout << "nullptr";
  return outs;
}



char relation (const std::string& s1, const std::string& s2) {
	if (s1[0] == '\0' && s2[0] == '\0')
		return '=';
	if (s1[0] != s2[0])
		return s1[0] < s2[0] ? '<' : '>';
	else
		return relation (&s1[1], &s2[1]);
}


template<class T>
void add_ordered_i(LN<T>*& l, T value) {
	if(l == nullptr || l->value > value)
		l = new LN <T> (value, l);
	else{
		auto curr = l;
		for (; curr->next != nullptr && curr->next->value < value; curr = curr->next);
		curr->next = new LN<T>(value, curr->next);
	}
}


template<class T>
void add_ordered_r (LN<T>*& l, T value) {
	if (l == nullptr || l->value > value)
		l = new LN <T> (value, l);
	else
		add_ordered_r (l->next, value);
}



#endif /* SOLUTION_HPP_ */
