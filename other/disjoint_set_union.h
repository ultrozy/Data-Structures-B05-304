#ifndef DISJOINT_SET_UNION_H_
#define DISJOINT_SET_UNION_H_

#include <cstddef>
#include <utility>
#include <vector>

class DSU {
 private:
  std::vector<size_t> parent_{};
  std::vector<size_t> rank_{};
  size_t count_{0ul};

 public:
  DSU() = default;
  DSU(size_t);

  size_t Count() noexcept;
  void MakeSet();
  size_t FindSet(size_t) noexcept;
  void Union(size_t, size_t) noexcept;
};

DSU::DSU(size_t n) : parent_(n), rank_(n, 0ul), count_{n} {
  for (size_t i = 0; i < n; ++i) {
    parent_[i] = i;
  }
}

size_t DSU::Count() noexcept {
  return count_;
}

void DSU::MakeSet() {
  parent_.emplace_back(parent_.size());
  rank_.emplace_back(0ul);
  ++count_;
}

size_t DSU::FindSet(size_t x) noexcept {
  size_t root = parent_[x];
  while (root != parent_[root]) {
    root = parent_[root];
  }
  while (x != root) {
    x = std::exchange(parent_[x], root);
  }
  return root;
}

void DSU::Union(size_t x, size_t y) noexcept {
  x = FindSet(x);
  y = FindSet(y);
  if (x == y) {
    return;
  }
  --count_;
  if (rank_[x] < rank_[y]) {
    parent_[x] = y;
  } else if (rank_[x] > rank_[y]) {
    parent_[y] = x;
  } else {
    parent_[x] = y;
    ++rank_[y];
  }
}

#endif