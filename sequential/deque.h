#ifndef DEQUE_H_
#define DEQUE_H_

#include <cstddef>

template <class T>
struct NodeDeque {
  NodeDeque<T>* prev = nullptr;
  NodeDeque<T>* next = nullptr;
  T value;
};

template <class T>
struct Deque {
  NodeDeque<T>* first = nullptr;
  NodeDeque<T>* last = nullptr;
  size_t size = 0;
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

template <class T>
void InsertAfter(NodeDeque<T>* p_curr_node, const T& value) {
  NodeDeque<T>* p_new_node = new NodeDeque<T>{p_curr_node, p_curr_node->next, value};
  p_curr_node->next = p_new_node;
  if (p_new_node->next) {
    p_new_node->next->prev = p_new_node;
  }
}

template <class T>
void Remove(NodeDeque<T>* p_curr_node) {
  p_curr_node->next->prev = p_curr_node->prev;
  p_curr_node->prev->next = p_curr_node->next;
  delete p_curr_node;
}

#endif