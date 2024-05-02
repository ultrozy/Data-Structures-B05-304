#ifndef VECTOR_H_
#define VECTOR_H_

#define VECTOR_MEMORY_IMPLEMENTED

#include <cstddef>
#include <iterator>
#include <type_traits>
#include <utility>
#include <memory>
#include <stdexcept>
#include <algorithm>

namespace detail_vector {
template <class ForwardIt>
using EnifForwardIt = std::enable_if_t<
    std::is_base_of_v<std::forward_iterator_tag, typename std::iterator_traits<ForwardIt>::iterator_category>, int>;
}

template <class T>
class Vector;
template <class T>
bool operator==(const Vector<T>&, const Vector<T>&) noexcept;
template <class T>
bool operator!=(const Vector<T>&, const Vector<T>&) noexcept;
template <class T>
bool operator>=(const Vector<T>&, const Vector<T>&) noexcept;
template <class T>
bool operator<=(const Vector<T>&, const Vector<T>&) noexcept;
template <class T>
bool operator>(const Vector<T>&, const Vector<T>&) noexcept;
template <class T>
bool operator<(const Vector<T>&, const Vector<T>&) noexcept;

template <class T>
class Vector {
 private:
  using MemT = std::aligned_storage_t<sizeof(T), alignof(T)>;
  MemT* start_data_;
  MemT* finish_data_;
  MemT* finish_memory_;

  static T* ObjPtr(MemT*) noexcept;
  static const T* ObjPtr(const MemT*) noexcept;

 public:
  using ValueType = T;
  using Pointer = T*;
  using ConstPointer = const T*;
  using Reference = T&;
  using ConstReference = const T&;
  using SizeType = size_t;
  using Iterator = T*;
  using ConstIterator = const T*;
  using ReverseIterator = std::reverse_iterator<T*>;
  using ConstReverseIterator = std::reverse_iterator<const T*>;

  Vector() noexcept;
  explicit Vector(SizeType);
  Vector(SizeType, const T&);
  template <class ForwardIt, detail_vector::EnifForwardIt<ForwardIt> = 0>
  Vector(ForwardIt, ForwardIt);
  Vector(std::initializer_list<T>);

  void Swap(Vector<T>&) noexcept;
  Vector(const Vector<T>&);
  Vector(Vector<T>&&) noexcept;
  Vector& operator=(const Vector<T>&);
  Vector& operator=(Vector<T>&&) noexcept;
  ~Vector();

  SizeType Size() const noexcept;
  SizeType Capacity() const noexcept;
  bool Empty() const noexcept;

  T* Data() noexcept;
  T& Front() noexcept;
  T& Back() noexcept;
  T& operator[](SizeType) noexcept;
  T& At(SizeType);
  const T* Data() const noexcept;
  const T& Front() const noexcept;
  const T& Back() const noexcept;
  const T& operator[](SizeType) const noexcept;
  const T& At(SizeType) const;

 private:
  SizeType NextCapacity() const noexcept;

 public:
  void Resize(SizeType);
  void Resize(SizeType, const T&);
  void Reserve(SizeType);
  void ShrinkToFit();

  template <class... Args>
  void EmplaceBack(Args&&...);
  void PushBack(const T&);
  void PushBack(T&&);
  void PopBack() noexcept;
  void Clear() noexcept;

  Iterator begin() noexcept;                      // NOLINT
  Iterator end() noexcept;                        // NOLINT
  ConstIterator begin() const noexcept;           // NOLINT
  ConstIterator end() const noexcept;             // NOLINT
  ConstIterator cbegin() const noexcept;          // NOLINT
  ConstIterator cend() const noexcept;            // NOLINT
  ReverseIterator rbegin() noexcept;              // NOLINT
  ReverseIterator rend() noexcept;                // NOLINT
  ConstReverseIterator rbegin() const noexcept;   // NOLINT
  ConstReverseIterator rend() const noexcept;     // NOLINT
  ConstReverseIterator crbegin() const noexcept;  // NOLINT
  ConstReverseIterator crend() const noexcept;    // NOLINT

