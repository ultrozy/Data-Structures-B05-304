#ifndef GRAPH_FLOWS_H_
#define GRAPH_FLOWS_H_

#include <unordered_map>
#include <vector>
#include <deque>
#include <algorithm>
#include <utility>
#include <limits>
#include <functional>
#include "./_graph_class.h"

template <bool dir, class Wei>
template <bool directed, class Weight, EnifWeighted<Wei, Weight>>
Weight Graph<dir, Wei>::FordFulkerson(Vertex source, Vertex destination) const {
  std::vector<std::unordered_map<Vertex, std::pair<Weight, Weight>>> transport_net(adj_list_.size());
  for (Vertex u = 0; u < adj_list_.size(); ++u) {
    for (const auto& edge : adj_list_[u]) {
      transport_net[u][edge.dst] = {0, edge.weight};
      if (transport_net[edge.dst].find(u) == transport_net[edge.dst].end()) {
        transport_net[edge.dst][u] = {0, 0};
      }
    }
  }
  Weight answer = 0;
  std::vector<Vertex> path;
  std::vector<bool> visited(adj_list_.size(), false);

  while (RecursiveDFSFordFulkerson(source, destination, transport_net, path, visited)) {
    Weight min_capacity = std::numeric_limits<Weight>::max();
    for (size_t i = 1; i < path.size(); ++i) {
      auto [flow, capacity] = transport_net[path[i - 1]][path[i]];
      min_capacity = std::min(min_capacity, capacity - flow);
    }
    for (size_t i = 1; i < path.size(); ++i) {
      Vertex src = path[i - 1];
      Vertex dst = path[i];
      transport_net[src][dst].first += min_capacity;
      transport_net[dst][src].first -= min_capacity;
    }
    answer += min_capacity;
    path.clear();
    std::fill(visited.begin(), visited.end(), false);
  }
  return answer;
}

template <bool dir, class Weight>
bool Graph<dir, Weight>::RecursiveDFSFordFulkerson(
    Vertex vertex, Vertex destination,
    std::vector<std::unordered_map<Vertex, std::pair<Weight, Weight>>>& transport_net, std::vector<Vertex>& path,
    std::vector<bool>& visited) const {
  visited[vertex] = true;
  path.emplace_back(vertex);
  if (vertex == destination) {
    return true;
  }
  for (const auto& [neigh, value] : transport_net[vertex]) {
    auto [flow, capacity] = value;
    if (!visited[neigh] && flow < capacity) {
      if (RecursiveDFSFordFulkerson(neigh, destination, transport_net, path, visited)) {
        return true;
      }
    }
  }
  path.pop_back();
  return false;
}

template <bool dir, class Wei>
template <bool directed, class Weight, EnifWeighted<Wei, Weight>>
Weight Graph<dir, Wei>::EdmondsKarp(Vertex, Vertex) const {
}

template <bool dir, class Wei>
template <bool directed, class Weight, EnifWeighted<Wei, Weight>>
Weight Graph<dir, Wei>::Dinic(Vertex source, Vertex destination) const {
  struct FlowEdge {
    Weight flow;
    Weight capacity;
    bool in_layer;
  };
  std::vector<std::unordered_map<Vertex, FlowEdge>> transport_net(adj_list_.size());
  for (Vertex u = 0; u < adj_list_.size(); ++u) {
    for (const auto& edge : adj_list_[u]) {
      transport_net[u][edge.dst] = {0, edge.weight, false};
      if (transport_net[edge.dst].find(u) == transport_net[edge.dst].end()) {
        transport_net[edge.dst][u] = {0, 0, false};
      }
    }
  }
  
  std::vector<size_t> dist(adj_list_.size());
  std::deque<Vertex> queue;
  auto bfs = [destination, &transport_net, &dist, &queue](Vertex start) {
    queue.emplace_back(start);
    for (size_t i = 0; i < dist.size(); ++i) {
      dist[i] = static_cast<size_t>(-1);
    }
    dist[start] = 0;
    while (!queue.empty()) {
      Vertex vertex = queue.front();
      queue.pop_front();
      for (auto& [neigh, fedge] : transport_net[vertex]) {
        if (dist[neigh] <= dist[vertex] || fedge.flow == fedge.capacity) {
          fedge.in_layer = false;
          continue;
        }
        if (dist[neigh] == static_cast<size_t>(-1)) {
          queue.emplace_back(neigh);
        }
        dist[neigh] = dist[vertex] + 1;
        fedge.in_layer = true;
      }
    }
    return dist[destination] < static_cast<size_t>(-1);
  };
  
  std::vector<Vertex> path;
  std::vector<bool> visited(adj_list_.size(), false);
  std::function<bool(Vertex)> rec_dfs;
  rec_dfs = [destination, &rec_dfs, &transport_net, &path, &visited](Vertex vertex) {
    visited[vertex] = true;
    path.emplace_back(vertex);
    if (vertex == destination) {
      return true;
    }
    for (const auto& [neigh, fedge] : transport_net[vertex]) {
      if (!visited[neigh] && fedge.in_layer && fedge.flow < fedge.capacity) {
        if (rec_dfs(neigh)) {
          return true;
        }
      }
    }
    path.pop_back();
    return false;
  };

  Weight answer = 0;
  while (bfs(source)) {
    while (rec_dfs(source)) {
      Weight min_capacity = std::numeric_limits<Weight>::max();
      for (size_t i = 1; i < path.size(); ++i) {
        const FlowEdge& fedge = transport_net[path[i - 1]][path[i]];
        min_capacity = std::min(min_capacity, fedge.capacity - fedge.flow);
      }
      for (size_t i = 1; i < path.size(); ++i) {
        Vertex src = path[i - 1];
        Vertex dst = path[i];
        transport_net[src][dst].flow += min_capacity;
        transport_net[dst][src].flow -= min_capacity;
      }
      answer += min_capacity;
      path.clear();
      std::fill(visited.begin(), visited.end(), false);
    }
    path.clear();
    std::fill(visited.begin(), visited.end(), false);
  }
  return answer;
}

#endif