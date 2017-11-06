#ifndef LINEAR_ARRAY_QUEUE_HPP_
#define LINEAR_ARRAY_QUEUE_HPP_

#include <string>
#include <iostream>
#include <initializer_list>
#include <sstream>
#include "ics_exceptions.hpp"
#include "iterator.hpp"
#include "queue.hpp"


namespace ics {

template<class T> class LinearArrayQueue : public Queue<T>  {
  public:
    LinearArrayQueue();
    explicit LinearArrayQueue(int initialLength);
    LinearArrayQueue(const LinearArrayQueue<T>& to_copy);
    LinearArrayQueue(std::initializer_list<T> il);
    LinearArrayQueue(ics::Iterator<T>& start, const ics::Iterator<T>& stop);
    virtual ~LinearArrayQueue();

    virtual bool empty      () const;
    virtual int  size       () const;
    virtual T&   peek       () const;
    virtual std::string str () const;

    virtual int  enqueue (const T& element);
    virtual T    dequeue ();
    virtual void clear   ();

    virtual int enqueue (ics::Iterator<T>& start, const ics::Iterator<T>& stop);

    virtual LinearArrayQueue<T>& operator = (const LinearArrayQueue<T>& rhs);
    virtual bool operator == (const Queue<T>& rhs) const;
    virtual bool operator != (const Queue<T>& rhs) const;

    template<class T2>
    friend std::ostream& operator << (std::ostream& outs, const LinearArrayQueue<T2>& s);

    class Iterator : public ics::Iterator<T> {
      public:
        //KLUDGE should be callable only in begin/end
        Iterator(LinearArrayQueue<T>* fof, int initial);
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
        int current;
        LinearArrayQueue<T>* ref_queue;
        int expected_mod_count;
        bool can_erase = true;
    };

    //For explicit use: Iterator<...>& it = c.ibegin(); ... or for (Iterator<...>& it = c.ibegin(); it != c.iend(); ++it)...
    virtual ics::Iterator<T>& ibegin () const;
    virtual ics::Iterator<T>& iend   () const;

    //For implicit use: for (... i : c)...
    virtual Iterator begin () const;
    virtual Iterator end   () const;


  private:
    T*  queue;
    int length    = 0; //Physical length of array (must be > .size())
    int used      = 0; //Amount of array used
    int mod_count = 0; //For sensing concurrent modification
    int  erase_at(int i);
    void ensure_length(int new_length);
  };





template<class T>
LinearArrayQueue<T>::LinearArrayQueue() {
  queue = new T[length];
}


template<class T>
LinearArrayQueue<T>::LinearArrayQueue(int initial_length) : length(initial_length) {
  if (length < 0)
    length = 0;
  queue = new T[length];
}


template<class T>
LinearArrayQueue<T>::LinearArrayQueue(const LinearArrayQueue<T>& to_copy) : length(to_copy.length)  {
  queue = new T[length];
  used = 0;
  for (int i=0; i<used; ++i)
    queue[i] = to_copy.queue[i];
}


template<class T>
LinearArrayQueue<T>::LinearArrayQueue(ics::Iterator<T>& start, const ics::Iterator<T>& stop) {
  queue = new T[length];
  enqueue(start,stop);
}


template<class T>
LinearArrayQueue<T>::LinearArrayQueue(std::initializer_list<T> il) {
  queue = new T[length];
  for (T q_elem : il)
    enqueue(q_elem);
}


template<class T>
LinearArrayQueue<T>::~LinearArrayQueue() {
  delete[] queue;
}


template<class T>
bool LinearArrayQueue<T>::empty() const {
  return used == 0;
}


template<class T>
inline int LinearArrayQueue<T>::size() const {
  return used;
}


template<class T>
T& LinearArrayQueue<T>::peek () const {
  if (this->empty())
    throw EmptyError("LinearArrayQueue::peek");

  return queue[0];
}


template<class T>
std::string LinearArrayQueue<T>::str() const {
  std::ostringstream answer;
  answer << *this << "(length=" << length << ",used=" << used << ",mod_count=" << mod_count << ")";
  return answer.str();
}


template<class T>
int LinearArrayQueue<T>::enqueue(const T& element) {
  this->ensure_length(used+1);
  queue[used++] = element;
  ++mod_count;
  return 1;
}


template<class T>
T LinearArrayQueue<T>::dequeue() {
  if (this->empty())
    throw EmptyError("LinearArrayQueue::dequeue");

  T answer = queue[0];
  for (int i=0; i<used-1; ++i)
    queue[i] = queue[i+1];
  	--used; //here is the error!
  ++mod_count;
  return answer;
}


template<class T>
void LinearArrayQueue<T>::clear() {
  used = 0;
  ++mod_count;
}


template<class T>
int LinearArrayQueue<T>::enqueue(ics::Iterator<T>& start, const ics::Iterator<T>& stop) {
  int count = 0;
  for (; start != stop; start++)
    count += enqueue(*start);
  return count;
}


template<class T>
LinearArrayQueue<T>& LinearArrayQueue<T>::operator = (const LinearArrayQueue<T>& rhs) {
  if (this == &rhs)
    return *this;
  this->ensure_length(rhs.used);
  used = rhs.used;
  for (int i=0; i<used; ++i)
    queue[i] = rhs.queue[i];
  mod_count = 0;  //nothing iterating on it
  return *this;
}


template<class T>
bool LinearArrayQueue<T>::operator == (const Queue<T>& rhs) const {
  if (this == &rhs)
    return true;
  if (used != rhs.size())
    return false;
  ics::Iterator<T>& rhs_i = rhs.ibegin();
  for (int i=0; i<used; ++i,++rhs_i)
    if (queue[i] != *rhs_i)
      return false;

  return true;
}


template<class T>
bool LinearArrayQueue<T>::operator != (const Queue<T>& rhs) const {
  return !(*this == rhs);
}

template<class T>
std::ostream& operator << (std::ostream& outs, const LinearArrayQueue<T>& q) {
  outs << "queue[";

  if (!q.empty()) {
    outs << q.queue[0];
    for (int i = 1; i < q.used; ++i)
      outs << ","<< q.queue[i];
  }

  outs << "]:rear";
  return outs;
}


//KLUDGE: memory-leak
template<class T>
auto LinearArrayQueue<T>::ibegin () const -> ics::Iterator<T>& {
  return *(new Iterator(const_cast<LinearArrayQueue<T>*>(this),0));
}


//KLUDGE: memory-leak
template<class T>
auto LinearArrayQueue<T>::iend () const -> ics::Iterator<T>& {
  return *(new Iterator(const_cast<LinearArrayQueue<T>*>(this),used));
}


template<class T>
auto LinearArrayQueue<T>::begin () const -> LinearArrayQueue<T>::Iterator {
  return Iterator(const_cast<LinearArrayQueue<T>*>(this),0);
}


template<class T>
auto LinearArrayQueue<T>::end () const -> LinearArrayQueue<T>::Iterator {
  return Iterator(const_cast<LinearArrayQueue<T>*>(this),used);
}


template<class T>
int LinearArrayQueue<T>::erase_at(int i) {
  for (int j=i; j<used-1; ++j)
    queue[j] = queue[j+1];
  --used;
  ++mod_count;
  return 1;
}


template<class T>
void LinearArrayQueue<T>::ensure_length(int new_length) {
  if (length >= new_length)
    return;
  T*  old_queue  = queue;
  length = std::max(new_length,2*length);
  queue = new T[length];
  for (int i=0; i<used; ++i)
    queue[i] = old_queue[i];

  delete [] old_queue;
}





template<class T>
LinearArrayQueue<T>::Iterator::Iterator(LinearArrayQueue<T>* fof, int initial) : current(initial), ref_queue(fof) {
  expected_mod_count = ref_queue->mod_count;
}


template<class T>
LinearArrayQueue<T>::Iterator::~Iterator() {}

template<class T>
T LinearArrayQueue<T>::Iterator::erase() {
  if (expected_mod_count != ref_queue->mod_count)
    throw ConcurrentModificationError("LinearArrayQueue::Iterator::erase");
  if (!can_erase)
    throw CannotEraseError("LinearArrayQueue::Iterator::erase Iterator cursor already erased");

  if (current < 0 ||
      current >= ref_queue->used)
    throw CannotEraseError("LinearArrayQueue::Iterator::erase Iterator cursor beyond data structure");

  can_erase = false;
  T toReturn = ref_queue->queue[current];
  ref_queue->erase_at(current);
  --current;
  expected_mod_count = ref_queue->mod_count;
  return toReturn;
}


template<class T>
std::string LinearArrayQueue<T>::Iterator::str() const {
  std::ostringstream answer;
  answer << ref_queue->str() << "(current=" << current << ",expected_mod_count=" << expected_mod_count << ",can_erase=" << can_erase << ")";
  return answer.str();
}


template<class T>
const ics::Iterator<T>& LinearArrayQueue<T>::Iterator::operator ++ () {
  if (expected_mod_count != ref_queue->mod_count)
    throw ConcurrentModificationError("LinearArrayQueue::Iterator::operator ++");

  if (current != ref_queue->used) {
    ++current;
    can_erase = true;
  }
  return *this;
}


//KLUDGE: creates garbage! (can return local value!)
template<class T>
const ics::Iterator<T>& LinearArrayQueue<T>::Iterator::operator ++ (int) {
  if (expected_mod_count != ref_queue->mod_count)
    throw ConcurrentModificationError("LinearArrayQueue::Iterator::operator ++(int)");
  Iterator* toReturn = new Iterator(this->ref_queue,current-1);
  if (current != ref_queue->used) {
    ++current;
    can_erase = true;
  }
  return *toReturn;
}


template<class T>
bool LinearArrayQueue<T>::Iterator::operator == (const ics::Iterator<T>& rhs) const {
  const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
  if (rhsASI == 0)
    throw ics::IteratorTypeError("LinearArrayQueue::Iterator::operator ==");
  if (expected_mod_count != ref_queue->mod_count)
    throw ConcurrentModificationError("LinearArrayQueue::Iterator::operator ==");
  if (ref_queue != rhsASI->ref_queue)
    throw ComparingDifferentIteratorsError("LinearArrayQueue::Iterator::operator ==");

  return current == rhsASI->current;
}


template<class T>
bool LinearArrayQueue<T>::Iterator::operator != (const ics::Iterator<T>& rhs) const {
  const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
  if (rhsASI == 0)
    throw IteratorTypeError("LinearArrayQueue::Iterator::operator !=");
  if (expected_mod_count != ref_queue->mod_count)
    throw ConcurrentModificationError("LinearArrayQueue::Iterator::operator !=");
  if (ref_queue != rhsASI->ref_queue)
    throw ComparingDifferentIteratorsError("LinearArrayQueue::Iterator::operator !=");

  return current != rhsASI->current;
}


template<class T>
T& LinearArrayQueue<T>::Iterator::operator *() const {
  if (expected_mod_count !=
      ref_queue->mod_count)
    throw ConcurrentModificationError("LinearArrayQueue::Iterator::operator *");
  if (current < 0 ||
      current >= ref_queue->used) {
    std::ostringstream where;
    where << current << " when size = " << ref_queue->size();
    throw IteratorPositionIllegal("LinearArrayQueue::Iterator::operator * Iterator illegal: "+where.str());
  }
  return ref_queue->queue[current];
}


template<class T>
T* LinearArrayQueue<T>::Iterator::operator ->() const {
  if (expected_mod_count !=
      ref_queue->mod_count)
    throw ConcurrentModificationError("LinearArrayQueue::Iterator::operator *");
  if (current < 0 ||
      current >= ref_queue->used) {
    std::ostringstream where;
    where << current << " when size = " << ref_queue->size();
    throw IteratorPositionIllegal("LinearArrayQueue::Iterator::operator * Iterator illegal: "+where.str());
  }
  return &ref_queue->queue[current];
}

}

#endif /* LINEAR_ARRAY_QUEUE_HPP_ */
