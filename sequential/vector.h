#ifndef VECTOR_H_
#define VECTOR_H_

#define VECTOR_MEMORY_IMPLEMENTED

#include <cstddef>
#include <iterator>
#include <memory>
#include <algorithm>

namespace detail {
template <class Iterator>
using EnifForwardIt = std::enable_if_t<
    std::is_base_of_v<std::forward_iterator_tag, typename std::iterator_traits<Iterator>::iterator_category>, int>;
}

template <class T>
class Vector;

template <class T>
bool operator==(const Vector<T>&, const Vector<T>&);
template <class T>
bool operator!=(const Vector<T>&, const Vector<T>&);
template <class T>
bool operator<=(const Vector<T>&, const Vector<T>&);
template <class T>
bool operator>=(const Vector<T>&, const Vector<T>&);
template <class T>
bool operator<(const Vector<T>&, const Vector<T>&);
template <class T>
bool operator>(const Vector<T>&, const Vector<T>&);

template <class T>
class Vector {
 private:
  std::byte* array_;
  size_t size_;
  size_t capacity_;

 public:
  using Iterator = T*;
  using ConstIterator = const T*;
  using ReverseIterator = std::reverse_iterator<T*>;
  using ConstReverseIterator = std::reverse_iterator<const T*>;

  Vector() noexcept;
  explicit Vector(size_t);
  Vector(size_t, const T&);
  Vector(std::initializer_list<T>);
  template <class ForwardIt, detail::EnifForwardIt<ForwardIt> = 0>
  Vector(ForwardIt, ForwardIt);

  Vector(const Vector<T>&);
  Vector(Vector<T>&&) noexcept;
  Vector& operator=(const Vector<T>&);
  Vector& operator=(Vector<T>&&) noexcept;
  ~Vector();

  size_t Size() const noexcept;
  size_t Capacity() const noexcept;
  bool Empty() const noexcept;

  T& operator[](size_t) noexcept;
  const T& operator[](size_t) const noexcept;
  T& At(size_t);
  const T& At(size_t) const;
  T& Front() noexcept;
  const T& Front() const noexcept;
  T& Back() noexcept;
  const T& Back() const noexcept;
  T* Data() noexcept;
  const T* Data() const noexcept;

  void Swap(Vector<T>&) noexcept;
  void Resize(size_t);
  void Resize(size_t, const T&);
  void Reserve(size_t);
  void ShrinkToFit();
  void Clear() noexcept;
  void PushBack(const T&);
  void PushBack(T&&);
  void PopBack() noexcept;

  Iterator begin() noexcept;
  ConstIterator begin() const noexcept;
  ConstIterator cbegin() const noexcept;
  Iterator end() noexcept;
  ConstIterator end() const noexcept;
  ConstIterator cend() const noexcept;
  ReverseIterator rbegin() noexcept;
  ConstReverseIterator rbegin() const noexcept;
  ConstReverseIterator crbegin() const noexcept;
  ReverseIterator rend() noexcept;
  ConstReverseIterator rend() const noexcept;
  ConstReverseIterator crend() const noexcept;

  friend bool operator== <T>(const Vector<T>&, const Vector<T>&);
  friend bool operator!= <T>(const Vector<T>&, const Vector<T>&);
  friend bool operator<= <T>(const Vector<T>&, const Vector<T>&);
  friend bool operator>= <T>(const Vector<T>&, const Vector<T>&);
  friend bool operator< <T>(const Vector<T>&, const Vector<T>&);
  friend bool operator><T>(const Vector<T>&, const Vector<T>&);
};

template <class T>
Vector<T>::Vector() noexcept : array_{nullptr}, size_{0ul}, capacity_{0ul} {
}

template <class T>
Vector<T>::Vector(size_t n) : array_{nullptr}, size_{n}, capacity_{n} {
  try {
    array_ = new std::byte[size_ * sizeof(T)];
    std::uninitialized_default_construct(begin(), end());
  } catch (...) {
    delete[] array_;
    throw;
  }
}

template <class T>
Vector<T>::Vector(size_t n, const T& value) : array_{nullptr}, size_{n}, capacity_{n} {
  try {
    array_ = new std::byte[size_ * sizeof(T)];
    std::uninitialized_fill(begin(), end(), value);
  } catch (...) {
    delete[] array_;
    throw;
  }
}
template <class T>
Vector<T>::Vector(std::initializer_list<T> ilist) : array_{nullptr}, size_{ilist.size()}, capacity_{size_} {
  try {
    array_ = new std::byte[size_ * sizeof(T)];
    std::uninitialized_copy(ilist.begin(), ilist.end(), begin());
  } catch (...) {
    delete[] array_;
    throw;
  }
}

template <class T>
template <class ForwardIt, detail::EnifForwardIt<ForwardIt>>
Vector<T>::Vector(ForwardIt start, ForwardIt finish)
    : array_{nullptr}, size_{std::distance(start, finish)}, capacity_{size_} {
  try {
    array_ = new std::byte[size_ * sizeof(T)];
    std::uninitialized_copy(start, finish, begin());
  } catch (...) {
    delete[] array_;
    throw;
  }
}

template <class T>
Vector<T>::Vector(const Vector<T>& other) : array_{nullptr}, size_{other.size_}, capacity_{other.capacity_} {
  try {
    array_ = new std::byte[capacity_ * sizeof(T)];
    std::uninitialized_copy(other.begin(), other.end(), begin());
  } catch (...) {
    delete[] array_;
    throw;
  }
}
template <class T>
Vector<T>::Vector(Vector<T>&&) noexcept {
}
template <class T>
Vector& Vector<T>::operator=(const Vector<T>&) {
}
template <class T>
Vector& Vector<T>::operator=(Vector<T>&&) noexcept {
}
template <class T>
Vector<T>::~Vector() {
}

template <class T>
Vector<T>::Iterator Vector<T>::begin() noexcept {
  return reinterpret_cast<T*>(array_);
}

template <class T>
Vector<T>::ReverseIterator Vector<T>::rbegin() noexcept {
  return ReverseIterator(reinterpret_cast<T*>(array_ + size_));
}

#endif