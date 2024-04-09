#ifndef GRAPH_MST_H_
#define GRAPH_MST_H_

#include "./_graph_class.h"
#include <limits>
#include <queue>

namespace detail {
template <class Weight>
struct Comparator {
  bool operator()(const Edge<Weight>& first, const Edge<Weight>& second) const noexcept {
    return first.weight > second.weight ? true : first.dst > second.dst;
  }
};
}  // namespace detail

template <bool dir, class Wei>
template <bool directed, class Weight, EnifUndirected<dir, directed>, EnifWeighted<Wei, Weight>>
std::vector<Edge<Weight>> Graph<dir, Wei>::Prim() const {
  std::vector<bool> in_mst(adj_list_.size(), false);
  std::vector<Edge<Weight>> answer;
  answer.reserve(adj_list_.size() - 1);
  size_t mst_size = 0;
  std::priority_queue<Edge<Weight>, std::vector<Edge<Weight>>, detail::Comparator<Weight>> queue;

  for (const auto& edge : adj_list_[0]) {
    queue.emplace(edge.src, edge.dst, edge.weight);
  }

  in_mst[0] = true;

  while (mst_size != adj_list_.size() - 1) {
    Edge<Weight> curr = queue.top();
    queue.pop();
    while (in_mst[curr.dst]) {
      curr = queue.top();
      queue.pop();
    }
    answer.push_back(curr);
    in_mst[curr.dst] = true;
    ++mst_size;

    for (const auto& edge : adj_list_[curr.dst]) {
      queue.emplace(edge.src, edge.dst, edge.weight);
    }
  }

  return answer;
}

#endif
