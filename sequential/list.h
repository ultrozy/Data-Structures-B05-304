#ifndef LIST_H_
#define LIST_H_

#include <cstddef>
#include <utility>
#include <type_traits>
#include <iterator>

template <class T>
class List;

template <class T>
void std::swap(List<T>&, List<T>&);

namespace detail {
template <class T>
struct NodeList {
  NodeList<T>* prev;
  NodeList<T>* next;
  T value;
};

template <class T>
class IteratorList {
 public:
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = typename std::remove_cv_t<T>;
  using pointer = T*;
  using reference = T&;

 private:
  using NodePtr = typename std::conditional_t<std::is_const_v<T>, const NodeList<value_type>*, NodeList<value_type>*>;
  NodePtr ptr_;

 public:
  explicit IteratorList(NodePtr);
  operator IteratorList<const T>() const;
  template <class U, class V>
  friend bool operator==(const IteratorList<U>&, const IteratorList<V>&);
  template <class U, class V>
  friend bool operator!=(const IteratorList<U>&, const IteratorList<V>&);
  reference operator*() const;
  pointer operator->() const;
  IteratorList<T>& operator++();
  IteratorList<T> operator++(int);
  IteratorList<T>& operator--();
  IteratorList<T> operator--(int);

  friend List<value_type>;
};

template <class T>
IteratorList<T>::IteratorList(IteratorList<T>::NodePtr ptr) : ptr_{ptr} {
}
template <class T>
IteratorList<T>::operator IteratorList<const T>() const {
  return IteratorList<const T>(ptr_);
}
template <class U, class V>
bool operator==(const IteratorList<U>& lhs, const IteratorList<V>& rhs) {
  return lhs.ptr_ == rhs.ptr_;
}
template <class U, class V>
bool operator!=(const IteratorList<U>& lhs, const IteratorList<V>& rhs) {
  return lhs.ptr_ != rhs.ptr_;
}
template <class T>
typename IteratorList<T>::reference IteratorList<T>::operator*() const {
  return ptr_->value;
}
template <class T>
typename IteratorList<T>::pointer IteratorList<T>::operator->() const {
  return &ptr_->value;
}
template <class T>
IteratorList<T>& IteratorList<T>::operator++() {
  ptr_ = ptr_->next;
  return *this;
}
template <class T>
IteratorList<T> IteratorList<T>::operator++(int) {
  IteratorList<T> old(ptr_);
  ptr_ = ptr_->next;
  return old;
}
template <class T>
IteratorList<T>& IteratorList<T>::operator--() {
  ptr_ = ptr_->prev;
  return *this;
}
template <class T>
IteratorList<T> IteratorList<T>::operator--(int) {
  IteratorList<T> old(ptr_);
  ptr_ = ptr_->prev;
  return old;
}
}  // namespace detail

/*
  Declaration order of data members is CRUCIAL.
  It must match with NodeList<T> declaration order:
    prev <-> back_
    next <-> front_
*/
template <class T>
class alignas(alignof(detail::NodeList<T>)) List {
  detail::NodeList<T>* back_;
  detail::NodeList<T>* front_;
  size_t size_;

  detail::NodeList<T>* End_();
  const detail::NodeList<T>* End_() const;
  void RecoverPointers_();

 public:
  using Iterator = detail::IteratorList<T>;
  using ConstIterator = detail::IteratorList<const T>;
  using ReverseIterator = std::reverse_iterator<Iterator>;
  using ConstReverseIterator = std::reverse_iterator<ConstIterator>;

  List();
  List(std::initializer_list<T>);
  template <class InputIt>
  List(InputIt, InputIt);

  friend void std::swap<T>(List<T>&, List<T>&);
  List(const List<T>&);
  List(List<T>&&) noexcept;
  List<T>& operator=(const List<T>&);
  List<T>& operator=(List<T>&&) noexcept;
  ~List();

  void Clear();
  size_t Size() const;

  template <class... Args>
  Iterator Emplace(ConstIterator, Args&&...);
  Iterator Insert(ConstIterator, const T&);
  Iterator Insert(ConstIterator, T&&);
  Iterator Erase(ConstIterator);

  template <class... Args>
  void EmplaceFront(Args&&...);
  void PushFront(const T&);
  void PushFront(T&&);
  void PopFront();

