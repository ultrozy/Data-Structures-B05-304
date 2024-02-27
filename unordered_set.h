#ifndef UNORDERED_SET_H_
#define UNORDERED_SET_H_

#include <cstddef>
#include <vector>
#include <forward_list>

template <class KeyT>
class UnorderedSet {
  double max_load_factor_ = 1.0;
  size_t size_;
  std::vector<std::forward_list<KeyT>> buckets_;

 public:
  bool Find(const KeyT&);
  bool Insert(const KeyT&);
  bool Erase(const KeyT&);
};

template <class KeyT>
bool UnorderedSet<KeyT>::Find(const KeyT& key) {
  size_t index = std::hash<KeyT>{}(key) % buckets_.size();
  for (auto it = buckets_[index].begin(); it != buckets_[index].end(); ++it) {
    if (*it == key) {
      return true;
    }
  }
  return false;
}

template <class KeyT>
bool UnorderedSet<KeyT>::Insert(const KeyT& key) {
  size_t index = std::hash<KeyT>{}(key) % buckets_.size();
  for (auto it = buckets_[index].begin(); it != buckets_[index].end(); ++it) {
    if (*it == key) {
      return false;
    }
  }
  buckets_[index].insert_after(buckets_[index].before_begin(), key);
  ++size_;
  if (static_cast<double>(size_) / buckets_.size() > max_load_factor_) {
    Rehash(buckets_.size() * 2);
  }
  return true;
}

template <class KeyT>
bool UnorderedSet<KeyT>::Erase(const KeyT& key) {
  size_t index = std::hash<KeyT>{}(key) % buckets_.size();
  for (auto it = buckets_[index].begin(), prev = buckets_[index].before_begin();  //
       it != buckets_[index].end(); ++it, ++prev) {
    if (*it == key) {
      buckets_[index].erase_after(prev);
      return true;
    }
  }
  return false;
}

#endif