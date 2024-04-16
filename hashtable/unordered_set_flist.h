#ifndef UNORDERED_SET_2_H_
#define UNORDERED_SET_2_H_

#define ITERATOR_IMPLEMENTED
#define FORWARD_LIST_IMPLEMENTED

#include <vector>
#include <forward_list>
#include <functional>
#include <utility>

template <class KeyT>
class UnorderedSet {
 private:
  size_t size_{0};
  std::forward_list<KeyT> data_{};
  std::forward_list<size_t> hash_{};
  std::vector<typename std::forward_list<KeyT>::iterator> buckets_data_{};
  std::vector<typename std::forward_list<size_t>::iterator> buckets_hash_{};

 public:
  using Iterator = typename std::forward_list<KeyT>::const_iterator;
  using ConstIterator = typename std::forward_list<KeyT>::const_iterator;
  using DifferenceType = typename std::forward_list<KeyT>::difference_type;

  Iterator begin() const noexcept {  // NOLINT
    return data_.cbegin();
  }
  ConstIterator cbegin() const noexcept {  // NOLINT
    return data_.cbegin();
  }
  Iterator end() const noexcept {  // NOLINT
    return data_.cend();
  }
  ConstIterator cend() const noexcept {  // NOLINT
    return data_.cend();
  }

  UnorderedSet() = default;

  explicit UnorderedSet(size_t count) : buckets_data_(count, data_.end()), buckets_hash_(count, hash_.end()) {
  }
  template <class ForwardIt>
  UnorderedSet(ForwardIt begin, ForwardIt end)
      : size_(std::distance(begin, end)), buckets_data_(size_, data_.end()), buckets_hash_(size_, hash_.end()) {
    while (begin != end) {
      size_t hash = std::hash<KeyT>{}(*begin);
      size_t id = hash % buckets_data_.size();
      if (FindInBucket(id, *begin)) {
        --size_;
      } else {
        InsertIntoBucket(id, *begin, hash);
      }
      ++begin;
    }
  }
  UnorderedSet(std::initializer_list<KeyT> init)
      : size_(init.size()), buckets_data_(size_, data_.end()), buckets_hash_(size_, hash_.end()) {
    for (const auto& key : init) {
      size_t hash = std::hash<KeyT>{}(key);
      size_t id = hash % buckets_data_.size();
      if (FindInBucket(id, key)) {
        --size_;
      } else {
        InsertIntoBucket(id, key, hash);
      }
    }
  }
  UnorderedSet(const UnorderedSet& other)
      : size_(other.size_)
      , data_(other.data_)
      , hash_(other.hash_)
      , buckets_data_(other.BucketCount(), data_.end())
      , buckets_hash_(other.BucketCount(), hash_.end()) {
    auto it_d_before = data_.before_begin(), it_d = data_.begin();
    auto it_h_before = hash_.before_begin(), it_h = hash_.begin();
    size_t curr_id = BucketCount();
    while (it_d != data_.end()) {
      if (curr_id != *it_h % BucketCount()) {
        curr_id = *it_h % BucketCount();
        buckets_data_[curr_id] = it_d_before;
        buckets_hash_[curr_id] = it_h_before;
      }
      it_d_before = it_d++;
      it_h_before = it_h++;
    }
  };
  UnorderedSet(UnorderedSet&& other) noexcept
      : size_(std::exchange(other.size_, 0))
      , data_(std::move(other.data_))
      , hash_(std::move(other.hash_))
      , buckets_data_(std::move(other.buckets_data_))
      , buckets_hash_(std::move(other.buckets_hash_)) {
    if (size_) {
      buckets_data_[*hash_.begin() % BucketCount()] = data_.before_begin();
      buckets_hash_[*hash_.begin() % BucketCount()] = hash_.before_begin();
    }
  }
  UnorderedSet& operator=(const UnorderedSet& other) {
    size_ = other.size_;
    data_ = other.data_;
    hash_ = other.hash_;

    size_t buckets_size = (BucketCount() < size_ ? other.BucketCount() : BucketCount());
    buckets_data_.clear();
    for (size_t i = 0; i < buckets_size; ++i) {
      buckets_data_.emplace_back(data_.end());
    }
    buckets_hash_.clear();
    for (size_t i = 0; i < buckets_size; ++i) {
      buckets_hash_.emplace_back(hash_.end());
    }

    auto it_d_before = data_.before_begin(), it_d = data_.begin();
    auto it_h_before = hash_.before_begin(), it_h = hash_.begin();
    size_t curr_id = BucketCount();
    while (it_d != data_.end()) {
      if (curr_id != *it_h % buckets_size) {
        curr_id = *it_h % buckets_size;
        buckets_data_[curr_id] = it_d_before;
        buckets_hash_[curr_id] = it_h_before;
      }
      it_d_before = it_d++;
      it_h_before = it_h++;
    }
    return *this;
  };
  UnorderedSet& operator=(UnorderedSet&& other) noexcept {
    size_ = std::exchange(other.size_, 0);
    data_ = std::move(other.data_);
    hash_ = std::move(other.hash_);
    buckets_data_ = std::move(other.buckets_data_);
    buckets_hash_ = std::move(other.buckets_hash_);
    if (size_) {
      buckets_data_[*hash_.begin() % BucketCount()] = data_.before_begin();
      buckets_hash_[*hash_.begin() % BucketCount()] = hash_.before_begin();
    }
    return *this;
  }
  ~UnorderedSet() = default;