  friend bool operator== <T>(const Vector<T>&, const Vector<T>&) noexcept;
  friend bool operator!= <T>(const Vector<T>&, const Vector<T>&) noexcept;
  friend bool operator>= <T>(const Vector<T>&, const Vector<T>&) noexcept;
  friend bool operator<= <T>(const Vector<T>&, const Vector<T>&) noexcept;
  friend bool operator><T>(const Vector<T>&, const Vector<T>&) noexcept;
  friend bool operator< <T>(const Vector<T>&, const Vector<T>&) noexcept;
};

template <class T>
T* Vector<T>::ObjPtr(Vector<T>::MemT* ptr) noexcept {
  return reinterpret_cast<T*>(ptr);
}
template <class T>
const T* Vector<T>::ObjPtr(const Vector<T>::MemT* ptr) noexcept {
  return reinterpret_cast<const T*>(ptr);
}

////////////////////////
////  CONSTRUCTORS  ////
////////////////////////

template <class T>
Vector<T>::Vector() noexcept : start_data_{nullptr}, finish_data_{nullptr}, finish_memory_{nullptr} {
}

template <class T>
Vector<T>::Vector(Vector<T>::SizeType size) : start_data_{nullptr}, finish_data_{nullptr}, finish_memory_{nullptr} {
  if (!size) {
    return;
  }
  start_data_ = new MemT[size];
  try {
    std::uninitialized_default_construct(begin(), begin() + size);
  } catch (...) {
    delete[] start_data_;
    throw;
  }
  finish_data_ = finish_memory_ = start_data_ + size;
}

template <class T>
Vector<T>::Vector(Vector<T>::SizeType size, const T& value)
    : start_data_{nullptr}, finish_data_{nullptr}, finish_memory_{nullptr} {
  if (!size) {
    return;
  }
  start_data_ = new MemT[size];
  try {
    std::uninitialized_fill(begin(), begin() + size, value);
  } catch (...) {
    delete[] start_data_;
    throw;
  }
  finish_data_ = finish_memory_ = start_data_ + size;
}

template <class T>
template <class ForwardIt, detail_vector::EnifForwardIt<ForwardIt>>
Vector<T>::Vector(ForwardIt it_begin, ForwardIt it_end)
    : start_data_{nullptr}, finish_data_{nullptr}, finish_memory_{nullptr} {
  if (it_begin == it_end) {
    return;
  }
  SizeType size = std::distance(it_begin, it_end);
  start_data_ = new MemT[size];
  try {
    std::uninitialized_copy(it_begin, it_end, begin());
  } catch (...) {
    delete[] start_data_;
    throw;
  }
  finish_data_ = finish_memory_ = start_data_ + size;
}

template <class T>
Vector<T>::Vector(std::initializer_list<T> ilist)
    : start_data_{nullptr}, finish_data_{nullptr}, finish_memory_{nullptr} {
  if (!ilist.size()) {
    return;
  }
  start_data_ = new MemT[ilist.size()];
  try {
    std::uninitialized_copy(ilist.begin(), ilist.end(), begin());
  } catch (...) {
    delete[] start_data_;
    throw;
  }
  finish_data_ = finish_memory_ = start_data_ + ilist.size();
}

////////////////////////////
////  THE RULE OF FIVE  ////
////////////////////////////

template <class T>
void Vector<T>::Swap(Vector<T>& other) noexcept {
  std::swap(start_data_, other.start_data_);
  std::swap(finish_data_, other.finish_data_);
  std::swap(finish_memory_, other.finish_memory_);
}

template <class T>
Vector<T>::Vector(const Vector<T>& other) : start_data_{nullptr}, finish_data_{nullptr}, finish_memory_{nullptr} {
  if (other.start_data_ == other.finish_data_) {
    return;
  }
  start_data_ = new MemT[other.Size()];
  try {
    std::uninitialized_copy(other.cbegin(), other.cend(), begin());
  } catch (...) {
    delete[] start_data_;
    throw;
  }
  finish_data_ = finish_memory_ = start_data_ + other.Size();
}

template <class T>
Vector<T>::Vector(Vector<T>&& other) noexcept
    : start_data_{std::exchange(other.start_data_, nullptr)}
    , finish_data_{std::exchange(other.finish_data_, nullptr)}
    , finish_memory_{std::exchange(other.finish_memory_, nullptr)} {
}

