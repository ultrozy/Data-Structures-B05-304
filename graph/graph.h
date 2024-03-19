#ifndef GRAPH_H_
#define GRAPH_H_

#include <cstddef>
#include <vector>
#include <utility>

using Vertex = size_t;
using Edge = std::pair<Vertex, Vertex>;
using Weight = size_t;
enum class Color { kWhite, kGrey, kBlack };

class UndirectedGraph {
  std::vector<std::vector<Vertex>> adj_;

 public:
  UndirectedGraph() = default;
  UndirectedGraph(size_t);
  UndirectedGraph(std::initializer_list<Edge>);
  std::vector<size_t> BFS(Vertex);
  std::vector<std::pair<size_t, size_t>> DfsVisit(Vertex);
  bool HasCycle();

 private:
  void RecursiveDfsVisit(Vertex, std::vector<std::pair<size_t, size_t>>&, std::vector<Color>&, size_t&);
  bool RecursiveHasCycle(Vertex, std::vector<Color>&);
};

class DirectedGraph {
  std::vector<std::vector<Vertex>> adj_;

 public:
  DirectedGraph() = default;
  DirectedGraph(size_t);
  DirectedGraph(std::initializer_list<Edge>);
  std::vector<size_t> BFS(Vertex);
  std::vector<std::pair<size_t, size_t>> DfsVisit(Vertex);
  bool HasCycle();

 private:
  void RecursiveDfsVisit(Vertex, std::vector<std::pair<size_t, size_t>>&, std::vector<Color>&, size_t&);
  bool RecursiveHasCycle(Vertex, std::vector<Color>&);
};

class UndirectedWeightedGraph {
  std::vector<std::vector<std::pair<Vertex, Weight>>> adj_;

 public:
  UndirectedWeightedGraph() = default;
  UndirectedWeightedGraph(size_t);
  UndirectedWeightedGraph(const std::vector<std::pair<Edge, Weight>>&);
  std::vector<size_t> BFS_01(Vertex);
  std::vector<size_t> BFS_0k(Vertex, size_t);
};

class DirectedWeightedGraph {
  std::vector<std::vector<std::pair<Vertex, Weight>>> adj_;

 public:
  DirectedWeightedGraph() = default;
  DirectedWeightedGraph(size_t);
  DirectedWeightedGraph(const std::vector<std::pair<Edge, Weight>>&);
  std::vector<size_t> BFS_01(Vertex);
  std::vector<size_t> BFS_0k(Vertex, size_t);
};

#endif