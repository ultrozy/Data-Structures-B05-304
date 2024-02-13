#ifndef STACK_H_
#define STACK_H_

#include <cstddef>
#include <utility>
#include <iostream>

template <class T>
struct NodeStack {
  NodeStack<T>* next = nullptr;
  T value;
};

template <class T>
class Stack {
  NodeStack<T>* back_ = nullptr;
  size_t size_ = 0;

 public:
  Stack() = default;
  void PushBack(const T&);
  T PopBack();
  T& Back();
  const T& Back() const;
  void Clear();

  ~Stack();
  Stack(const Stack<T>&);
  Stack<T>& operator=(const Stack<T>&);
  Stack(Stack<T>&&) noexcept;
  Stack<T>& operator=(Stack<T>&&) noexcept;

  void Print() const;

  template <class U>
  friend void Swap(Stack<U>&, Stack<U>&);
};

template <class T>
void Stack<T>::PushBack(const T& value) {
  ++size_;
  back_ = new NodeStack<T>{back_, value};
}

template <class T>
T Stack<T>::PopBack() {
  --size_;
  T value = back_->value;
  delete std::exchange(back_, back_->next);
  return value;
}

template <class T>
T& Stack<T>::Back() {
  return back_->value;
}

template <class T>
const T& Stack<T>::Back() const {
  return back_->value;
}

template <class T>
void Stack<T>::Clear() {
  size_ = 0;
  while (back_) {
    delete std::exchange(back_, back_->next);
  }
}

template <class U>
void Swap(Stack<U>& fst, Stack<U>& snd) {
  std::swap(fst.back_, snd.back_);
  std::swap(fst.size_, snd.size_);
}

/* Правило пяти */

// Деструктор
template <class T>
Stack<T>::~Stack() {
  Clear();
}

// Конструктор копирования
template <class T>
Stack<T>::Stack(const Stack<T>& other) : back_{nullptr}, size_{other.size_} {
  if (!other.back_) {
    return;
  }
  NodeStack<T>* p_node = back_ = new NodeStack<T>{nullptr, other.back_->value};
  NodeStack<T>* p_other_node = other.back_;
  while (p_other_node->next) {
    p_other_node = p_other_node->next;
    p_node = p_node->next = new NodeStack<T>{nullptr, p_other_node->value};
  }
}

// Оператор присваивания копированием
template <class T>
Stack<T>& Stack<T>::operator=(const Stack<T>& other) {
  if (this == &other) {
    return *this;
  }
  Clear();
  if (!other.back_) {
    return *this;
  }
  NodeStack<T>* p_node = back_ = new NodeStack<T>{nullptr, other.back_->value};
  NodeStack<T>* p_other_node = other.back_;
  while (p_other_node->next) {
    p_other_node = p_other_node->next;
    p_node = p_node->next = new NodeStack<T>{nullptr, p_other_node->value};
  }
  return *this;
}

// Конструктор перемещения
template <class T>
Stack<T>::Stack(Stack<T>&& other) noexcept
    : back_{std::exchange(other.back_, nullptr)}  //
    , size_{std::exchange(other.size_, 0)} {
}

// Оператор присваивания перемещением
template <class T>
Stack<T>& Stack<T>::operator=(Stack<T>&& other) noexcept {
  if (this == &other) {
    return *this;
  }
  Clear();
  back_ = std::exchange(other.back_, nullptr);
  size_ = std::exchange(other.size_, 0);
  return *this;
}

// Move and Swap Idiom
// template <class T>
// Stack<T>::Stack(Stack<T>&& other) noexcept {
//   Swap(*this, other);
// }

// template <class T>
// Stack<T>& Stack<T>::operator=(Stack<T>&& other) noexcept {
//   Stack<T> temp(std::move(other));
//   Swap(*this, temp);
// }

/* Правило пяти*/

template <class T>
void Stack<T>::Print() const {
  std::cout << "-> ";
  auto p_node = back_;
  while (p_node) {
    std::cout << std::exchange(p_node, p_node->next)->value << ' ';
  }
  std::cout << '\n';
}

#endif