template <class T>
Vector<T>& Vector<T>::operator=(const Vector<T>& other) {
  if (this == &other) {
    return *this;
  }

  // Allocation required
  if (Capacity() < other.Size()) {
    MemT* new_storage = new MemT[other.Size()];
    try {
      std::uninitialized_copy(other.cbegin(), other.cend(), ObjPtr(new_storage));
    } catch (...) {
      delete[] new_storage;
      throw;
    }
    std::destroy(begin(), end());
    delete[] std::exchange(start_data_, new_storage);
    finish_data_ = finish_memory_ = start_data_ + other.Size();
    return *this;
  }

  // No allocation
  if (Size() < other.Size()) {
    std::copy_n(other.cbegin(), Size(), begin());
    std::uninitialized_copy(other.cbegin() + Size(), other.cend(), end());
  } else {
    std::copy(other.cbegin(), other.cend(), begin());
    std::destroy(begin() + other.Size(), end());
  }
  finish_data_ = start_data_ + other.Size();
  return *this;
}

template <class T>
Vector<T>& Vector<T>::operator=(Vector<T>&& other) noexcept {
  if (this == &other) {
    return *this;
  }
  std::destroy(begin(), end());
  delete[] start_data_;

  start_data_ = std::exchange(other.start_data_, nullptr);
  finish_data_ = std::exchange(other.finish_data_, nullptr);
  finish_memory_ = std::exchange(other.finish_memory_, nullptr);
  return *this;
}

template <class T>
Vector<T>::~Vector() {
  std::destroy(begin(), end());
  delete[] start_data_;
}

////////////////
////  INFO  ////
////////////////

template <class T>
Vector<T>::SizeType Vector<T>::Size() const noexcept {
  return finish_data_ - start_data_;
}

template <class T>
Vector<T>::SizeType Vector<T>::Capacity() const noexcept {
  return finish_memory_ - start_data_;
}

template <class T>
bool Vector<T>::Empty() const noexcept {
  return start_data_ == finish_data_;
}

//////////////////
////  ACCESS  ////
//////////////////

template <class T>
T* Vector<T>::Data() noexcept {
  return reinterpret_cast<T*>(start_data_);
}

template <class T>
T& Vector<T>::Front() noexcept {
  return *reinterpret_cast<T*>(start_data_);
}

template <class T>
T& Vector<T>::Back() noexcept {
  return *reinterpret_cast<T*>(finish_data_ - 1);
}

template <class T>
T& Vector<T>::operator[](Vector<T>::SizeType index) noexcept {
  return *reinterpret_cast<T*>(start_data_ + index);
}

template <class T>
T& Vector<T>::At(Vector<T>::SizeType index) {
  if (index >= Size()) {
    throw std::out_of_range("Range check failed in Vector<T>::At(size_t)");
  }
  return *reinterpret_cast<T*>(start_data_ + index);
}

template <class T>
const T* Vector<T>::Data() const noexcept {
  return reinterpret_cast<const T*>(start_data_);
}

template <class T>
const T& Vector<T>::Front() const noexcept {
  return *reinterpret_cast<const T*>(start_data_);
}

template <class T>
const T& Vector<T>::Back() const noexcept {
  return *reinterpret_cast<const T*>(finish_data_ - 1);
}

template <class T>
const T& Vector<T>::operator[](Vector<T>::SizeType index) const noexcept {
  return *reinterpret_cast<const T*>(start_data_ + index);
}

template <class T>
const T& Vector<T>::At(Vector<T>::SizeType index) const {
  if (index >= Size()) {
    throw std::out_of_range("Range check failed in Vector<T>::At(size_t) const");
  }
  return *reinterpret_cast<const T*>(start_data_ + index);
}

//////////////////
////  MODIFY  ////
//////////////////

template <class T>
Vector<T>::SizeType Vector<T>::NextCapacity() const noexcept {
  return (Capacity() << 1) + 1;
}

