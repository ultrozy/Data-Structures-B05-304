#ifndef DEQUE_H_
#define DEQUE_H_

#include <iterator>
#include <type_traits>
#include <cstddef>

template <class T>
struct NodeDeque {
  NodeDeque<T>* prev = nullptr;
  NodeDeque<T>* next = nullptr;
  T value;
};

template <class T>
class IteratorDeque {
 private:
  NodeDeque<T>* ptr_;

 public:
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = std::remove_cv_t<std::remove_reference_t<T>>;
  using difference_type = std::ptrdiff_t;
  using reference_type = T&;
  using pointer_type = T*;

  IteratorDeque(NodeDeque<T>*);

  bool operator==(IteratorDeque);
  bool operator!=(IteratorDeque);
  reference_type operator*();
  pointer_type operator->();
  IteratorDeque& operator++();
  IteratorDeque operator++(int);
  IteratorDeque& operator--();
  IteratorDeque operator--(int);
};

template <class T>
IteratorDeque<T>::IteratorDeque(NodeDeque<T>* ptr) : ptr_{ptr} {
}

template <class T>
bool IteratorDeque<T>::operator==(IteratorDeque other) {
  return ptr_ == other.ptr_;
}

template <class T>
bool IteratorDeque<T>::operator!=(IteratorDeque other) {
  return ptr_ != other.ptr_;
}

template <class T>
IteratorDeque<T>::reference_type IteratorDeque<T>::operator*() {
  return ptr_->value;
}

template <class T>
IteratorDeque<T>::pointer_type IteratorDeque<T>::operator->() {
  return &ptr_->value;
}

template <class T>
IteratorDeque<T>& IteratorDeque<T>::operator++() {
  ptr_ = ptr_->next;
  return *this;
}

template <class T>
IteratorDeque<T> IteratorDeque<T>::operator++(int) {
  IteratorDeque<T> old(ptr_);
  ptr_ = ptr_->next;
  return old;
}

template <class T>
IteratorDeque<T>& IteratorDeque<T>::operator--() {
  ptr_ = ptr_->prev;
  return *this;
}

template <class T>
IteratorDeque<T> IteratorDeque<T>::operator--(int) {
  IteratorDeque<T> old(ptr_);
  ptr_ = ptr_->prev;
  return old;
}

//////////////////////
// CLASS DEFINITION //
//////////////////////

template <class T>
class Deque {
 private:
  NodeDeque<T>* last = nullptr;
  NodeDeque<T>* first = nullptr;
  size_t size = 0;

 public:
  using Iterator = IteratorDeque<T>;
  using ConstIterator = IteratorDeque<const T>;
  using ReverseIterator = std::reverse_iterator<Iterator>;
  using ConstReverseIterator = std::reverse_iterator<ConstIterator>;

  Iterator begin();
  Iterator end();
  ConstIterator begin() const;
  ConstIterator end() const;
  ConstIterator cbegin() const;
  ConstIterator cend() const;
  ReverseIterator rbegin();
  ReverseIterator rend();
  ConstReverseIterator rbegin() const;
  ConstReverseIterator rend() const;
  ConstReverseIterator crbegin() const;
  ConstReverseIterator crend() const;
};

template <class T>
void PushFirst(Deque<T>& deque, const T& value) {
  if (deque.size) {
    deque.first->prev = new NodeDeque<T>{nullptr, deque.first, value};
    deque.first = deque.first->prev;
  } else {
    deque.first = deque.last = new NodeDeque<T>{nullptr, nullptr, value};
  }
  ++deque.size;
}

template <class T>
void PushLast(Deque<T>& deque, const T& value) {
  if (deque.size) {
    deque.last->next = new NodeDeque<T>{deque.last, nullptr, value};
    deque.last = deque.last->next;
  } else {
    deque.first = deque.last = new NodeDeque<T>{nullptr, nullptr, value};
  }
  ++deque.size;
}

template <class T>
T PopFirst(Deque<T>& deque) {
  --deque.size;
  T value = deque.first->value;
  if (deque.size) {
    NodeDeque<T>* p_node = deque.first;
    deque.first = p_node->next;
    delete p_node;
    deque.first->prev = nullptr;
  } else {
    delete deque.first;
    deque.first = deque.last = nullptr;
  }
  return value;
}

template <class T>
T PopLast(Deque<T>& deque) {
  --deque.size;
  T value = deque.last->value;
  if (deque.size) {
    NodeDeque<T>* p_node = deque.last;
    deque.last = p_node->prev;
    delete p_node;
    deque.last->next = nullptr;
  } else {
    delete deque.last;
    deque.first = deque.last = nullptr;
  }
  return value;
}

#endif

