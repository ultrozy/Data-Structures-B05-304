#include <deque>
#include "graph0k.h"

Graph::Graph(size_t n) : adj_(n) {
}
Graph::Graph(const std::vector<std::pair<Edge, Weight>>& edges) : adj_{} {
  Vertex max_vertex = 0;
  for (const auto& rebro : edges) {
    if (max_vertex < rebro.first.first) {
      max_vertex = rebro.first.first;
    }
    if (max_vertex < rebro.first.second) {
      max_vertex = rebro.first.second;
    }
  }
  adj_.resize(max_vertex + 1);
  for (const auto& rebro : edges) {
    adj_[rebro.first.first].emplace_back(rebro.first.second, rebro.second);
    // If undirected
    // adj_[rebro.first.second].emplace_back(rebro.first.first, rebro.second);
  }
}

std::vector<size_t> Graph::BFS_01(Vertex start) {
  std::vector<size_t> dist(adj_.size(), static_cast<size_t>(-1));
  std::vector<bool> visited(adj_.size(), false);

  dist[start] = 0;
  visited[start] = true;

  std::deque<Vertex> queue{start};
  while (!queue.empty()) {
    auto curr = queue.front();
    queue.pop_front();
    for (auto neigh : adj_[curr]) {
      Vertex vertex = neigh.first;
      Weight weight = neigh.second;
      if (!visited[vertex]) {
        if (weight) {
          queue.emplace_back(vertex);
        } else {
          queue.emplace_front(vertex);
        }
      }
      if (dist[vertex] > dist[curr] + weight) {
        dist[vertex] = dist[curr] + weight;
      }
      visited[vertex] = true;
    }
  }
  return dist;
}

std::vector<size_t> Graph::BFS_0k(Vertex start, size_t k) {
  std::vector<size_t> dist(adj_.size(), static_cast<size_t>(-1));
  std::vector<bool> obrabatyvalas(adj_.size(), false);

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
    for (auto neigh : adj_[curr]) {
      Vertex vertex = neigh.first;
      Weight weight = neigh.second;
      if (!obrabatyvalas[vertex]) {
        ques[(qind + weight) % (k + 1)].emplace_back();
      }
      if (dist[vertex] > dist[curr] + weight) {
        dist[vertex] = dist[curr] + weight;
      }
      obrabatyvalas[vertex] = true;
    }
  }
  return dist;
}