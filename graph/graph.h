#ifndef GRAPH_H_
#define GRAPH_H_

#include <cstddef>
#include <vector>
#include <utility>

using Vertex = size_t;
using Edge = std::pair<Vertex, Vertex>;

class Graph {
  std::vector<std::vector<Vertex>> adj_;

 public:
  Graph() = default;
  Graph(size_t);
  Graph(const std::vector<Edge>&);
  std::vector<size_t> BFS(Vertex);
};

#endif