#ifndef FIBONACCI_HEAP_
#define FIBONACCI_HEAP_

#include <cstddef>
#include <type_traits>
#include <iterator>
#include <utility>
#include <vector>
#include <ostream>

template <class T>
class FibonacciHeap;

template <class T>
void std::swap(FibonacciHeap<T>&, FibonacciHeap<T>&);

template <class T>
std::ostream& operator<<(std::ostream&, const FibonacciHeap<T>&);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////                   C L A S S                    ////////////////
////////////////              D E F I N I T I O N               ////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template <class T>
class FibonacciHeap {
  ////////////////////////////
  ////  NODE  DEFINITION  ////
  ////////////////////////////

 private:
  struct Node_ {
    T value;
    size_t degree;
    Node_* child;
    Node_* left;
    Node_* right;
    Node_* parent;
    bool marked;

    Node_(const T& value, size_t degree, Node_* child, Node_* left, Node_* right, Node_* parent, bool marked)
        : value{value}, degree{degree}, child{child}, left{left}, right{right}, parent{parent}, marked{marked} {
    }
    Node_(T&& value, size_t degree, Node_* child, Node_* left, Node_* right, Node_* parent, bool marked)
        : value{std::move(value)}
        , degree{degree}
        , child{child}
        , left{left}
        , right{right}
        , parent{parent}
        , marked{marked} {
    }
    template <class Head, class... Tail,
              std::enable_if_t<!std::is_same_v<T, std::remove_cv_t<std::remove_reference_t<Head>>>, bool> = true>
    Node_(Head&& head, Tail&&... tail)
        : value(std::forward<Head>(head), std::forward<Tail>(tail)...)
        , degree{0ul}
        , child{nullptr}
        , left{nullptr}
        , right{nullptr}
        , parent{nullptr}
        , marked{false} {
    }
  };

  ////////////////////////////////
  ////  ITERATOR  DEFINITION  ////
  ////////////////////////////////

 private:
  template <class U>
  class MyIterator_ {
   private:
    using InnerPtr_ = typename std::conditional_t<std::is_const_v<U>, const Node_*, Node_*>;
    InnerPtr_ ptr_;

   public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = typename std::remove_cv_t<U>;
    using difference_type = std::ptrdiff_t;
    using pointer = const value_type*;
    using reference = const value_type&;

    MyIterator_(const MyIterator_<U>&) = default;
    MyIterator_(MyIterator_<U>&&) noexcept = default;
    MyIterator_& operator=(const MyIterator_<U>&) = default;
    MyIterator_& operator=(MyIterator_<U>&&) noexcept = default;
    ~MyIterator_() = default;

    MyIterator_();
    explicit MyIterator_(InnerPtr_);
    template <class V, std::enable_if_t<std::is_same_v<U, V> && std::is_const_v<U>, bool> = true>
    explicit MyIterator_(const MyIterator_<std::remove_const_t<U>>&);

    MyIterator_& operator++();
    MyIterator_ operator++(int);
    reference operator*();
    pointer operator->();
    bool operator==(const MyIterator_<U>&) const;
    bool operator!=(const MyIterator_<U>&) const;

    friend class FibonacciHeap<T>;
  };

  ///////////////////////////////////////
  ////  FIBONACCI  HEAP  DEFINITION  ////
  ///////////////////////////////////////

 private:
  Node_* root_;
  Node_* min_;
  size_t size_;

  using DegreePowerType = uint64_t;

  static void SetBrothers(Node_*, Node_*);
  static void SetBrothers(Node_*, Node_*, Node_*);
  void UpdateMinUnderRoot();
  size_t ExtractMinInit();
  void InsertIntoNodeList(Node_**, Node_*);
  void MoveToRootList(Node_*);

 public:
  using Iterator = MyIterator_<T>;
  using ConstIterator = MyIterator_<const T>;

  FibonacciHeap();
  FibonacciHeap(std::initializer_list<T>);
  FibonacciHeap(const FibonacciHeap<T>&);
  FibonacciHeap(FibonacciHeap<T>&&) noexcept;
  FibonacciHeap& operator=(const FibonacciHeap<T>&);
  FibonacciHeap& operator=(FibonacciHeap<T>&&) noexcept;
  ~FibonacciHeap();

  Iterator begin();
  Iterator end();
  ConstIterator begin() const;
  ConstIterator end() const;
  ConstIterator cbegin() const;
  ConstIterator cend() const;

  size_t Size() const;
  const T& GetMin() const;
  Iterator Insert(const T&);
  Iterator Insert(T&&);
  template <class... Args>
  Iterator Emplace(Args&&...);
  T ExtractMin();
  void DecreaseKey(Iterator, const T&);
  void DecreaseKey(Iterator, T&&);

