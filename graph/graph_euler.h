#ifndef GRAPH_EULER_H_
#define GRAPH_EULER_H_

#include "./_graph_class.h"

template <bool dir, class Wei>
std::optional<std::pair<Vertex, Vertex>> Graph<dir, Wei>::CheckIfSemiEuler() const {
  std::optional<Vertex> start{};
  std::optional<Vertex> end{};
  if constexpr (dir) {
    std::vector<size_t> num_in(adj_list_.size(), 0ul);
    for (Vertex vertex = 0; vertex < adj_list_.size(); ++vertex) {
      for (const auto& edge : adj_list_[vertex]) {
        ++num_in[edge.dst];
      }
    }
    for (Vertex vertex = 0; vertex < adj_list_.size(); ++vertex) {
      if (num_in[vertex] == adj_list_[vertex].size()) {
        continue;
      } else if (num_in[vertex] == adj_list_[vertex].size() + 1) {
        if (end.has_value()) {
          return {};
        }
        end = vertex;
      } else if (num_in[vertex] + 1 == adj_list_[vertex].size()) {
        if (start.has_value()) {
          return {};
        }
        start = vertex;
      } else {
        return {};
      }
    }
    if (start.has_value() ^ end.has_value()) {
      return {};
    }
    if (SCC().size() > 1) {
      return {};
    }
    if (start.has_value()) {
      return {{*start, *end}};
    }
    return {{0, 0}};
  }
  // TODO
}

template <bool dir, class Wei>
bool Graph<dir, Wei>::CheckIfEuler() const {
  // TODO
  return false;
}

template <bool dir, class Wei>
std::vector<Vertex> Graph<dir, Wei>::EulerPath() const {
  // TODO
  return {};
}

template <bool dir, class Wei>
std::vector<Vertex> Graph<dir, Wei>::EulerCycle() const {
  // TODO
  return {};
}

#endif