#ifndef STACK_H_
#define STACK_H_

#include <cstddef>

template <class T>
struct NodeStack {
  NodeStack<T>* next = nullptr;
  T value;
};

template <class T>
struct Stack {
  NodeStack<T>* back = nullptr;
  size_t size = 0;
};

template <class T>
void PushBack(Stack<T>& stack, const T& value) {
  ++stack.size;
  // В динамической памяти, тк иначе NodeStack уничтожится при выходе из функции
  NodeStack<T>* p_node = new NodeStack<T>{stack.back, value};
  stack.back = p_node;
}

template <class T>
T PopBack(Stack<T>& stack) {
  --stack.size;
  NodeStack<T>* p_node = stack.back;
  stack.back = p_node->next;
  T value = p_node->value;
  delete p_node;
  return value;
}

template <class T>
T Back(const Stack<T>& stack) {
  return stack.back->value;
}

template <class T>
void Clear(Stack<T>& stack) {
  stack.size = 0;
  while (stack.back) {
    NodeStack<T>* p_node = stack.back;
    stack.back = p_node->next;
    delete p_node;
  }
}

#endif