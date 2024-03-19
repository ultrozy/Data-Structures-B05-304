#ifndef BINARY_HEAP_H_
#define BINARY_HEAP_H_

#include <cstddef>
#include <utility>
#include <ostream>
#include <vector>

template <class T>
class BinaryHeap;

template <class T>
std::ostream& operator<<(std::ostream&, const BinaryHeap<T>&);

template <class T>
class BinaryHeap {
 private:
  std::vector<T> array_;

 public:
  BinaryHeap();
  BinaryHeap(std::initializer_list<T>);

  size_t Size() const;
  const T& GetMin() const;
  T ExtractMin();
  size_t Insert(const T&);
  size_t Insert(T&&);
  size_t ChangeKey(size_t, const T&);
  size_t ChangeKey(size_t, T&&);

 private:
  void Heapify();
  size_t SiftUp(size_t index);
  size_t SiftDown(size_t index);

  friend std::ostream& operator<< <T>(std::ostream&, const BinaryHeap<T>&);
};

template <class T>
BinaryHeap<T>::BinaryHeap() : array_{} {
}

template <class T>
BinaryHeap<T>::BinaryHeap(std::initializer_list<T> list) : array_(list.begin(), list.end()) {
  Heapify();
}

template <class T>
size_t BinaryHeap<T>::Size() const {
  return array_.size();
}

template <class T>
const T& BinaryHeap<T>::GetMin() const {
  return array_[0];
}

template <class T>
T BinaryHeap<T>::ExtractMin() {
  T value(std::move(array_[0]));
  array_[0] = std::move(array_.back());
  array_.pop_back();
  if (!array_.empty()) {
    SiftDown(0ul);
  }
  return value;
}

template <class T>
size_t BinaryHeap<T>::Insert(const T& value) {
  array_.push_back(value);
  return SiftUp(array_.size() - 1ul);
}

template <class T>
size_t BinaryHeap<T>::Insert(T&& value) {
  array_.push_back(std::move(value));
  return SiftUp(array_.size() - 1ul);
}

template <class T>
size_t BinaryHeap<T>::ChangeKey(size_t index, const T& value) {
  if (array_[index] > value) {
    array_[index] = value;
    return SiftUp(index);
  }
  array_[index] = value;
  return SiftDown(index);
}

template <class T>
size_t BinaryHeap<T>::ChangeKey(size_t index, T&& value) {
  if (array_[index] > value) {
    array_[index] = std::move(value);
    return SiftUp(index);
  }
  array_[index] = std::move(value);
  return SiftDown(index);
}

template <class T>
void BinaryHeap<T>::Heapify() {
  if (array_.size() < 2) {
    return;
  }
  for (size_t i = (array_.size() >> 1) - 1; i; --i) {
    SiftDown(i);
  }
  SiftDown(0ul);
}

template <class T>
size_t BinaryHeap<T>::SiftUp(size_t index) {
  while (index) {
    size_t parent = (index - 1) >> 1;
    if (array_[parent] > array_[index]) {
      std::swap(array_[index], array_[parent]);
      index = parent;
    } else {
      return index;
    }
  }
  return 0ul;
}

template <class T>
size_t BinaryHeap<T>::SiftDown(size_t index) {
  size_t smallest = index;
  do {
    std::swap(array_[index], array_[smallest]);
    index = smallest;
    size_t left = (index << 1) + 1;
    size_t right = left + 1;
    if (left < array_.size() && array_[smallest] > array_[left]) {
      smallest = left;
    }
    if (right < array_.size() && array_[smallest] > array_[right]) {
      smallest = right;
    }
  } while (index < smallest);
  return index;
}

template <class T>
std::ostream& operator<<(std::ostream& os, const BinaryHeap<T>& heap) {
  for (const auto& value : heap.array_) {
    os << value << ' ';
  }
  return os;
}

#endif