#ifndef FORWARD_LIST_H_
#define FORWARD_LIST_H_

#include <cstddef>
#include <utility>
#include <type_traits>
#include <iterator>

template <class T>
class ForwardList;

namespace detail {
template <class T>
struct NodeForwardList {
  NodeForwardList<T>* next;
  T value;
};

template <class T>
class IteratorForwardList {
 public:
  using iterator_category = std::forward_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = typename std::remove_cv_t<T>;
  using pointer = T*;
  using reference = T&;

 private:
  using NodePtr =
      typename std::conditional_t<std::is_const_v<T>, const NodeForwardList<value_type>*, NodeForwardList<value_type>*>;
  NodePtr ptr_;

 public:
  explicit IteratorForwardList(NodePtr);
  operator IteratorForwardList<const T>() const;
  template <class U, class V>
  friend bool operator==(const IteratorForwardList<U>&, const IteratorForwardList<V>&);
  template <class U, class V>
  friend bool operator!=(const IteratorForwardList<U>&, const IteratorForwardList<V>&);
  reference operator*() const;
  pointer operator->() const;
  IteratorForwardList<T>& operator++();
  IteratorForwardList<T> operator++(int);

  friend ForwardList<value_type>;
};

template <class T>
IteratorForwardList<T>::IteratorForwardList(IteratorForwardList<T>::NodePtr ptr) : ptr_{ptr} {
}
template <class T>
IteratorForwardList<T>::operator IteratorForwardList<const T>() const {
  return IteratorForwardList<const T>(ptr_);
}
template <class U, class V>
bool operator==(const IteratorForwardList<U>& lhs, const IteratorForwardList<V>& rhs) {
  return lhs.ptr_ == rhs.ptr_;
}
template <class U, class V>
bool operator!=(const IteratorForwardList<U>& lhs, const IteratorForwardList<V>& rhs) {
  return lhs.ptr_ != rhs.ptr_;
}
template <class T>
typename IteratorForwardList<T>::reference IteratorForwardList<T>::operator*() const {
  return ptr_->value;
}
template <class T>
typename IteratorForwardList<T>::pointer IteratorForwardList<T>::operator->() const {
  return &ptr_->value;
}
template <class T>
IteratorForwardList<T>& IteratorForwardList<T>::operator++() {
  ptr_ = ptr_->next;
  return *this;
}
template <class T>
IteratorForwardList<T> IteratorForwardList<T>::operator++(int) {
  IteratorForwardList<T> old(ptr_);
  ptr_ = ptr_->next;
  return old;
}
}  // namespace detail

template <class T>
class alignas(alignof(detail::NodeForwardList<T>)) ForwardList {
  detail::NodeForwardList<T>* front_;
  size_t size_;

 public:
  using Iterator = detail::IteratorForwardList<T>;
  using ConstIterator = detail::IteratorForwardList<const T>;

  ForwardList();
  ForwardList(std::initializer_list<T>);
  template <class InputIt>
  ForwardList(InputIt, InputIt);

  template <class U>
  friend void Swap(ForwardList<U>&, ForwardList<U>&);
  ForwardList(const ForwardList<T>&);
  ForwardList(ForwardList<T>&&) noexcept;
  ForwardList<T>& operator=(const ForwardList<T>&);
  ForwardList<T>& operator=(ForwardList<T>&&) noexcept;
  ~ForwardList();

  void Clear();
  size_t Size() const;

  template <class... Args>
  Iterator EmplaceAfter(ConstIterator, Args&&...);
  Iterator InsertAfter(ConstIterator, const T&);
  Iterator InsertAfter(ConstIterator, T&&);
  Iterator EraseAfter(ConstIterator);

  template <class... Args>
  void EmplaceFront(Args&&...);
  void PushFront(const T&);
  void PushFront(T&&);
  void PopFront();

  Iterator before_begin();
  ConstIterator before_begin() const;
  ConstIterator cbefore_begin() const;
  Iterator begin();
  ConstIterator begin() const;
  ConstIterator cbegin() const;
  Iterator end();
  ConstIterator end() const;
  ConstIterator cend() const;
};

///////////////////
////  DEFAULT  ////
////  METHODS  ////
///////////////////

template <class T>
ForwardList<T>::ForwardList() : front_{nullptr}, size_{0ul} {
}
template <class T>
ForwardList<T>::ForwardList(std::initializer_list<T> ilist) : front_{nullptr}, size_{ilist.size()} {
  if (!size_) {
    return;
  }
  auto it = ilist.begin();
  auto curr = front_ = new detail::NodeForwardList<T>{nullptr, *(it++)};
  while (it != ilist.end()) {
    curr = curr->next = new detail::NodeForwardList<T>{nullptr, *(it++)};
  }
}
template <class T>
template <class InputIt>
ForwardList<T>::ForwardList(InputIt begin, InputIt end) : front_{nullptr}, size_{0ul} {
  if (begin == end) {
    return;
  }
  auto curr = front_ = new detail::NodeForwardList<T>{nullptr, *(begin++)};
  ++size_;
  while (begin != end) {
    curr = curr->next = new detail::NodeForwardList<T>{nullptr, *(begin++)};
    ++size_;
  }
}

