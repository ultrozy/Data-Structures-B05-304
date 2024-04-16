#ifndef UNORDERED_SET_1_H_
#define UNORDERED_SET_1_H_

#include <vector>
#include <forward_list>
#include <functional>

template <class KeyT>
class UnorderedSet {
 private:
  size_t size_{0};
  std::vector<std::forward_list<KeyT>> hash_table_{};

 public:
  UnorderedSet() = default;
  explicit UnorderedSet(size_t count) : hash_table_(count) {
  }
  template <class ForwardIt>
  UnorderedSet(ForwardIt begin, ForwardIt end) : size_(std::distance(begin, end)) {
    hash_table_.resize(size_);
    while (begin != end) {
      auto& bucket = hash_table_[std::hash<KeyT>{}(*begin) % hash_table_.size()];
      if (std::find(bucket.begin(), bucket.end(), *begin) == bucket.end()) {
        bucket.push_front(*begin);
      } else {
        --size_;
      }
      ++begin;
    }
  }
  UnorderedSet(std::initializer_list<KeyT> init) : size_(init.size()) {
    hash_table_.resize(size_);
    for (const auto& elem : init) {
      auto& bucket = hash_table_[std::hash<KeyT>{}(elem) % hash_table_.size()];
      if (std::find(bucket.begin(), bucket.end(), elem) == bucket.end()) {
        bucket.push_front(elem);
      } else {
        --size_;
      }
    }
  }

  UnorderedSet(const UnorderedSet& other) = default;
  UnorderedSet(UnorderedSet&& other) : size_(other.size_), hash_table_(std::move(other.hash_table_)) {
    other.size_ = 0;
  }
  UnorderedSet& operator=(const UnorderedSet& other) = default;
  UnorderedSet& operator=(UnorderedSet&& other) {
    size_ = other.size_;
    other.size_ = 0;
    hash_table_ = std::move(other.hash_table_);
    return *this;
  }
  ~UnorderedSet() = default;

  size_t Size() const noexcept {
    return size_;
  }
  bool Empty() const noexcept {
    return !size_;
  }
  void Clear() {
    size_ = 0;
    for (auto& bucket : hash_table_) {
      bucket.clear();
    }
  }

  void Rehash(size_t new_bucket_count) {
    if (new_bucket_count != hash_table_.size() && size_ <= new_bucket_count) {
      UnconditionalRehash(new_bucket_count);
    }
  }
  void Reserve(size_t new_bucket_count) {
    if (new_bucket_count > hash_table_.size()) {
      UnconditionalRehash(new_bucket_count);
    }
  }

  bool Find(const KeyT& key) const {
    if (size_) {
      auto& bucket = hash_table_[std::hash<KeyT>{}(key) % hash_table_.size()];
      return std::find(bucket.begin(), bucket.end(), key) != bucket.end();
    }
    return false;
  }
  void Insert(const KeyT& key) {
    if (Find(key)) {
      return;
    }
    if (size_ == hash_table_.size()) {
      UnconditionalRehash(size_ ? size_ << 1 : 1);
    }
    ++size_;
    hash_table_[std::hash<KeyT>{}(key) % hash_table_.size()].push_front(key);
  }
  void Insert(KeyT&& key) {
    if (Find(key)) {
      return;
    }
    if (size_ == hash_table_.size()) {
      UnconditionalRehash(size_ ? size_ << 1 : 1);
    }
    ++size_;
    hash_table_[std::hash<KeyT>{}(key) % hash_table_.size()].push_front(std::move(key));
  }
  void Erase(const KeyT& key) {
    if (!Find(key)) {
      return;
    }
    --size_;
    hash_table_[std::hash<KeyT>{}(key) % hash_table_.size()].remove(key);
  }

  size_t BucketCount() const {
    return hash_table_.size();
  }
  size_t BucketSize(size_t id) const {
    return id < hash_table_.size() ? std::distance(hash_table_[id].begin(), hash_table_[id].end()) : 0;
  }
  size_t Bucket(const KeyT& key) const {
    return std::hash<KeyT>{}(key) % hash_table_.size();
  }
  double LoadFactor() const {
    return size_ ? static_cast<double>(size_) / hash_table_.size() : 0.0;
  }

 private:
  void UnconditionalRehash(size_t new_bucket_count) {
    auto new_table = std::vector<std::forward_list<KeyT>>(new_bucket_count);
    for (const auto& bucket : hash_table_) {
      for (const auto& elem : bucket) {
        new_table[std::hash<KeyT>{}(elem) % new_bucket_count].push_front(elem);
      }
    }
    std::swap(new_table, hash_table_);
  }
};

#endif