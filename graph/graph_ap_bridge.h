#ifndef GRAPH_AP_BRIDGE_H_
#define GRAPH_AP_BRIDGE_H_

#include "./_graph_class.h"

template <bool dir, class Wei>
template <bool directed, EnifUndirected<dir, directed>>
std::vector<Vertex> Graph<dir, Wei>::ArticulationPoints() const {
  DFStimes time(adj_list_.size(), std::make_pair<size_t, size_t>(0ul, 0ul));  // first = time_in, second = time_up
  std::vector<Color> color(adj_list_.size(), Color::kWhite);
  std::vector<Vertex> artic_points{};
  size_t curr_time = 0ul;
  for (Vertex vertex = 0; vertex < adj_list_.size(); ++vertex) {
    if (color[vertex] == Color::kWhite) {
      RecursiveAP({}, vertex, time, color, artic_points, curr_time);
    }
  }
  return artic_points;
}

template <bool dir, class Wei>
void Graph<dir, Wei>::RecursiveAP(std::optional<Vertex> parent, Vertex vertex, DFStimes& time,
                                  std::vector<Color>& color, std::vector<Vertex>& artic_points,
                                  size_t& curr_time) const {
  color[vertex] = Color::kGrey;
  time[vertex].first = time[vertex].second = ++curr_time;
  bool is_ap = false;
  size_t num_children = 0ul;

  for (const auto& edge : adj_list_[vertex]) {
    Vertex child = edge.dst;
    if (color[child] == Color::kWhite) {
      ++num_children;
      RecursiveAP(vertex, child, time, color, artic_points, curr_time);
      time[vertex].second = std::min(time[vertex].second, time[child].second);
      if (parent.has_value() && time[child].second >= time[vertex].first) {
        is_ap = true;
      }
    } else if (color[child] == Color::kGrey) {
      time[vertex].second = std::min(time[vertex].second, time[child].first);
    }
  }
  if (!parent.has_value() && num_children > 1ul) {
    is_ap = true;
  }

  if (is_ap) {
    artic_points.emplace_back(vertex);
  }
  color[vertex] = Color::kBlack;
}

template <bool dir, class Wei>
template <bool directed, EnifUndirected<dir, directed>>
std::vector<Edge<Wei>> Graph<dir, Wei>::Bridges() const {
  DFStimes time(adj_list_.size(), std::make_pair<size_t, size_t>(0ul, 0ul));  // first = time_in, second = time_up
  std::vector<Color> color(adj_list_.size(), Color::kWhite);
  std::vector<Edge<Wei>> bridges{};
  size_t curr_time = 0ul;
  for (Vertex vertex = 0; vertex < adj_list_.size(); ++vertex) {
    if (color[vertex] == Color::kWhite) {
      RecursiveBridges({}, vertex, time, color, bridges, curr_time);
    }
  }

  // delete multiple edges
  std::unordered_map<Edge<Wei>, size_t> num_edge{};
  for (Vertex vertex = 0; vertex < adj_list_.size(); ++vertex) {
    for (const auto& edge : adj_list_[vertex]) {
      if (num_edge.find(edge) != num_edge.end()) {
        ++num_edge[edge];
      } else {
        num_edge[edge] = 1ul;
      }
    }
  }
  for (const auto& [edge, num] : num_edge) {
    if (num <= 1ul) {
      continue;
    }
    auto it = bridges.find(edge);
    if (it != bridges.end()) {
      bridges.erase(it);
    }
  }
  return bridges;
}

template <bool dir, class Wei>
void Graph<dir, Wei>::RecursiveBridges(std::optional<Vertex> parent, Vertex vertex, DFStimes& time,
                                       std::vector<Color>& color, std::unordered_set<Edge<Wei>>& bridges,
                                       size_t& curr_time) const {
  color[vertex] = Color::kGrey;
  time[vertex].first = time[vertex].second = ++curr_time;

  for (const auto& edge : adj_list_[vertex]) {
    Vertex child = edge.dst;
    if (child == parent) {
      continue;
    }
    if (color[child] == Color::kWhite) {
      RecursiveBridges(vertex, child, time, color, bridges, curr_time);
      time[vertex].second = std::min(time[vertex].second, time[child].second);
      if (time[child].second > time[vertex].first) {
        bridges.emplace(edge);
      }
    } else if (color[child] == Color::kGrey) {
      time[vertex].second = std::min(time[vertex].second, time[child].first);
    }
  }
  color[vertex] = Color::kBlack;
}

#endif