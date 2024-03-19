#ifndef QUEUE_H_
#define QUEUE_H_

#include <cstddef>

template <class T>
struct NodeQueue {
  NodeQueue<T>* next = nullptr;
  T value;
};

template <class T>
class Queue {
 private:
  NodeQueue<T>* front = nullptr;
  NodeQueue<T>* back = nullptr;
  size_t size = 0;
  mutable int num_operations = 0;

 public:
  static Queue<T> Generator(const T* array, size_t size) {
    Queue<T> obj;
    for (size_t i = 0; i < size; ++i) {
      obj.PushBack(array[i]);
    }
    return obj;
  }
  void PushBack(const T& value);
  T PopFront();
  T Front() const;
  size_t Size() const;
  void Clear();
};

template <class T>
void Queue<T>::PushBack(const T& value) {
  ++num_operations;
  if (size) {
    back->next = new NodeQueue<T>{nullptr, value};
    back = back->next;
  } else {
    front = back = new NodeQueue<T>{nullptr, value};
  }
  ++size;
}

template <class T>
T Queue<T>::PopFront() {
  ++num_operations;
  --size;
  T value = front->value;
  if (size) {
    NodeQueue<T>* p_node = front;
    front = p_node->next;
    delete p_node;
  } else {
    delete front;
    front = back = nullptr;
  }
  return value;
}

template <class T>
T Queue<T>::Front() const {
  ++num_operations;
  return front->value;
}

template <class T>
size_t Queue<T>::Size() const {
  ++num_operations;
  return size;
}

template <class T>
void Queue<T>::Clear() {
  ++num_operations;
  size = 0;
  while (front != back) {
    NodeQueue<T>* p_node = front;
    front = p_node->next;
    delete p_node;
  }
  delete front;
  front = back = nullptr;
}

#endif