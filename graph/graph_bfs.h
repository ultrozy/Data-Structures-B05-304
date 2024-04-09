#ifndef GRAPH_BFS_H_
#define GRAPH_BFS_H_

#include <deque>

#include "./_graph_class.h"

template <bool dir, class Wei>
std::vector<size_t> Graph<dir, Wei>::BFS(Vertex vertex) const {
  std::vector<size_t> dist(adj_list_.size(), static_cast<size_t>(-1));
  std::vector<bool> visited(adj_list_.size(), false);

  dist[vertex] = 0;
  visited[vertex] = true;

  std::deque<Vertex> queue{vertex};
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

template <bool dir, class Wei>
template <class Weight, EnifWeighted<Wei, Weight>>
std::vector<size_t> Graph<dir, Wei>::BFS_01(Vertex vertex) const {
  std::vector<size_t> dist(adj_list_.size(), static_cast<size_t>(-1));
  std::vector<bool> visited(adj_list_.size(), false);

  dist[vertex] = 0;
  visited[vertex] = true;

  std::deque<Vertex> queue{vertex};
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

template <bool dir, class Wei>
template <class Weight, EnifWeighted<Wei, Weight>>
std::vector<size_t> Graph<dir, Wei>::BFS_0k(Vertex vertex, size_t k) const {
  std::vector<size_t> dist(adj_list_.size(), static_cast<size_t>(-1));
  std::vector<bool> obrabatyvalas(adj_list_.size(), false);

  dist[vertex] = 0;
  obrabatyvalas[vertex] = true;
  size_t num_on_que = 1;
  size_t qind = 0;

  std::vector<std::deque<Vertex>> ques(k + 1);
  ques[0].emplace_front(vertex);
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

#endif