template <class T>
void Vector<T>::Resize(Vector<T>::SizeType new_size) {
  if (new_size <= Size()) {
    std::destroy(begin() + new_size, end());
    finish_data_ = start_data_ + new_size;
    return;
  }
  if (new_size <= Capacity()) {
    std::uninitialized_default_construct(end(), begin() + new_size);
    finish_data_ = start_data_ + new_size;
    return;
  }
  MemT* new_storage = new MemT[new_size];
  try {
    std::uninitialized_default_construct(ObjPtr(new_storage) + Size(), ObjPtr(new_storage) + new_size);
  } catch (...) {
    delete[] new_storage;
    throw;
  }
  std::uninitialized_move(begin(), end(), ObjPtr(new_storage));
  std::destroy(begin(), end());
  delete[] std::exchange(start_data_, new_storage);
  finish_memory_ = finish_data_ = start_data_ + new_size;
}

template <class T>
void Vector<T>::Resize(Vector<T>::SizeType new_size, const T& value) {
  if (new_size <= Size()) {
    std::destroy(begin() + new_size, end());
    finish_data_ = start_data_ + new_size;
    return;
  }
  if (new_size <= Capacity()) {
    std::uninitialized_fill(end(), begin() + new_size, value);
    finish_data_ = start_data_ + new_size;
    return;
  }
  MemT* new_storage = new MemT[new_size];
  try {
    std::uninitialized_fill(ObjPtr(new_storage) + Size(), ObjPtr(new_storage) + new_size, value);
  } catch (...) {
    delete[] new_storage;
    throw;
  }
  std::uninitialized_move(begin(), end(), ObjPtr(new_storage));
  std::destroy(begin(), end());
  delete[] std::exchange(start_data_, new_storage);
  finish_memory_ = finish_data_ = start_data_ + new_size;
}

template <class T>
void Vector<T>::Reserve(Vector<T>::SizeType new_capacity) {
  if (new_capacity <= Capacity()) {
    return;
  }
  MemT* new_storage = new MemT[new_capacity];
  std::uninitialized_move(begin(), end(), ObjPtr(new_storage));
  std::destroy(begin(), end());
  finish_memory_ = new_storage + new_capacity;
  finish_data_ = new_storage + Size();
  delete[] std::exchange(start_data_, new_storage);
}

template <class T>
void Vector<T>::ShrinkToFit() {
  if (finish_data_ == finish_memory_) {
    return;
  }
  if (start_data_ == finish_data_) {
    delete[] start_data_;
    start_data_ = finish_data_ = finish_memory_ = nullptr;
    return;
  }
  MemT* new_storage = new MemT[Size()];
  std::uninitialized_move(begin(), end(), ObjPtr(new_storage));
  std::destroy(begin(), end());
  finish_memory_ = finish_data_ = new_storage + Size();
  delete[] std::exchange(start_data_, new_storage);
}

template <class T>
template <class... Args>
void Vector<T>::EmplaceBack(Args&&... args) {
  if (finish_data_ != finish_memory_) {
    new (finish_data_) T(std::forward<Args>(args)...);
    ++finish_data_;
    return;
  }
  MemT* new_storage = new MemT[NextCapacity()];
  try {
    new (new_storage + Size()) T(std::forward<Args>(args)...);
  } catch (...) {
    delete[] new_storage;
    throw;
  }
  std::uninitialized_move(begin(), end(), ObjPtr(new_storage));
  std::destroy(begin(), end());
  finish_memory_ = new_storage + NextCapacity();
  finish_data_ = new_storage + Size() + 1;
  delete[] std::exchange(start_data_, new_storage);
}

template <class T>
void Vector<T>::PushBack(const T& value) {
  EmplaceBack(value);
}

template <class T>
void Vector<T>::PushBack(T&& value) {
  EmplaceBack(std::move(value));
}

template <class T>
void Vector<T>::PopBack() noexcept {
  std::destroy_at(reinterpret_cast<T*>(--finish_data_));
}

template <class T>
void Vector<T>::Clear() noexcept {
  std::destroy(begin(), end());
  finish_data_ = start_data_;
}

/////////////////////
////  ITERATORS  ////
/////////////////////

template <class T>
Vector<T>::Iterator Vector<T>::begin() noexcept {
  return reinterpret_cast<T*>(start_data_);
}

template <class T>
Vector<T>::Iterator Vector<T>::end() noexcept {
  return reinterpret_cast<T*>(finish_data_);
}

template <class T>
Vector<T>::ConstIterator Vector<T>::begin() const noexcept {
  return reinterpret_cast<const T*>(start_data_);
}