  void Clear() {
    size_ = 0;
    data_.clear();
    hash_.clear();
    for (auto& bucket : buckets_data_) {
      bucket = data_.end();
    }
    for (auto& bucket : buckets_hash_) {
      bucket = hash_.end();
    }
  }
  void Rehash(size_t new_bucket_count) {
    if (new_bucket_count != BucketCount() && size_ <= new_bucket_count) {
      UnconditionalRehash(new_bucket_count);
    }
  }
  void Reserve(size_t new_bucket_count) {
    if (new_bucket_count > BucketCount()) {
      UnconditionalRehash(new_bucket_count);
    }
  }

  bool Find(const KeyT& key) const {
    return size_ ? FindInBucket(std::hash<KeyT>{}(key) % BucketCount(), key) : false;
  }
  void Insert(const KeyT& key) {
    if (Find(key)) {
      return;
    }
    if (size_ == BucketCount()) {
      UnconditionalRehash(size_ ? size_ << 1 : 1);
    }
    ++size_;
    size_t hash = std::hash<KeyT>{}(key);
    size_t id = hash % BucketCount();
    InsertIntoBucket(id, key, hash);
  }
  void Insert(KeyT&& key) {
    if (Find(key)) {
      return;
    }
    if (size_ == BucketCount()) {
      UnconditionalRehash(size_ ? size_ << 1 : 1);
    }
    ++size_;
    size_t hash = std::hash<KeyT>{}(key);
    size_t id = hash % BucketCount();
    InsertIntoBucket(id, std::move(key), hash);
  }
  void Erase(const KeyT& key) {
    if (!size_) {
      return;
    }
    --size_;
    size_t hash = std::hash<KeyT>{}(key);
    size_t id = hash % BucketCount();
    if (buckets_data_[id] == data_.end()) {
      return;
    }
    auto it_d_before = buckets_data_[id], it_d = std::next(it_d_before);
    auto it_h_before = buckets_hash_[id], it_h = std::next(it_h_before);
    while (it_d != data_.end() && *it_h % BucketCount() == id && *it_d != key) {
      it_d_before = it_d++;
      it_h_before = it_h++;
    }
    if (it_d == data_.end() || *it_h % BucketCount() != id) {
      return;
    }

    auto it_h_after = std::next(it_h);
    bool is_last = false;
    if (it_h_after == hash_.end()) {
      is_last = true;
    } else {
      size_t after_id = *it_h_after % BucketCount();
      if (after_id != id) {
        buckets_data_[after_id] = it_d_before;
        buckets_hash_[after_id] = it_h_before;
        is_last = true;
      }
    }

    if (is_last && buckets_data_[id] == it_d_before) {
      buckets_data_[id] = data_.end();
      buckets_hash_[id] = hash_.end();
    }
    data_.erase_after(it_d_before);
    hash_.erase_after(it_h_before);
  }

