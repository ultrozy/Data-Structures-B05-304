#ifndef FROZEN_SET_H_
#define FROZEN_SET_H_

#include <functional>
#include <random>
#include <optional>
#include "universal_hash.h"

template <class T>
class FrozenSet {
 private:
  static inline std::mt19937_64 rng{std::random_device{}()};
  std::vector<std::vector<std::optional<T>>> hash_table_;
  UniversalHash primary_hash_;
  std::vector<UniversalHash> secondary_hashes_;

 public:
  FrozenSet() = default;

  template <class ForwardIt>
  FrozenSet(ForwardIt, ForwardIt);

  size_t Size();
  bool Empty();
  void Clear();
  bool Find(const T&);
};

template <class T>
template <class ForwardIt>
FrozenSet<T>::FrozenSet(ForwardIt begin, ForwardIt end)
    : hash_table_(std::distance(begin, end)), primary_hash_{}, secondary_hashes_{} {
  if (!hash_table_.size()) {
    return;
  }
  std::vector<std::vector<T>> objects(hash_table_.size());
  std::hash<T> std_hasher{};
  size_t sum_sqr = 0;
  do {
    primary_hash_ = UniversalHash::GenerateHash(rng);
    for (size_t i = 0; i < objects.size(); ++i) {
      objects[i].clear();
    }
    for (ForwardIt it = begin; it != end; ++it) {
      objects[primary_hash_(std_hasher(*it)) % objects.size()].push_back(*it);
    }
    sum_sqr = 0;
    for (size_t i = 0; i < objects.size(); ++i) {
      sum_sqr += objects[i].size() * objects[i].size();
    }
  } while (sum_sqr >= 4 * objects.size());

  for (size_t i = 0; i < hash_table_.size(); ++i) {
    hash_table_[i] = std::vector<std::optional<T>>(objects[i].size() * objects[i].size());
    bool have_collisions = false;
    do {
      secondary_hashes_[i] = UniversalHash::GenerateHash(rng);
      for (size_t j = 0; j < hash_table_[i].size(); ++j) {
        hash_table_[i][j].reset();
      }
      have_collisions = false;
      for (const auto& obj : objects[i]) {
        size_t j = secondary_hashes_[i](std_hasher(obj)) % hash_table_[i].size();
        if (hash_table_[i][j].has_value()) {
          have_collisions = true;
          break;
        } else {
          hash_table_[i][j] = obj;
        }
      }
    } while (have_collisions);
  }
}

template class FrozenSet<int>;

#endif