#ifndef GRAPH_H_
#define GRAPH_H_

#include <cstddef>
#include <utility>
#include <type_traits>
#include <optional>
#include <vector>
#include <deque>

using Vertex = size_t;
template <class Weight = void>
struct Edge {
  Vertex src;
  Vertex dst;
  Weight weight;

  Edge(Vertex src, Vertex dst, Weight weight) : src(src), dst(dst), weight(weight) {
  }
};
template <>
struct Edge<void> {
  Vertex src;
  Vertex dst;

  Edge(Vertex src, Vertex dst) : src(src), dst(dst) {
  }
};

enum class Color { kWhite, kGrey, kBlack };
using DFStimes = std::vector<std::pair<size_t, size_t>>;

//////////////////////
////    CLASS     ////
////  DEFINITION  ////
//////////////////////

template <bool directed, class Weight = void>
class Graph {
  std::vector<std::vector<Edge<Weight>>> adj_list_;

 public:
  Graph() = default;
  Graph(size_t);
  Graph(std::initializer_list<Edge<Weight>>);
  Graph<directed, Weight> Transposed() const;

  // BFS

  std::vector<size_t> BFS(Vertex) const;

  template <class W = Weight, std::enable_if_t<std::is_same_v<Weight, W> && !std::is_same_v<W, void>, bool> = true>
  std::vector<size_t> BFS_01(Vertex) const;

  template <class W = Weight, std::enable_if_t<std::is_same_v<Weight, W> && !std::is_same_v<W, void>, bool> = true>
  std::vector<size_t> BFS_0k(Vertex, size_t) const;

  // DFS

  DFStimes DFS() const;
  bool HasCycle() const;

  template <bool d = directed, std::enable_if_t<d, bool> = true>
  std::pair<bool, std::vector<Vertex>> TopSort(bool do_reverse = true) const;
  std::vector<std::vector<Vertex>> SCC() const;

  std::optional<std::pair<Vertex, Vertex>> CheckIfSemiEuler() const;
  bool CheckIfEuler() const;
  std::vector<Vertex> EulerPath() const;
  std::vector<Vertex> EulerCycle() const;

 private:
  void RecursiveDfsVisit(Vertex, DFStimes&, std::vector<Color>&, size_t&) const;
  bool RecursiveHasCycle(Vertex, std::vector<Color>&, std::vector<Vertex>&) const;
  template <bool d = directed, std::enable_if_t<d, bool> = true>
  bool RecursiveTopSort(Vertex, std::vector<Color>&, std::vector<Vertex>&) const;
};

//////////////////////
////    METHOD    ////
////  DEFINITION  ////
//////////////////////

template <bool directed, class Weight>
Graph<directed, Weight>::Graph(size_t n) : adj_list_(n) {
}
template <bool directed, class Weight>
Graph<directed, Weight>::Graph(std::initializer_list<Edge<Weight>> edges) : adj_list_{} {
  Vertex max_vertex = 0;
  for (const auto& rebro : edges) {
    if (max_vertex < rebro.src) {
      max_vertex = rebro.src;
    }
    if (max_vertex < rebro.dst) {
      max_vertex = rebro.dst;
    }
  }
  adj_list_.resize(max_vertex + 1);
  for (const auto& rebro : edges) {
    adj_list_[rebro.src].emplace_back(rebro);
    if constexpr (!directed && std::is_same_v<Weight, void>) {
      adj_list_[rebro.dst].emplace_back(rebro.dst, rebro.src);
    }
    if constexpr (!directed && !std::is_same_v<Weight, void>) {
      adj_list_[rebro.dst].emplace_back(rebro.dst, rebro.src, rebro.weight);
    }
  }
}

///////////////
////  BFS  ////
///////////////

template <bool directed, class Weight>
std::vector<size_t> Graph<directed, Weight>::BFS(Vertex start) const {
  std::vector<size_t> dist(adj_list_.size(), static_cast<size_t>(-1));
  std::vector<bool> visited(adj_list_.size(), false);

  dist[start] = 0;
  visited[start] = true;

  std::deque<Vertex> queue{start};
  while (!queue.empty()) {
    auto curr = queue.front();
    queue.pop_front();
    for (auto edge : adj_list_[curr]) {
      if (visited[edge.dst]) {
        continue;
      }
      visited[edge.dst] = true;
      queue.emplace_back(edge.dst);
      dist[edge.dst] = dist[curr] + 1;
    }
  }
  return dist;
}

template <bool directed, class Weight>
template <class W, std::enable_if_t<std::is_same_v<Weight, W> && !std::is_same_v<W, void>, bool>>
std::vector<size_t> Graph<directed, Weight>::BFS_01(Vertex start) const {
  std::vector<size_t> dist(adj_list_.size(), static_cast<size_t>(-1));
  std::vector<bool> visited(adj_list_.size(), false);

  dist[start] = 0;
  visited[start] = true;

  std::deque<Vertex> queue{start};
  while (!queue.empty()) {
    auto curr = queue.front();
    queue.pop_front();
    for (auto edge : adj_list_[curr]) {
      if (!visited[edge.dst]) {
        if (edge.weight) {
          queue.emplace_back(edge.dst);
        } else {
          queue.emplace_front(edge.dst);
        }
      }
      if (dist[edge.dst] > dist[curr] + edge.weight) {
        dist[edge.dst] = dist[curr] + edge.weight;
      }
      visited[edge.dst] = true;
    }
  }
  return dist;
}