  template <class... Args>
  void EmplaceBack(Args&&...);
  void PushBack(const T&);
  void PushBack(T&&);
  void PopBack();

  Iterator begin();
  ConstIterator begin() const;
  ConstIterator cbegin() const;
  Iterator end();
  ConstIterator end() const;
  ConstIterator cend() const;

  ReverseIterator rbegin();
  ConstReverseIterator rbegin() const;
  ConstReverseIterator crbegin() const;
  ReverseIterator rend();
  ConstReverseIterator rend() const;
  ConstReverseIterator crend() const;
};

template <class T>
detail::NodeList<T>* List<T>::End_() {
  return reinterpret_cast<detail::NodeList<T>*>(this);
}
template <class T>
const detail::NodeList<T>* List<T>::End_() const {
  return reinterpret_cast<const detail::NodeList<T>*>(this);
}

///////////////////
////  DEFAULT  ////
////  METHODS  ////
///////////////////

template <class T>
List<T>::List() : back_{End_()}, front_{End_()}, size_{0ul} {
}
template <class T>
List<T>::List(std::initializer_list<T> ilist) : back_{End_()}, front_{End_()}, size_{ilist.size()} {
  if (!size_) {
    return;
  }
  auto it = ilist.begin();
  back_ = front_ = new detail::NodeList<T>{End_(), End_(), *(it++)};
  while (it != ilist.end()) {
    back_ = back_->next = new detail::NodeList<T>{back_, End_(), *(it++)};
  }
}
template <class T>
template <class InputIt>
List<T>::List(InputIt begin, InputIt end) : back_{End_()}, front_{End_()}, size_{0ul} {
  if (begin == end) {
    return;
  }
  back_ = front_ = new detail::NodeList<T>{End_(), End_(), *(begin++)};
  ++size_;
  while (begin != end) {
    back_ = back_->next = new detail::NodeList<T>{back_, End_(), *(begin++)};
    ++size_;
  }
}

/*
  Because there are pointers pointing to (*this) object,
  move constructor and swap have less trivial implementation.
  This additional code is represented in List<T>::RecoverPointers().
*/
template <class T>
void List<T>::RecoverPointers_() {
  if (size_) {
    front_->prev = back_->next = End_();
  } else {
    front_ = back_ = End_();
  }
}
template <class T>
void std::swap(List<T>& x, List<T>& y) {
  std::swap(x.back_, y.back_);
  std::swap(x.front_, y.front_);
  std::swap(x.size_, y.size_);
  x.RecoverPointers_();
  y.RecoverPointers_();
}
template <class T>
List<T>::List(const List<T>& other) : back_{End_()}, front_{End_()}, size_{other.size_} {
  if (!size_) {
    return;
  }
  auto other_curr = other.front_;
  back_ = front_ = new detail::NodeList<T>{End_(), End_(), other_curr->value};
  other_curr = other_curr->next;
  while (other_curr != other.End_()) {
    back_ = back_->next = new detail::NodeList<T>{back_, End_(), other_curr->value};
    other_curr = other_curr->next;
  }
}
template <class T>
List<T>::List(List<T>&& other) noexcept
    : back_{std::exchange(other.back_, other.End_())}
    , front_{std::exchange(other.front_, other.End_())}
    , size_{std::exchange(other.size_, 0ul)} {
  RecoverPointers_();
}
template <class T>
List<T>& List<T>::operator=(const List<T>& other) {
  List<T> temp(other);
  std::swap(*this, temp);
  return *this;
}
template <class T>
List<T>& List<T>::operator=(List<T>&& other) noexcept {
  List<T> temp(std::move(other));
  std::swap(*this, temp);
  return *this;
}
template <class T>
List<T>::~List() {
  Clear();
}

template <class T>
void List<T>::Clear() {
  size_ = 0ul;
  back_ = End_();
  while (front_ != End_()) {
    delete std::exchange(front_, front_->next);
  }
}
template <class T>
size_t List<T>::Size() const {
  return size_;
}

/////////////////////
////  MODIFYING  ////
////   METHODS   ////
/////////////////////