  friend void std::swap<T>(FibonacciHeap<T>&, FibonacciHeap<T>&);
  friend std::ostream& operator<< <T>(std::ostream&, const FibonacciHeap<T>&);
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////                I T E R A T O R                 ////////////////
////////////////                 M E T H O D S                  ////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template <class T>
template <class U>
FibonacciHeap<T>::MyIterator_<U>::MyIterator_() : ptr_{nullptr} {
}
template <class T>
template <class U>
FibonacciHeap<T>::MyIterator_<U>::MyIterator_(FibonacciHeap<T>::MyIterator_<U>::InnerPtr_ ptr) : ptr_{ptr} {
}
template <class T>
template <class U>
template <class V, std::enable_if_t<std::is_same_v<U, V> && std::is_const_v<U>, bool>>
FibonacciHeap<T>::MyIterator_<U>::MyIterator_(const FibonacciHeap<T>::MyIterator_<std::remove_const_t<U>>& other)
    : ptr_{other.ptr_} {
}
template <class T>
template <class U>
auto FibonacciHeap<T>::MyIterator_<U>::operator++() -> FibonacciHeap<T>::MyIterator_<U>& {
  if (ptr_->child) {
    ptr_ = ptr_->child;
    return *this;
  }
  while (ptr_ && !ptr_->right) {
    ptr_ = ptr_->parent;
  }
  if (ptr_) {
    ptr_ = ptr_->right;
  }
  return *this;
}
template <class T>
template <class U>
auto FibonacciHeap<T>::MyIterator_<U>::operator++(int) -> FibonacciHeap<T>::MyIterator_<U> {
  auto old = ptr_;
  if (ptr_->child) {
    ptr_ = ptr_->child;
    return MyIterator_<U>(old);
  }
  while (ptr_ && !ptr_->right) {
    ptr_ = ptr_->parent;
  }
  if (ptr_) {
    ptr_ = ptr_->right;
  }
  return MyIterator_<U>(old);
}
template <class T>
template <class U>
auto FibonacciHeap<T>::MyIterator_<U>::operator*() -> FibonacciHeap<T>::MyIterator_<U>::reference {
  return ptr_->value;
}
template <class T>
template <class U>
auto FibonacciHeap<T>::MyIterator_<U>::operator->() -> FibonacciHeap<T>::MyIterator_<U>::pointer {
  return &ptr_->value;
}
template <class T>
template <class U>
bool FibonacciHeap<T>::MyIterator_<U>::operator==(const FibonacciHeap<T>::MyIterator_<U>& other) const {
  return ptr_ == other.ptr_;
}
template <class T>
template <class U>
bool FibonacciHeap<T>::MyIterator_<U>::operator!=(const FibonacciHeap<T>::MyIterator_<U>& other) const {
  return ptr_ != other.ptr_;
}

////////////////////////////
////  CREATE  ITERATOR  ////
////////////////////////////

template <class T>
auto FibonacciHeap<T>::begin() -> FibonacciHeap<T>::Iterator {
  return Iterator(root_);
}
template <class T>
auto FibonacciHeap<T>::end() -> FibonacciHeap<T>::Iterator {
  return Iterator();
}
template <class T>
auto FibonacciHeap<T>::begin() const -> FibonacciHeap<T>::ConstIterator {
  return ConstIterator(root_);
}
template <class T>
auto FibonacciHeap<T>::end() const -> FibonacciHeap<T>::ConstIterator {
  return ConstIterator();
}
template <class T>
auto FibonacciHeap<T>::cbegin() const -> FibonacciHeap<T>::ConstIterator {
  return ConstIterator(root_);
}
template <class T>
auto FibonacciHeap<T>::cend() const -> FibonacciHeap<T>::ConstIterator {
  return ConstIterator();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////                 D E F A U L T                  ////////////////
////////////////                 M E T H O D S                  ////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template <class T>
void std::swap(FibonacciHeap<T>& lhs, FibonacciHeap<T>& rhs) {
  std::swap(lhs.root_, rhs.root_);
  std::swap(lhs.min_, rhs.min_);
  std::swap(lhs.size_, rhs.size_);
}

template <class T>
FibonacciHeap<T>::FibonacciHeap() : root_{nullptr}, min_{nullptr}, size_{0ul} {
}
template <class T>
FibonacciHeap<T>::FibonacciHeap(std::initializer_list<T> list) : root_{nullptr}, min_{nullptr}, size_{list.size()} {
  for (auto& value : list) {
    root_ = new Node_(value, 0ul, nullptr, nullptr, root_, nullptr, false);
    SetBrothers(root_, root_->right);
    UpdateMinUnderRoot();
  }
}
template <class T>
FibonacciHeap<T>::FibonacciHeap(const FibonacciHeap<T>& other) : root_{nullptr}, min_{nullptr}, size_{other.size_} {
  if (!other.root_) {
    return;
  }
  root_ = new Node_(other.root_->value, other.root_->degree, nullptr, nullptr, nullptr, nullptr, other.root_->marked);
  auto curr = root_;
  auto other_curr = other.root_;
  while (other_curr) {
    if (other_curr->child) {
      other_curr = other_curr->child;
      curr->child = new Node_(other_curr->value, other_curr->degree,  //
                              nullptr, nullptr, nullptr, curr, other_curr->marked);
      curr = curr->child;
      continue;
    }
    while (other_curr && !other_curr->right) {
      other_curr = other_curr->parent;
      curr = curr->parent;
    }
    if (other_curr) {
      other_curr = other_curr->right;
      curr->right = new Node_(other_curr->value, other_curr->degree,  //
                              nullptr, curr, nullptr, curr->parent, other_curr->marked);
      curr = curr->right;
    }
  }
  min_ = root_;
  curr = root_->right;
  while (curr) {
    if (min_->value > curr->value) {
      min_ = curr;
    }
    curr = curr->right;
  }
}
template <class T>
FibonacciHeap<T>::FibonacciHeap(FibonacciHeap<T>&& other) noexcept
    : root_{std::exchange(other.root_, nullptr)}
    , min_{std::exchange(other.min_, nullptr)}
    , size_{std::exchange(other.size_, 0ul)} {
}
template <class T>
FibonacciHeap<T>& FibonacciHeap<T>::operator=(const FibonacciHeap<T>& other) {
  FibonacciHeap<T> temp(other);
  std::swap(*this, temp);
  return *this;
}
template <class T>
FibonacciHeap<T>& FibonacciHeap<T>::operator=(FibonacciHeap<T>&& other) noexcept {
  FibonacciHeap<T> temp(std::move(other));
  std::swap(*this, temp);
  return *this;
}
template <class T>
FibonacciHeap<T>::~FibonacciHeap() {
  while (root_->child) {
    root_ = root_->child;
  }
  while (root_) {
    if (!root_->right) {
      delete std::exchange(root_, root_->parent);
      continue;
    }
    delete std::exchange(root_, root_->right);
    while (root_->child) {
      root_ = root_->child;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////                   O T H E R                    ////////////////
////////////////                 M E T H O D S                  ////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template <class T>
void FibonacciHeap<T>::SetBrothers(FibonacciHeap<T>::Node_* left, FibonacciHeap<T>::Node_* right) {
  if (left) {
    left->right = right;
  }
  if (right) {
    right->left = left;
  }
}

template <class T>
void FibonacciHeap<T>::SetBrothers(FibonacciHeap<T>::Node_* left, FibonacciHeap<T>::Node_* center,
                                   FibonacciHeap<T>::Node_* right) {
  SetBrothers(left, center);
  SetBrothers(center, right);
}

template <class T>
void FibonacciHeap<T>::UpdateMinUnderRoot() {
  if (!min_ || (root_ && min_->value > root_->value)) {
    min_ = root_;
  }
}

template <class T>
size_t FibonacciHeap<T>::ExtractMinInit() {
  --size_;
  if (root_ == min_) {
    root_ = min_->right;
  }
  SetBrothers(min_->left, min_->right);
  delete std::exchange(min_, min_->child);

  DegreePowerType deg_pow{0ull};
  auto curr = root_;
  while (curr) {
    deg_pow += static_cast<DegreePowerType>(1) << curr->degree;
    curr = curr->right;
  }
  curr = min_;
  while (curr) {
    deg_pow += static_cast<DegreePowerType>(1) << curr->degree;
    curr = curr->right;
  }

  size_t num_degrees{0ul};
  while (deg_pow) {
    ++num_degrees;
    deg_pow >>= 1u;
  }
  return num_degrees;
}

template <class T>
void FibonacciHeap<T>::InsertIntoNodeList(Node_** node_list, Node_* new_node) {
  while (node_list[new_node->degree]) {
    auto other = std::exchange(node_list[new_node->degree], nullptr);
    if (new_node->value > other->value) {
      std::swap(new_node, other);
    }
    ++new_node->degree;
    SetBrothers(nullptr, other, new_node->child);
    other->parent = new_node;
    new_node->child = other;
  }
  node_list[new_node->degree] = new_node;
}

template <class T>
void FibonacciHeap<T>::MoveToRootList(FibonacciHeap<T>::Node_* node) {
  node->marked = false;
  if (!node->parent) {
    return;
  }
  if (!node->left) {
    node->parent->child = node->right;
  }
  SetBrothers(node->left, node->right);
  SetBrothers(nullptr, node, root_);
  node->parent = nullptr;
  root_ = node;
}

template <class T>
size_t FibonacciHeap<T>::Size() const {
  return size_;
}

template <class T>
const T& FibonacciHeap<T>::GetMin() const {
  return min_->value;
}

template <class T>
typename FibonacciHeap<T>::Iterator FibonacciHeap<T>::Insert(const T& value) {
  ++size_;
  root_ = new Node_(value, 0ul, nullptr, nullptr, root_, nullptr, false);
  SetBrothers(root_, root_->right);
  UpdateMinUnderRoot();
  return Iterator(root_);
}

template <class T>
typename FibonacciHeap<T>::Iterator FibonacciHeap<T>::Insert(T&& value) {
  ++size_;
  root_ = new Node_(std::move(value), 0ul, nullptr, nullptr, root_, nullptr, false);
  SetBrothers(root_, root_->right);
  UpdateMinUnderRoot();
  return Iterator(root_);
}

template <class T>
template <class... Args>
typename FibonacciHeap<T>::Iterator FibonacciHeap<T>::Emplace(Args&&... args) {
  ++size_;
  auto new_root = new Node_(std::forward<Args>(args)...);
  SetBrothers(new_root, root_);
  root_ = new_root;
  UpdateMinUnderRoot();
  return Iterator(root_);
}

template <class T>
T FibonacciHeap<T>::ExtractMin() {
  T value(std::move(min_->value));
  size_t num_degrees = ExtractMinInit();
  auto node_list = new Node_* [num_degrees] {};

  auto curr = root_;
  while (curr) {
    auto next = curr->right;
    InsertIntoNodeList(node_list, curr);
    curr = next;
  }
  curr = min_;
  while (curr) {
    auto next = curr->right;
    InsertIntoNodeList(node_list, curr);
    curr = next;
  }
  root_ = min_ = nullptr;
  for (size_t index = 0ul; index < num_degrees; ++index) {
    if (!node_list[index]) {
      continue;
    }
    SetBrothers(nullptr, node_list[index], root_);
    root_ = node_list[index];
    root_->parent = nullptr;
    UpdateMinUnderRoot();
  }

  delete[] node_list;
  return value;
}

template <class T>
void FibonacciHeap<T>::DecreaseKey(FibonacciHeap<T>::Iterator it, const T& new_value) {
  if (*it <= new_value) {
    return;
  }
  Node_* curr = it.ptr_;
  curr->value = new_value;
  if (min_->value > curr->value) {
    min_ = curr;
  }
  if (!curr->parent || curr->parent->value <= curr->value) {
    return;
  }

  curr->marked = true;
  while (curr->parent && curr->marked) {
    MoveToRootList(std::exchange(curr, curr->parent));
  }
}

template <class T>
void FibonacciHeap<T>::DecreaseKey(FibonacciHeap<T>::Iterator it, T&& new_value) {
  if (*it <= new_value) {
    return;
  }
  Node_* curr = it.ptr_;
  curr->value = std::move(new_value);
  if (min_->value > curr->value) {
    min_ = curr;
  }
  if (!curr->parent || curr->parent->value <= curr->value) {
    return;
  }

  curr->marked = true;
  while (curr->parent && curr->marked) {
    MoveToRootList(std::exchange(curr, curr->parent));
  }
  curr->marked = true;
}

template <class T>
std::ostream& operator<<(std::ostream& os, const FibonacciHeap<T>& obj) {
  os << "Fibonacci Heap. Size: " << obj.size_ << '\n';
  auto curr = obj.root_;
  size_t indent = 1;
  std::vector<bool> has_right{true};
  while (curr) {
    if (curr != obj.root_) {
      os << "   ";
      for (size_t i = 1; i < indent - 1; ++i) {
        os << (has_right[i] ? " ║  " : "    ");
      }
      os << (indent > 1 ? " ║  \n" : "\n");
    }
    os << (curr == obj.min_ ? "-> " : "   ");
    for (size_t i = 1; i < indent - 1; ++i) {
      os << (has_right[i] ? " ║  " : "    ");
    }
    std::cout << (indent <= 1 ? "" : (curr->right ? " ╠══" : " ╚══"));
    os << (curr->marked ? "░░ " : "██ ") << curr->value << '\n';

    if (curr->child) {
      curr = curr->child;
      has_right.resize(++indent);
      has_right[indent - 1] = curr->right;
      continue;
    }
    while (curr && !curr->right) {
      curr = curr->parent;
      --indent;
    }
    if (curr) {
      curr = curr->right;
      has_right[indent - 1] = curr->right;
    }
  }
  return os;
}

#endif