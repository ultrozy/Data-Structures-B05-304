#ifndef _GRAPH_CLASS_H_
#define _GRAPH_CLASS_H_

#include <cstddef>
#include <utility>
#include <optional>
#include <vector>
#include <unordered_set>

#include "./_graph_primitives.h"

template <bool dir, class Wei = void>
class Graph {
  std::vector<std::vector<Edge<Wei>>> adj_list_;

  // DEFAULT METHODS
 public:
  Graph() = default;
  Graph(const Graph<dir, Wei>&) = default;
  Graph(Graph<dir, Wei>&&) noexcept = default;
  Graph<dir, Wei>& operator=(const Graph<dir, Wei>&) = default;
  Graph<dir, Wei>& operator=(Graph<dir, Wei>&&) noexcept = default;
  ~Graph() = default;

  Graph(size_t);
  Graph(std::initializer_list<Edge<Wei>>);
  size_t Size() const;
  Graph<dir, Wei> Transposed() const;
  void AddVertex();
  template <class Weight = Wei, EnifWeighted<Wei, Weight> = 0>
  void AddEdge(Vertex, Vertex, Weight);
  template <class Weight = Wei, EnifNoWeight<Wei, Weight> = 0>
  void AddEdge(Vertex, Vertex);

  // BFS
 public:
  std::vector<size_t> BFS(Vertex) const;
  template <class Weight = Wei, EnifWeighted<Wei, Weight> = 0>
  std::vector<size_t> BFS_01(Vertex) const;
  template <class Weight = Wei, EnifWeighted<Wei, Weight> = 0>
  std::vector<size_t> BFS_0k(Vertex, size_t) const;

  // DFS
 public:
  DFStimes DFS() const;
  bool HasCycle() const;
  template <bool directed = dir, EnifDirected<dir, directed> = 0>
  std::pair<bool, std::vector<Vertex>> TopSort(bool do_reverse = true) const;
  std::vector<std::vector<Vertex>> SCC() const;

 private:
  void RecursiveDfsVisit(Vertex, DFStimes&, std::vector<Color>&, size_t&) const;
  bool RecursiveHasCycle(Vertex, std::vector<Color>&, std::vector<Vertex>&) const;
  bool RecursiveTopSort(Vertex, std::vector<Color>&, std::vector<Vertex>&) const;

  // Euler
 public:
  std::optional<std::pair<Vertex, Vertex>> CheckIfSemiEuler() const;
  bool CheckIfEuler() const;
  std::vector<Vertex> EulerPath() const;
  std::vector<Vertex> EulerCycle() const;

  // AP & bridges
 public:
  template <bool directed = dir, EnifUndirected<dir, directed> = 0>
  std::vector<Vertex> ArticulationPoints() const;
  template <bool directed = dir, EnifUndirected<dir, directed> = 0>
  std::vector<Edge<Wei>> Bridges() const;

 private:
  void RecursiveAP(std::optional<Vertex>, Vertex, DFStimes&, std::vector<Color>&, std::vector<Vertex>&, size_t&) const;
  void RecursiveBridges(std::optional<Vertex>, Vertex, DFStimes&, std::vector<Color>&, std::unordered_set<Edge<Wei>>&,
                        size_t&) const;

  // MST
 public:
  template <bool directed = dir, class Weight = Wei, EnifUndirected<dir, directed> = 0, EnifWeighted<Wei, Weight> = 0>
  std::vector<Edge<Weight>> Prim() const;
  template <bool directed = dir, class Weight = Wei, EnifUndirected<dir, directed> = 0, EnifWeighted<Wei, Weight> = 0>
  std::vector<Edge<Weight>> Kruskal() const;
  template <bool directed = dir, class Weight = Wei, EnifUndirected<dir, directed> = 0, EnifWeighted<Wei, Weight> = 0>
  std::vector<Edge<Weight>> Boruvka() const;

