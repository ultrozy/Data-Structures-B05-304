#include <deque>
#include "graph.h"

Graph::Graph(size_t n) : adj_(n) {
}
Graph::Graph(const std::vector<Edge>& edges) : adj_{} {
  Vertex max_vertex = 0;
  for (const auto& rebro : edges) {
    if (max_vertex < rebro.first) {
      max_vertex = rebro.first;
    }
    if (max_vertex < rebro.second) {
      max_vertex = rebro.second;
    }
  }
  adj_.resize(max_vertex + 1);
  for (const auto& rebro : edges) {
    adj_[rebro.first].emplace_back(rebro.second);
    // If undirected
    // adj_[rebro.second].emplace_back(rebro.first);
  }
}

std::vector<size_t> Graph::BFS(Vertex start) {
  std::vector<size_t> dist(adj_.size(), static_cast<size_t>(-1));
  std::vector<bool> visited(adj_.size(), false);

  dist[start] = 0;
  visited[start] = true;

  std::deque<Vertex> queue{start};
  while (!queue.empty()) {
    auto curr = queue.front();
    queue.pop_front();
    for (auto neigh : adj_[curr]) {
      if (visited[neigh]) {
        continue;
      }
      visited[neigh] = true;
      queue.emplace_back(neigh);
      dist[neigh] = dist[curr] + 1;
    }
  }
  return dist;
}