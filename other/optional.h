#ifndef OPTIONAL_H_
#define OPTIONAL_H_

#include <stdexcept>

class BadOptionalAccess : std::exception {};

template <class T>
class Optional {
  bool has_value_;
  std::byte memory_[sizeof(T)];

 public:
  Optional();
  Optional(const T&);
  Optional(T&&);
  template <class... Args>
  Optional(Args&&...);

  T& operator*();
  const T& operator*() const;
  T* operator->();
  const T* operator->() const;
  T& Value();
  const T& Value() const;

  void Reset();

  Optional(const Optional<T>&);
  Optional(Optional<T>&&);
  Optional<T>& operator=(const Optional<T>&);
  Optional<T>& operator=(Optional<T>&&);
  ~Optional();
};

template <class T>
Optional<T>::Optional() : has_value_{false} {
}

template <class T>
Optional<T>::Optional(const T& value) : has_value_{true} {
  new (memory_) T(value);
}

template <class T>
Optional<T>::Optional(T&& value) : has_value_{true} {
  new (memory_) T(std::move(value));
}

template <class T>
template <class... Args>
Optional<T>::Optional(Args&&... args) : has_value_{true} {
  new (memory_) T(std::forward<Args>(args)...);
}

template <class T>
T& Optional<T>::operator*() {
  return *reinterpret_cast<T*>(memory_);
}

template <class T>
const T& Optional<T>::operator*() const {
  return *reinterpret_cast<const T*>(memory_);
}

template <class T>
T* Optional<T>::operator->() {
  return reinterpret_cast<T*>(memory_);
}

template <class T>
const T* Optional<T>::operator->() const {
  return reinterpret_cast<const T*>(memory_);
}

template <class T>
T& Optional<T>::Value() {
  if (!has_value_) {
    throw BadOptionalAccess{};
  }
  return *reinterpret_cast<T*>(memory_);
}

template <class T>
const T& Optional<T>::Value() const {
  if (!has_value_) {
    throw BadOptionalAccess{};
  }
  return *reinterpret_cast<const T*>(memory_);
}

template <class T>
void Optional<T>::Reset() {
  if (has_value_) {
    operator*().~T();
    has_value_ = false;
  }
}

template <class T>
Optional<T>::Optional(const Optional<T>& other) : has_value_{other.has_value_} {
  if (has_value_) {
    new (memory_) T(*other);
  }
}

template <class T>
Optional<T>::Optional(Optional<T>&& other) : has_value_{other.has_value_} {
  if (has_value_) {
    new (memory_) T(std::move(*other));
  }
}

template <class T>
Optional<T>& Optional<T>::operator=(const Optional<T>& other) {
  Reset();
  has_value_ = other.has_value_;
  if (has_value_) {
    new (memory_) T(*other);
  }
}

template <class T>
Optional<T>& Optional<T>::operator=(Optional<T>&& other) {
  Reset();
  has_value_ = other.has_value_;
  if (has_value_) {
    new (memory_) T(std::move(*other));
  }
}

template <class T>
Optional<T>::~Optional() {
  Reset();
}

#endif