#ifndef STRING_H_
#define STRING_H_

#include <cstddef>
#include <iostream>

class String {
  char* ptr_;
  size_t size_;
  size_t capacity_;

 public:
  String();
  String(const char*);

  String(const String&);
  String& operator=(const String&);
  ~String();

  void Print() const;
};

String::String() : ptr_{new char[1]{}}, size_{0}, capacity_{1} {
}
String::String(const char* s) : ptr_{nullptr}, size_{0}, capacity_{1} {
  while (s[size_]) {
    ++size_;
  }
  if (!size_) {
    ptr_ = new char[1]{};
    return;
  }
  ptr_ = new char[capacity_ = size_ + 1];
  for (size_t i = 0; i < size_; ++i) {
    ptr_[i] = s[i];
  }
  ptr_[size_] = '\0';
}

String::String(const String& other) : ptr_{new char[other.size_ + 1]}, size_{other.size_}, capacity_{other.size_ + 1} {
  for (size_t i = 0; i < size_; ++i) {
    ptr_[i] = other.ptr_[i];
  }
  ptr_[size_] = '\0';
}

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

String::~String() {
  delete[] ptr_;
}

void String::Print() const {
  std::cout << size_ << ' ' << capacity_ << ' ' << ptr_ << '\n';
}

#endif