  // Distance
 public:
  template <class Weight = Wei, EnifWeighted<Wei, Weight> = 0>
  std::vector<std::optional<Weight>> Dijkstra(Vertex) const;
  template <class Weight = Wei, EnifWeighted<Wei, Weight> = 0>
  std::pair<bool, std::vector<Weight>> BellmanFord(Vertex) const;
  template <class Weight = Wei, EnifWeighted<Wei, Weight> = 0>
  std::optional<std::vector<std::vector<Weight>>> FloydWarshall() const;
  template <class Weight = Wei, EnifWeighted<Wei, Weight> = 0>
  std::optional<std::vector<std::vector<Weight>>> Johnson() const;

  // Flows
  template <bool directed = dir, class Weight = Wei, EnifWeighted<Wei, Weight> = 0>
  Weight FordFulkerson(Vertex, Vertex) const;
  template <bool directed = dir, class Weight = Wei, EnifWeighted<Wei, Weight> = 0>
  Weight EdmondsKarp(Vertex, Vertex) const;
  template <bool directed = dir, class Weight = Wei, EnifWeighted<Wei, Weight> = 0>
  Weight Dinic(Vertex, Vertex) const;

 private:
  bool RecursiveDFSFordFulkerson(Vertex, Vertex, std::vector<std::unordered_map<Vertex, std::pair<Wei, Wei>>>&,
                                 std::vector<Vertex>&, std::vector<bool>&) const;
};

///////////////////
////  DEFAULT  ////
////  METHODS  ////
///////////////////

template <bool dir, class Wei>
Graph<dir, Wei>::Graph(size_t n) : adj_list_(n) {
}

template <bool dir, class Wei>
Graph<dir, Wei>::Graph(std::initializer_list<Edge<Wei>> list_edges) : adj_list_{} {
  Vertex max_vertex = 0;
  for (const auto& edge : list_edges) {
    if (max_vertex < edge.src) {
      max_vertex = edge.src;
    }
    if (max_vertex < edge.dst) {
      max_vertex = edge.dst;
    }
  }
  adj_list_.resize(max_vertex + 1);
  for (const auto& edge : list_edges) {
    adj_list_[edge.src].emplace_back(edge);
    if constexpr (!dir && std::is_same_v<Wei, void>) {
      adj_list_[edge.dst].emplace_back(edge.dst, edge.src);
    }
    if constexpr (!dir && !std::is_same_v<Wei, void>) {
      adj_list_[edge.dst].emplace_back(edge.dst, edge.src, edge.weight);
    }
  }
}

template <bool dir, class Wei>
size_t Graph<dir, Wei>::Size() const {
  return adj_list_.size();
}

template <bool dir, class Wei>
Graph<dir, Wei> Graph<dir, Wei>::Transposed() const {
  Graph<dir, Wei> graph{adj_list_.size()};
  for (Vertex vertex = 0; vertex < adj_list_.size(); ++vertex) {
    for (const auto& edge : adj_list_[vertex]) {
      if constexpr (std::is_same_v<Wei, void>) {
        graph.adj_list_[edge.dst].emplace_back(edge.dst, edge.src);
      } else {
        graph.adj_list_[edge.dst].emplace_back(edge.dst, edge.src, edge.weight);
      }
    }
  }
  return graph;
}

template <bool dir, class Wei>
void Graph<dir, Wei>::AddVertex() {
  adj_list_.emplace_back();
}

template <bool dir, class Wei>
template <class Weight, EnifNoWeight<Wei, Weight>>
void Graph<dir, Wei>::AddEdge(Vertex src, Vertex dst) {
  adj_list_[src].emplace_back(src, dst);
  if constexpr (!dir) {
    adj_list_[dst].emplace_back(dst, src);
  }
}

template <bool dir, class Wei>
template <class Weight, EnifWeighted<Wei, Weight>>
void Graph<dir, Wei>::AddEdge(Vertex src, Vertex dst, Weight weight) {
  adj_list_[src].emplace_back(src, dst, weight);
  if constexpr (!dir) {
    adj_list_[dst].emplace_back(dst, src, weight);
  }
}

#endif
