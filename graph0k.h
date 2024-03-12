#ifndef GRAPH_H_
#define GRAPH_H_

#include <cstddef>
#include <vector>
#include <utility>

using Vertex = size_t;
using Edge = std::pair<Vertex, Vertex>;
using Weight = size_t;

class Graph {
  std::vector<std::vector<std::pair<Vertex, Weight>>> adj_;

 public:
  Graph() = default;
  Graph(size_t);
  Graph(const std::vector<std::pair<Edge, Weight>>&);
  std::vector<size_t> BFS_01(Vertex);
  std::vector<size_t> BFS_0k(Vertex, size_t);
};

#endif