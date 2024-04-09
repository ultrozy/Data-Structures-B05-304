#ifndef GRAPH_DFS_H_
#define GRAPH_DFS_H_

#include "./_graph_class.h"

template <bool dir, class Wei>
DFStimes Graph<dir, Wei>::DFS() const {
  DFStimes time(adj_list_.size(), std::make_pair<size_t, size_t>(0ul, 0ul));
  std::vector<Color> color(adj_list_.size(), Color::kWhite);
  size_t curr_time = 0ul;
  for (Vertex vertex = 0; vertex < adj_list_.size(); ++vertex) {
    RecursiveDfsVisit(vertex, time, color, curr_time);
  }
  return time;
}

template <bool dir, class Wei>
void Graph<dir, Wei>::RecursiveDfsVisit(Vertex vertex, DFStimes& time, std::vector<Color>& color,
                                        size_t& curr_time) const {
  color[vertex] = Color::kGrey;
  time[vertex].first = ++curr_time;
  for (auto edge : adj_list_[vertex]) {
    if (color[edge.dst] == Color::kWhite) {
      RecursiveDfsVisit(edge.dst, time, color, curr_time);
    }
  }
  color[vertex] = Color::kBlack;
  time[vertex].second = ++curr_time;
}

template <bool dir, class Wei>
bool Graph<dir, Wei>::HasCycle() const {
  std::vector<Color> color(adj_list_.size(), Color::kWhite);
  std::vector<Vertex> parent(adj_list_.size(), static_cast<size_t>(-1));
  for (Vertex vertex = 0; vertex < adj_list_.size(); ++vertex) {
    if (color[vertex] == Color::kWhite && RecursiveHasCycle(vertex, color, parent)) {
      return true;
    }
  }
  return false;
}

template <bool dir, class Wei>
bool Graph<dir, Wei>::RecursiveHasCycle(Vertex vertex, std::vector<Color>& color, std::vector<Vertex>& parent) const {
  color[vertex] = Color::kGrey;
  for (auto edge : adj_list_[vertex]) {
    if (color[edge.dst] == Color::kGrey) {
      if constexpr (dir) {
        return true;
      } else {
        if (parent[vertex] != edge.dst) {
          return true;
        }
      }
    }
    parent[edge.dst] = vertex;
    if (color[edge.dst] == Color::kWhite && RecursiveHasCycle(edge.dst, color, parent)) {
      return true;
    }
  }
  color[vertex] = Color::kBlack;
  return false;
}

template <bool dir, class Wei>
template <bool directed, EnifDirected<dir, directed>>
std::pair<bool, std::vector<Vertex>> Graph<dir, Wei>::TopSort(bool do_reverse) const {
  bool acyclic = true;
  std::vector<Color> color(adj_list_.size(), Color::kWhite);
  std::vector<Vertex> reverse_order{};
  reverse_order.reserve(adj_list_.size());
  for (Vertex vertex = 0; vertex < adj_list_.size(); ++vertex) {
    if (color[vertex] == Color::kWhite) {
      acyclic = RecursiveTopSort(vertex, color, reverse_order) && acyclic;
    }
  }
  if (do_reverse) {
    return {acyclic, {reverse_order.rbegin(), reverse_order.rend()}};
  } else {
    return {acyclic, reverse_order};
  }
}

template <bool dir, class Wei>
bool Graph<dir, Wei>::RecursiveTopSort(Vertex vertex, std::vector<Color>& color,
                                       std::vector<Vertex>& reverse_order) const {
  bool acyclic = true;
  color[vertex] = Color::kGrey;
  for (auto edge : adj_list_[vertex]) {
    if (color[edge.dst] == Color::kWhite) {
      acyclic = RecursiveTopSort(edge.dst, color, reverse_order) && acyclic;
    } else if (color[edge.dst] == Color::kGrey) {
      acyclic = false;
    }
  }
  color[vertex] = Color::kBlack;
  reverse_order.emplace_back(vertex);
  return acyclic;
}

template <bool dir, class Wei>
std::vector<std::vector<Vertex>> Graph<dir, Wei>::SCC() const {
  if constexpr (dir) {
    auto [acyclic, topsort] = TopSort(false);
    auto g_t = Transposed();
    std::vector<std::vector<Vertex>> result;
    std::vector<Color> color(adj_list_.size(), Color::kWhite);
    for (auto it = topsort.rbegin(); it != topsort.rend(); ++it) {
      if (color[*it] == Color::kWhite) {
        result.emplace_back();
        g_t.RecursiveTopSort(*it, color, result.back());
      }
    }
    return result;
  } else {
    // TODO
  }
}

#endif