/*
  (*this) acts as a placeholder NodeList<T> for non-existent value.
  But the pointers of (*this) are EXISTENT: (prev == this->back_) and (next == this->front_).
  Therefore, List<T> effectively is a cyclic structure.
  As a result, there is NO NEED to care about (this->front_) and (this->back_) specifically.
  We only need to assign (prev) and (next) pointers correctly.
*/
template <class T>
template <class... Args>
typename List<T>::Iterator List<T>::Emplace(List<T>::ConstIterator pos, Args&&... args) {
  ++size_;
  auto posptr = const_cast<detail::NodeList<T>*>(pos.ptr_);
  auto newptr = new detail::NodeList<T>{posptr->prev, posptr, {std::forward<Args>(args)...}};
  newptr->next->prev = newptr;
  newptr->prev->next = newptr;
  return Iterator(newptr);
}
template <class T>
typename List<T>::Iterator List<T>::Insert(List<T>::ConstIterator pos, const T& value) {
  ++size_;
  auto posptr = const_cast<detail::NodeList<T>*>(pos.ptr_);
  auto newptr = new detail::NodeList<T>{posptr->prev, posptr, value};
  newptr->next->prev = newptr;
  newptr->prev->next = newptr;
  return Iterator(newptr);
}
template <class T>
typename List<T>::Iterator List<T>::Insert(List<T>::ConstIterator pos, T&& value) {
  ++size_;
  auto posptr = const_cast<detail::NodeList<T>*>(pos.ptr_);
  auto newptr = new detail::NodeList<T>{posptr->prev, posptr, std::move(value)};
  newptr->next->prev = newptr;
  newptr->prev->next = newptr;
  return Iterator(newptr);
}
template <class T>
typename List<T>::Iterator List<T>::Erase(List<T>::ConstIterator pos) {
  --size_;
  auto posptr = const_cast<detail::NodeList<T>*>(pos.ptr_);
  posptr->next->prev = posptr->prev;
  posptr->prev->next = posptr->next;
  delete std::exchange(posptr, posptr->next);
  return Iterator(posptr);
}

template <class T>
template <class... Args>
void List<T>::EmplaceFront(Args&&... args) {
  Emplace(ConstIterator(front_), std::forward<Args>(args)...);
}
template <class T>
void List<T>::PushFront(const T& value) {
  Insert(ConstIterator(front_), value);
}
template <class T>
void List<T>::PushFront(T&& value) {
  Insert(ConstIterator(front_), std::move(value));
}
template <class T>
void List<T>::PopFront() {
  Erase(ConstIterator(front_));
}

template <class T>
template <class... Args>
void List<T>::EmplaceBack(Args&&... args) {
  Emplace(ConstIterator(End_()), std::forward<Args>(args)...);
}
template <class T>
void List<T>::PushBack(const T& value) {
  Insert(ConstIterator(End_()), value);
}
template <class T>
void List<T>::PushBack(T&& value) {
  Insert(ConstIterator(End_()), std::move(value));
}
template <class T>
void List<T>::PopBack() {
  Erase(ConstIterator(back_));
}

////////////////////
////  ITERATOR  ////
////  METHODS   ////
////////////////////

template <class T>
typename List<T>::Iterator List<T>::begin() {
  return Iterator(front_);
}
template <class T>
typename List<T>::ConstIterator List<T>::begin() const {
  return ConstIterator(front_);
}
template <class T>
typename List<T>::ConstIterator List<T>::cbegin() const {
  return ConstIterator(front_);
}
template <class T>
typename List<T>::Iterator List<T>::end() {
  return Iterator(End_());
}
template <class T>
typename List<T>::ConstIterator List<T>::end() const {
  return ConstIterator(End_());
}
template <class T>
typename List<T>::ConstIterator List<T>::cend() const {
  return ConstIterator(End_());
}

template <class T>
typename List<T>::ReverseIterator List<T>::rbegin() {
  return ReverseIterator(Iterator(End_()));
}
template <class T>
typename List<T>::ConstReverseIterator List<T>::rbegin() const {
  return ConstReverseIterator(ConstIterator(End_()));
}
template <class T>
typename List<T>::ConstReverseIterator List<T>::crbegin() const {
  return ConstReverseIterator(ConstIterator(End_()));
}
template <class T>
typename List<T>::ReverseIterator List<T>::rend() {
  return ReverseIterator(Iterator(front_));
}
template <class T>
typename List<T>::ConstReverseIterator List<T>::rend() const {
  return ConstReverseIterator(ConstIterator(front_));
}
template <class T>
typename List<T>::ConstReverseIterator List<T>::crend() const {
  return ConstReverseIterator(ConstIterator(front_));
}

#endif