template <class T>
Vector<T>::ConstIterator Vector<T>::end() const noexcept {
  return reinterpret_cast<const T*>(finish_data_);
}

template <class T>
Vector<T>::ConstIterator Vector<T>::cbegin() const noexcept {
  return reinterpret_cast<const T*>(start_data_);
}

template <class T>
Vector<T>::ConstIterator Vector<T>::cend() const noexcept {
  return reinterpret_cast<const T*>(finish_data_);
}

template <class T>
Vector<T>::ReverseIterator Vector<T>::rbegin() noexcept {
  return ReverseIterator(reinterpret_cast<T*>(finish_data_));
}

template <class T>
Vector<T>::ReverseIterator Vector<T>::rend() noexcept {
  return ReverseIterator(reinterpret_cast<T*>(start_data_));
}

template <class T>
Vector<T>::ConstReverseIterator Vector<T>::rbegin() const noexcept {
  return ConstReverseIterator(reinterpret_cast<const T*>(finish_data_));
}

template <class T>
Vector<T>::ConstReverseIterator Vector<T>::rend() const noexcept {
  return ConstReverseIterator(reinterpret_cast<const T*>(start_data_));
}

template <class T>
Vector<T>::ConstReverseIterator Vector<T>::crbegin() const noexcept {
  return ConstReverseIterator(reinterpret_cast<const T*>(finish_data_));
}

template <class T>
Vector<T>::ConstReverseIterator Vector<T>::crend() const noexcept {
  return ConstReverseIterator(reinterpret_cast<const T*>(start_data_));
}

///////////////////////
////  COMPARISONS  ////
///////////////////////

template <class T>
bool operator==(const Vector<T>& lhs, const Vector<T>& rhs) noexcept {
  auto l_it = lhs.begin();
  auto r_it = rhs.begin();
  while (l_it != lhs.end() && r_it != rhs.end() && *l_it == *r_it) {
    ++l_it;
    ++r_it;
  }
  return l_it == lhs.end() && r_it == rhs.end();
}

template <class T>
bool operator!=(const Vector<T>& lhs, const Vector<T>& rhs) noexcept {
  auto l_it = lhs.begin();
  auto r_it = rhs.begin();
  while (l_it != lhs.end() && r_it != rhs.end() && *l_it == *r_it) {
    ++l_it;
    ++r_it;
  }
  return l_it != lhs.end() || r_it != rhs.end();
}

template <class T>
bool operator>=(const Vector<T>& lhs, const Vector<T>& rhs) noexcept {
  auto l_it = lhs.begin();
  auto r_it = rhs.begin();
  while (l_it != lhs.end() && r_it != rhs.end() && *l_it == *r_it) {
    ++l_it;
    ++r_it;
  }
  if (l_it != lhs.end() && r_it != rhs.end()) {
    return *l_it > *r_it;
  }
  return r_it == rhs.end();
}

template <class T>
bool operator<=(const Vector<T>& lhs, const Vector<T>& rhs) noexcept {
  auto l_it = lhs.begin();
  auto r_it = rhs.begin();
  while (l_it != lhs.end() && r_it != rhs.end() && *l_it == *r_it) {
    ++l_it;
    ++r_it;
  }
  if (l_it != lhs.end() && r_it != rhs.end()) {
    return *l_it < *r_it;
  }
  return l_it == lhs.end();
}

template <class T>
bool operator>(const Vector<T>& lhs, const Vector<T>& rhs) noexcept {
  auto l_it = lhs.begin();
  auto r_it = rhs.begin();
  while (l_it != lhs.end() && r_it != rhs.end() && *l_it == *r_it) {
    ++l_it;
    ++r_it;
  }
  if (l_it != lhs.end() && r_it != rhs.end()) {
    return *l_it > *r_it;
  }
  return l_it != lhs.end();
}

template <class T>
bool operator<(const Vector<T>& lhs, const Vector<T>& rhs) noexcept {
  auto l_it = lhs.begin();
  auto r_it = rhs.begin();
  while (l_it != lhs.end() && r_it != rhs.end() && *l_it == *r_it) {
    ++l_it;
    ++r_it;
  }
  if (l_it != lhs.end() && r_it != rhs.end()) {
    return *l_it < *r_it;
  }
  return r_it != rhs.end();
}

#endif