template <bool directed, class Weight>
template <class W, std::enable_if_t<std::is_same_v<Weight, W> && !std::is_same_v<W, void>, bool>>
std::vector<size_t> Graph<directed, Weight>::BFS_0k(Vertex start, size_t k) const {
  std::vector<size_t> dist(adj_list_.size(), static_cast<size_t>(-1));
  std::vector<bool> obrabatyvalas(adj_list_.size(), false);

  dist[start] = 0;
  obrabatyvalas[start] = true;
  size_t num_on_que = 1;
  size_t qind = 0;

  std::vector<std::deque<Vertex>> ques(k + 1);
  ques[0].emplace_front(start);
  while (num_on_que) {
    if (ques[qind].empty()) {
      qind = (qind + 1) % (k + 1);
      continue;
    }
    Vertex curr = ques[qind].front();
    ques[qind].pop_front();
    for (auto edge : adj_list_[curr]) {
      if (!obrabatyvalas[edge.dst]) {
        ques[(qind + edge.weight) % (k + 1)].emplace_back();
      }
      if (dist[edge.dst] > dist[curr] + edge.weight) {
        dist[edge.dst] = dist[curr] + edge.weight;
      }
      obrabatyvalas[edge.dst] = true;
    }
  }
  return dist;
}

///////////////
////  DFS  ////
///////////////

template <bool directed, class Weight>
DFStimes Graph<directed, Weight>::DFS() const {
  DFStimes time(adj_list_.size(), std::make_pair<size_t, size_t>(0ul, 0ul));
  std::vector<Color> color(adj_list_.size(), Color::kWhite);
  size_t curr_time = 0ul;
  for (Vertex vertex = 0; vertex < adj_list_.size(); ++vertex) {
    RecursiveDfsVisit(vertex, time, color, curr_time);
  }
  return time;
}

template <bool directed, class Weight>
void Graph<directed, Weight>::RecursiveDfsVisit(Vertex vertex, DFStimes& time, std::vector<Color>& color,
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

template <bool directed, class Weight>
bool Graph<directed, Weight>::HasCycle() const {
  std::vector<Color> color(adj_list_.size(), Color::kWhite);
  std::vector<Vertex> parent(adj_list_.size(), static_cast<size_t>(-1));
  for (Vertex vertex = 0; vertex < adj_list_.size(); ++vertex) {
    if (color[vertex] == Color::kWhite && RecursiveHasCycle(vertex, color, parent)) {
      return true;
    }
  }
  return false;
}

template <bool directed, class Weight>
bool Graph<directed, Weight>::RecursiveHasCycle(Vertex vertex, std::vector<Color>& color,
                                                std::vector<Vertex>& parent) const {
  color[vertex] = Color::kGrey;
  for (auto edge : adj_list_[vertex]) {
    if (color[edge.dst] == Color::kGrey) {
      if constexpr (directed) {
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

template <bool directed, class Weight>
template <bool d, std::enable_if_t<d, bool>>
std::pair<bool, std::vector<Vertex>> Graph<directed, Weight>::TopSort(bool do_reverse) const {
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

template <bool directed, class Weight>
template <bool d, std::enable_if_t<d, bool>>
bool Graph<directed, Weight>::RecursiveTopSort(Vertex vertex, std::vector<Color>& color,
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

template <bool directed, class Weight>
Graph<directed, Weight> Graph<directed, Weight>::Transposed() const {
  Graph<directed, Weight> graph{adj_list_.size()};
  for (Vertex vertex = 0; vertex < adj_list_.size(); ++vertex) {
    for (const auto& edge : adj_list_[vertex]) {
      if constexpr (std::is_same_v<Weight, void>) {
        graph.adj_list_[edge.dst].emplace_back(edge.dst, edge.src);
      } else {
        graph.adj_list_[edge.dst].emplace_back(edge.dst, edge.src, edge.weight);
      }
    }
  }
  return graph;
}

template <bool directed, class Weight>
std::vector<std::vector<Vertex>> Graph<directed, Weight>::SCC() const {
  if constexpr (directed) {
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

template <bool directed, class Weight>
std::optional<std::pair<Vertex, Vertex>> Graph<directed, Weight>::CheckIfSemiEuler() const {
  std::optional<Vertex> start{};
  std::optional<Vertex> end{};
  if constexpr (directed) {
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
}

template <bool directed, class Weight>
bool Graph<directed, Weight>::CheckIfEuler() const {
  // TODO
  return false;
}

template <bool directed, class Weight>
std::vector<Vertex> Graph<directed, Weight>::EulerPath() const {
  // TODO
  return {};
}

template <bool directed, class Weight>
std::vector<Vertex> Graph<directed, Weight>::EulerCycle() const {
  // TODO
  return {};
}

#endif
