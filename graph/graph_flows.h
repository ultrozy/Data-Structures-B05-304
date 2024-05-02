#ifndef GRAPH_FLOWS_H_
#define GRAPH_FLOWS_H_

#include <unordered_map>
#include <vector>
#include <algorithm>
#include <limits>
#include "./_graph_class.h"

template <bool dir, class Wei>
template <bool directed, class Weight, EnifUndirected<dir, directed>, EnifWeighted<Wei, Weight>>
Weight Graph<dir, Wei>::FordFulkerson(Vertex source, Vertex destination) const {
  std::unordered_map<Edge<Weight>, Weight> flow_net;
  Weight answer = 0;
  std::vector<Edge<Weight>> path;
  std::vector<bool> visited(adj_list_.size(), false);

  while (RecursiveDFSFordFulkerson({source, source, 0}, destination, flow_net, path, visited)) {
    Weight min_capacity = std::numeric_limits<Weight>::max();
    for (size_t i = 1; i < path.size(); ++i) {
      min_capacity = std::min(min_capacity, path[i].weight - flow_net[path[i]]);
    }
    for (const auto& edge : path) {
      flow_net[edge] += min_capacity;
      flow_net[{edge.dst, edge.src, edge.weight}] -= min_capacity;
    }
    answer += min_capacity;
    path.clear();
    std::fill(visited.begin(), visited.end(), false);
  }
  return answer;
}

template <bool dir, class Weight>
bool Graph<dir, Weight>::RecursiveDFSFordFulkerson(Edge<Weight> cur_edge, Vertex destination,
                                                   std::unordered_map<Edge<Weight>, Weight>& flow_net,
                                                   std::vector<Edge<Weight>>& path, std::vector<bool>& visited) const {
  visited[cur_edge.dst] = true;
  path.emplace_back(cur_edge);
  if (cur_edge.dst == destination) {
    return true;
  }
  for (const auto& edge : adj_list_[cur_edge.dst]) {
    if (!visited[edge.dst] && flow_net[edge] < edge.weight) {
      if (RecursiveDFSFordFulkerson(edge, destination, flow_net, path, visited)) {
        return true;
      }
    }
  }
  path.pop_back();
  return false;
}

template <bool dir, class Wei>
template <bool directed, class Weight, EnifUndirected<dir, directed>, EnifWeighted<Wei, Weight>>
Weight Graph<dir, Wei>::EdmondsKarp(Vertex, Vertex) const {
}

template <bool dir, class Wei>
template <bool directed, class Weight, EnifUndirected<dir, directed>, EnifWeighted<Wei, Weight>>
Weight Graph<dir, Wei>::Dinic(Vertex, Vertex) const {
}

#endif