template <class U>
void Swap(ForwardList<U>& x, ForwardList<U>& y) {
  std::swap(x.front_, y.front_);
  std::swap(x.size_, y.size_);
}
template <class T>
ForwardList<T>::ForwardList(const ForwardList<T>& other) : front_{nullptr}, size_{other.size_} {
  if (!size_) {
    return;
  }
  auto other_curr = other.front_;
  auto curr = front_ = new detail::NodeForwardList<T>{nullptr, other_curr->value};
  other_curr = other_curr->next;
  while (other_curr) {
    curr = curr->next = new detail::NodeForwardList<T>{nullptr, other_curr->value};
    other_curr = other_curr->next;
  }
}
template <class T>
ForwardList<T>::ForwardList(ForwardList<T>&& other) noexcept
    : front_{std::exchange(other.front_, nullptr)}, size_{std::exchange(other.size_, 0ul)} {
}
template <class T>
ForwardList<T>& ForwardList<T>::operator=(const ForwardList<T>& other) {
  ForwardList<T> temp(other);
  Swap(*this, temp);
  return *this;
}
template <class T>
ForwardList<T>& ForwardList<T>::operator=(ForwardList<T>&& other) noexcept {
  ForwardList<T> temp(std::move(other));
  Swap(*this, temp);
  return *this;
}
template <class T>
ForwardList<T>::~ForwardList() {
  Clear();
}

template <class T>
void ForwardList<T>::Clear() {
  size_ = 0ul;
  while (front_) {
    delete std::exchange(front_, front_->next);
  }
}
template <class T>
size_t ForwardList<T>::Size() const {
  return size_;
}

/////////////////////
////  MODIFYING  ////
////   METHODS   ////
/////////////////////

/*
  In case of these calls:
    - EmplaceAfter(before_begin, ...);
    - InsertAfter(before_begin, ...);
    - EraseAfter(before_begin);
  there is no need to explicitly update (this->front_).
*/
template <class T>
template <class... Args>
typename ForwardList<T>::Iterator ForwardList<T>::EmplaceAfter(ForwardList<T>::ConstIterator pos, Args&&... args) {
  ++size_;
  auto posptr = const_cast<detail::NodeForwardList<T>*>(pos.ptr_);
  posptr->next = new detail::NodeForwardList<T>{posptr->next, {std::forward<Args>(args)...}};
  return ForwardList<T>::Iterator(posptr->next);
}
template <class T>
typename ForwardList<T>::Iterator ForwardList<T>::InsertAfter(ForwardList<T>::ConstIterator pos, const T& value) {
  ++size_;
  auto posptr = const_cast<detail::NodeForwardList<T>*>(pos.ptr_);
  posptr->next = new detail::NodeForwardList<T>{posptr->next, value};
  return ForwardList<T>::Iterator(posptr->next);
}
template <class T>
typename ForwardList<T>::Iterator ForwardList<T>::InsertAfter(ForwardList<T>::ConstIterator pos, T&& value) {
  ++size_;
  auto posptr = const_cast<detail::NodeForwardList<T>*>(pos.ptr_);
  posptr->next = new detail::NodeForwardList<T>{posptr->next, std::move(value)};
  return ForwardList<T>::Iterator(posptr->next);
}
template <class T>
typename ForwardList<T>::Iterator ForwardList<T>::EraseAfter(ForwardList<T>::ConstIterator pos) {
  --size_;
  auto posptr = const_cast<detail::NodeForwardList<T>*>(pos.ptr_);
  delete std::exchange(posptr->next, posptr->next->next);
  return Iterator(posptr->next);
}

template <class T>
template <class... Args>
void ForwardList<T>::EmplaceFront(Args&&... args) {
  ++size_;
  front_ = new detail::NodeForwardList<T>{front_, {std::forward<Args>(args)...}};
}
template <class T>
void ForwardList<T>::PushFront(const T& value) {
  ++size_;
  front_ = new detail::NodeForwardList<T>{front_, value};
}
template <class T>
void ForwardList<T>::PushFront(T&& value) {
  ++size_;
  front_ = new detail::NodeForwardList<T>{front_, std::move(value)};
}
template <class T>
void ForwardList<T>::PopFront() {
  --size_;
  delete std::exchange(front_, front_->next);
}

////////////////////
////  ITERATOR  ////
////  METHODS   ////
////////////////////

template <class T>
typename ForwardList<T>::Iterator ForwardList<T>::before_begin() {
  return Iterator(reinterpret_cast<detail::NodeForwardList<T>*>(this));
}
template <class T>
typename ForwardList<T>::ConstIterator ForwardList<T>::before_begin() const {
  return ConstIterator(reinterpret_cast<const detail::NodeForwardList<T>*>(this));
}
template <class T>
typename ForwardList<T>::ConstIterator ForwardList<T>::cbefore_begin() const {
  return ConstIterator(reinterpret_cast<const detail::NodeForwardList<T>*>(this));
}
template <class T>
typename ForwardList<T>::Iterator ForwardList<T>::begin() {
  return Iterator(front_);
}
template <class T>
typename ForwardList<T>::ConstIterator ForwardList<T>::begin() const {
  return ConstIterator(front_);
}
template <class T>
typename ForwardList<T>::ConstIterator ForwardList<T>::cbegin() const {
  return ConstIterator(front_);
}
template <class T>
typename ForwardList<T>::Iterator ForwardList<T>::end() {
  return Iterator(nullptr);
}
template <class T>
typename ForwardList<T>::ConstIterator ForwardList<T>::end() const {
  return ConstIterator(nullptr);
}
template <class T>
typename ForwardList<T>::ConstIterator ForwardList<T>::cend() const {
  return ConstIterator(nullptr);
}

#endif