  size_t Size() const noexcept {
    return size_;
  }
  bool Empty() const noexcept {
    return !size_;
  }
  size_t BucketCount() const {
    return buckets_data_.size();
  }
  size_t BucketSize(size_t id) const {
    if (id >= BucketCount() || buckets_data_[id] == data_.cend()) {
      return 0;
    };
    size_t bucket_size = 0;
    auto it = std::next(buckets_hash_[id]);
    while (it != hash_.cend() && *it % BucketCount() == id) {
      ++bucket_size;
      ++it;
    }
    return bucket_size;
  }
  size_t Bucket(const KeyT& key) const {
    return std::hash<KeyT>{}(key) % BucketCount();
  }
  double LoadFactor() const {
    return size_ ? static_cast<double>(size_) / BucketCount() : 0.0;
  }

 private:
  bool FindInBucket(size_t id, const KeyT& key) const {
    if (buckets_data_[id] == data_.end()) {
      return false;
    }
    auto it_d = std::next(buckets_data_[id]);
    auto it_h = std::next(buckets_hash_[id]);
    while (it_d != data_.end()) {
      if (*it_h % BucketCount() != id) {
        return false;
      }
      if (*it_d == key) {
        return true;
      }
      ++it_d;
      ++it_h;
    }
    return false;
  }
  void InsertIntoBucket(size_t id, const KeyT& key, size_t hash) {
    if (buckets_data_[id] == data_.end()) {
      buckets_data_[id] = data_.before_begin();
      buckets_hash_[id] = hash_.before_begin();
      auto after_new = hash_.begin();
      data_.insert_after(data_.before_begin(), key);
      hash_.insert_after(hash_.before_begin(), hash);
      if (after_new != hash_.end()) {
        buckets_data_[*after_new % buckets_data_.size()] = data_.begin();
        buckets_hash_[*after_new % buckets_hash_.size()] = hash_.begin();
      }
    } else {
      data_.insert_after(buckets_data_[id], key);
      hash_.insert_after(buckets_hash_[id], hash);
    }
  }
  void InsertIntoBucket(size_t id, KeyT&& key, size_t hash) {
    if (buckets_data_[id] == data_.end()) {
      buckets_data_[id] = data_.before_begin();
      buckets_hash_[id] = hash_.before_begin();
      auto after_new = hash_.begin();
      data_.insert_after(data_.before_begin(), std::move(key));
      hash_.insert_after(hash_.before_begin(), hash);
      if (after_new != hash_.end()) {
        buckets_data_[*after_new % buckets_data_.size()] = data_.begin();
        buckets_hash_[*after_new % buckets_hash_.size()] = hash_.begin();
      }
    } else {
      data_.insert_after(buckets_data_[id], std::move(key));
      hash_.insert_after(buckets_hash_[id], hash);
    }
  }
  void UnconditionalRehash(size_t new_bucket_count) {
    std::forward_list<KeyT> old_data{};
    std::forward_list<size_t> old_hash{};
    std::swap(old_data, data_);
    std::swap(old_hash, hash_);

    buckets_data_.clear();
    for (size_t i = 0; i < new_bucket_count; ++i) {
      buckets_data_.emplace_back(data_.end());
    }
    buckets_hash_.clear();
    for (size_t i = 0; i < new_bucket_count; ++i) {
      buckets_hash_.emplace_back(hash_.end());
    }

    while (old_data.begin() != old_data.end()) {
      size_t id = *old_hash.begin() % new_bucket_count;
      if (buckets_data_[id] == data_.end()) {
        buckets_data_[id] = data_.before_begin();
        buckets_hash_[id] = hash_.before_begin();
        auto after_new = hash_.begin();
        data_.splice_after(data_.before_begin(), old_data, old_data.before_begin());
        hash_.splice_after(hash_.before_begin(), old_hash, old_hash.before_begin());
        if (after_new != hash_.end()) {
          buckets_data_[*after_new % buckets_data_.size()] = data_.begin();
          buckets_hash_[*after_new % buckets_hash_.size()] = hash_.begin();
        }
      } else {
        data_.splice_after(buckets_data_[id], old_data, old_data.before_begin());
        hash_.splice_after(buckets_hash_[id], old_hash, old_hash.before_begin());
      }
    }
  }
};

#endif