#ifndef GRAPH_DISTANCE_H_
#define GRAPH_DISTANCE_H_

#include "./_graph_class.h"
#include <utility>
#include <optional>
#include <queue>
#include <limits>
#include "../heap/fibonacci_heap.h"

template <class Weight>
bool operator<(const std::pair<std::optional<Weight>, Vertex>& lhs,
               const std::pair<std::optional<Weight>, Vertex>& rhs) {
  if (lhs.first.has_value() && rhs.first.has_value()) {
    if (*lhs.first != *rhs.first) {
      return *lhs.first < *rhs.first;
    }
    return lhs.second < rhs.second;
  }
  if (!lhs.first.has_value() && rhs.first.has_value()) {
    return false;
  }
  if (lhs.first.has_value() && !rhs.first.has_value()) {
    return true;
  }
  return lhs.second < rhs.second;
}
template <class Weight>
bool operator==(const std::pair<std::optional<Weight>, Vertex>& lhs,
                const std::pair<std::optional<Weight>, Vertex>& rhs) {
  return !(lhs < rhs) && !(rhs < lhs);
}
template <class Weight>
bool operator!=(const std::pair<std::optional<Weight>, Vertex>& lhs,
                const std::pair<std::optional<Weight>, Vertex>& rhs) {
  return !(lhs == rhs);
}
template <class Weight>
bool operator>=(const std::pair<std::optional<Weight>, Vertex>& lhs,
                const std::pair<std::optional<Weight>, Vertex>& rhs) {
  return !(lhs < rhs);
}
template <class Weight>
bool operator>(const std::pair<std::optional<Weight>, Vertex>& lhs,
               const std::pair<std::optional<Weight>, Vertex>& rhs) {
  return rhs < lhs;
}
template <class Weight>
bool operator<=(const std::pair<std::optional<Weight>, Vertex>& lhs,
                const std::pair<std::optional<Weight>, Vertex>& rhs) {
  return !(rhs < lhs);
}

template <bool dir, class Wei>
template <class Weight, EnifWeighted<Wei, Weight>>
std::vector<std::optional<Weight>> Graph<dir, Wei>::Dijkstra(Vertex start) const {
  std::vector<std::optional<Weight>> answer(adj_list_.size());
  answer[start] = 0;
  using FibHeap = FibonacciHeap<std::pair<std::optional<Weight>, Vertex>>;
  FibHeap heap;
  std::vector<typename FibHeap::Iterator> it_vec;
  it_vec.reserve(adj_list_.size());
  for (Vertex v = 0; v < adj_list_.size(); ++v) {
    it_vec.emplace_back(heap.Emplace(answer[v], v));
  }

  while (heap.Size()) {
    auto [weight, vertex] = heap.ExtractMin();
    it_vec[vertex] = heap.end();
    if (!weight.has_value()) {
      continue;
    }
    for (const auto& edge : adj_list_[vertex]) {
      if (it_vec[edge.dst] == heap.end()) {
        continue;
      }
      Weight new_weight = *weight + edge.weight;
      if (!answer[edge.dst].has_value() || new_weight < *answer[edge.dst]) {
        answer[edge.dst] = new_weight;
        heap.DecreaseKey(it_vec[edge.dst], {new_weight, edge.dst});
      }
    }
  }
  return answer;
}

template <bool dir, class Wei>
template <class Weight, EnifWeighted<Wei, Weight>>
std::pair<bool, std::vector<Weight>> Graph<dir, Wei>::BellmanFord(Vertex start) const {
  constexpr Weight plus_inf = std::numeric_limits<Weight>::max();
  constexpr Weight minus_inf = std::numeric_limits<Weight>::lowest();

  // Dist initialization
  std::vector<Weight> dist;
  if (start == adj_list_.size()) {
    dist.resize(adj_list_.size(), 0);
  } else {
    dist.resize(adj_list_.size(), plus_inf);
    dist[start] = 0;
  }

  // Bellman - Ford
  for (size_t i = 1; i < adj_list_.size(); ++i) {
    for (Vertex vertex = 0; vertex < adj_list_.size(); ++vertex) {
      if (dist[vertex] == plus_inf) {
        continue;
      }
      for (const auto& edge : adj_list_[vertex]) {
        Weight new_dist = dist[vertex] + edge.weight;
        if (dist[edge.dst] == plus_inf || dist[edge.dst] > new_dist) {
          dist[edge.dst] = new_dist;
        }
      }
    }
  }

  // Detect negative vertices
  std::vector<Vertex> negative_vertices;
  for (Vertex vertex = 0; vertex < adj_list_.size(); ++vertex) {
    if (dist[vertex] == plus_inf) {
      continue;
    }
    for (const auto& edge : adj_list_[vertex]) {
      Weight new_dist = dist[vertex] + edge.weight;
      if (dist[edge.dst] > new_dist) {
        negative_vertices.emplace_back(edge.dst);
      }
    }
  }

  // Assign -inf
  std::vector<Vertex> stack;
  for (Vertex neg_start : negative_vertices) {
    stack.emplace_back(neg_start);
    dist[neg_start] = minus_inf;
    while (!stack.empty()) {
      Vertex vertex = stack.back();
      stack.pop_back();
      for (const auto& edge : adj_list_[vertex]) {
        if (dist[edge.dst] != minus_inf) {
          stack.emplace_back(edge.dst);
          dist[edge.dst] = minus_inf;
        }
      }
    }
  }
  return {negative_vertices.empty(), std::move(dist)};
}

template <bool dir, class Wei>
template <class Weight, EnifWeighted<Wei, Weight>>
std::optional<std::vector<std::vector<Weight>>> Graph<dir, Wei>::FloydWarshall() const {
}
template <bool dir, class Wei>
template <class Weight, EnifWeighted<Wei, Weight>>
std::optional<std::vector<std::vector<Weight>>> Graph<dir, Wei>::Johnson() const {
}

#endif
