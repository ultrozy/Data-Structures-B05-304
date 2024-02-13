#ifndef STRING_H_
#define STRING_H_

#include <cstddef>
#include <iostream>
#include <utility>

class String {
  char* ptr_;
  size_t size_;
  size_t capacity_;

 public:
  String();
  String(const char*);

  ~String();
  String(const String&);
  String& operator=(const String&);
  String(String&&) noexcept;
  String& operator=(String&&) noexcept;

  void Print() const;
};

String::String() : ptr_{nullptr}, size_{0}, capacity_{0} {
}
String::String(const char* s) : ptr_{nullptr}, size_{0}, capacity_{0} {
  while (s[size_]) {
    ++size_;
  }
  if (!size_) {
    return;
  }
  ptr_ = new char[capacity_ = size_ + 1];
  for (size_t i = 0; i < size_; ++i) {
    ptr_[i] = s[i];
  }
  ptr_[size_] = '\0';
}

// Copy constructor
String::String(const String& other)    //
    : ptr_{new char[other.size_ + 1]}  //
    , size_{other.size_}               //
    , capacity_{other.size_ + 1} {
  for (size_t i = 0; i < size_; ++i) {
    ptr_[i] = other.ptr_[i];
  }
  ptr_[size_] = '\0';
}

// Copy assignment
String& String::operator=(const String& other) {
  if (this == &other) {
    return *this;
  }
  delete[] ptr_;
  ptr_ = new char[other.capacity_];
  size_ = other.size_;
  capacity_ = other.capacity_;
  for (size_t i = 0; i < size_; ++i) {
    ptr_[i] = other.ptr_[i];
  }
  ptr_[size_] = '\0';
  return *this;
}

// Move constructor
String::String(String&& other) noexcept         //
    : ptr_{std::exchange(other.ptr_, nullptr)}  //
    , size_{std::exchange(other.size_, 0)}      //
    , capacity_{std::exchange(other.capacity_, 0)} {
}

// Move assignment
String& String::operator=(String&& other) noexcept {
  if (this == &other) {
    return *this;
  }
  delete[] ptr_;
  ptr_ = std::exchange(other.ptr_, nullptr);
  size_ = std::exchange(other.size_, 0);
  capacity_ = std::exchange(other.capacity_, 0);
  return *this;
}

String::~String() {
  delete[] ptr_;
}

void String::Print() const {
  std::cout << size_ << ' ' << capacity_ << ' ' << ptr_ << '\n';
}

#endif