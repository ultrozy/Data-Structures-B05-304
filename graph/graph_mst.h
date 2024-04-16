#ifndef GRAPH_MST_H_
#define GRAPH_MST_H_

#include "./_graph_class.h"
#include <algorithm>
#include <optional>
#include <limits>
#include <queue>
#include "../other/disjoint_set_union.h"

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

template <bool dir, class Wei>
template <bool directed, class Weight, EnifUndirected<dir, directed>, EnifWeighted<Wei, Weight>>
std::vector<Edge<Weight>> Graph<dir, Wei>::Kruskal() const {
  std::vector<Edge<Weight>> sorted_edges;
  std::vector<Edge<Weight>> answer;
  answer.reserve(adj_list_.size() - 1);
  for (const auto& curr_edges : adj_list_) {
    for (const auto& edge : curr_edges) {
      sorted_edges.emplace_back(edge);
    }
  }
  std::sort(sorted_edges.begin(), sorted_edges.end(),
            [](Edge<Weight>& lhs, Edge<Weight>& rhs) { return lhs.weight < rhs.weight; });

  DSU dsu(adj_list_.size());

  for (const auto& edge : sorted_edges) {
    if (dsu.FindSet(edge.src) != dsu.FindSet(edge.dst)) {
      answer.emplace_back(edge);
      dsu.Union(edge.src, edge.dst);
    }
  }

  return answer;
}

template <bool dir, class Wei>
template <bool directed, class Weight, EnifUndirected<dir, directed>, EnifWeighted<Wei, Weight>>
std::vector<Edge<Weight>> Graph<dir, Wei>::Boruvka() const {
  std::vector<Edge<Weight>> answer;
  answer.reserve(adj_list_.size() - 1);
  DSU dsu(adj_list_.size());
  std::vector<std::optional<Edge<Weight>>> components(adj_list_.size());
  while (dsu.Count() > 1) {
    for (const auto& curr_edges : adj_list_) {
      for (const auto& edge : curr_edges) {
        Vertex root_src = dsu.FindSet(edge.src);
        Vertex root_dst = dsu.FindSet(edge.dst);
        if (root_src == root_dst) {
          continue;
        }

        if (!components[root_src].has_value() || edge.weight < components[root_src]->weight) {
          components[root_src] = edge;
        }
        if (!components[root_dst].has_value() || edge.weight < components[root_dst]->weight) {
          components[root_dst] = edge;
        }
      }
    }
    for (auto& edge : components) {
      if (edge.has_value() && dsu.FindSet(edge->src) != dsu.FindSet(edge->dst)) {
        answer.emplace_back(*edge);
        dsu.Union(edge->src, edge->dst);
      }
      edge.reset();
    }
  }

  return answer;
}

#endif
