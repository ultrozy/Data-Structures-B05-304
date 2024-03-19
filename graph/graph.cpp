#include <deque>
#include "graph.h"

UndirectedGraph::UndirectedGraph(size_t n) : adj_(n) {
}
UndirectedGraph::UndirectedGraph(std::initializer_list<Edge> edges) : adj_{} {
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
    adj_[rebro.second].emplace_back(rebro.first);
  }
}
std::vector<size_t> UndirectedGraph::BFS(Vertex start) {
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

DirectedGraph::DirectedGraph(size_t n) : adj_(n) {
}
DirectedGraph::DirectedGraph(std::initializer_list<Edge> edges) : adj_{} {
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
  }
}
std::vector<size_t> DirectedGraph::BFS(Vertex start) {
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

///////////////
////  DFS  ////
///////////////

std::vector<std::pair<size_t, size_t>> UndirectedGraph::DfsVisit(Vertex start) {
  std::vector<std::pair<size_t, size_t>> time(adj_.size(), std::make_pair<size_t, size_t>(0ul, 0ul));
  std::vector<Color> color(adj_.size(), Color::kWhite);
  size_t curr_time = 0ul;
  RecursiveDfsVisit(start, time, color, curr_time);
  return time;
}

void UndirectedGraph::RecursiveDfsVisit(Vertex vertex, std::vector<std::pair<size_t, size_t>>& time,  //
                                        std::vector<Color>& color, size_t& curr_time) {
  color[vertex] = Color::kGrey;
  time[vertex].first = ++curr_time;
  for (auto child : adj_[vertex]) {
    if (color[child] == Color::kWhite) {
      RecursiveDfsVisit(child, time, color, curr_time);
    }
  }
  color[vertex] = Color::kBlack;
  time[vertex].second = ++curr_time;
}

std::vector<std::pair<size_t, size_t>> DirectedGraph::DfsVisit(Vertex start) {
  std::vector<std::pair<size_t, size_t>> time(adj_.size(), std::make_pair<size_t, size_t>(0ul, 0ul));
  std::vector<Color> color(adj_.size(), Color::kWhite);
  size_t curr_time = 0ul;
  RecursiveDfsVisit(start, time, color, curr_time);
  return time;
}

void DirectedGraph::RecursiveDfsVisit(Vertex vertex, std::vector<std::pair<size_t, size_t>>& time,  //
                                      std::vector<Color>& color, size_t& curr_time) {
  color[vertex] = Color::kGrey;
  time[vertex].first = ++curr_time;
  for (auto child : adj_[vertex]) {
    if (color[child] == Color::kWhite) {
      RecursiveDfsVisit(child, time, color, curr_time);
    }
  }
  color[vertex] = Color::kBlack;
  time[vertex].second = ++curr_time;
}

bool UndirectedGraph::HasCycle() {
  std::vector<Color> color(adj_.size(), Color::kWhite);
  std::vector<Vertex> parent(adj_.size(), static_cast<size_t>(-1));
  for (Vertex vertex = 0; vertex < adj_.size(); ++vertex) {
    if (color[vertex] == Color::kWhite && RecursiveHasCycle(vertex, color, parent)) {
      return true;
    }
  }
  return false;
}

bool UndirectedGraph::RecursiveHasCycle(Vertex vertex, std::vector<Color>& color, std::vector<Vertex>& parent) {
  color[vertex] = Color::kGrey;
  for (auto child : adj_[vertex]) {
    if (color[child] == Color::kGrey && parent[vertex] != child) {
      return true;
    }
    parent[child] = vertex;
    if (RecursiveHasCycle(child, color, parent)) {
      return true;
    }
  }
  color[vertex] = Color::kBlack;
  return false;
}

bool DirectedGraph::HasCycle() {
  std::vector<Color> color(adj_.size(), Color::kWhite);
  for (Vertex vertex = 0; vertex < adj_.size(); ++vertex) {
    if (color[vertex] == Color::kWhite && RecursiveHasCycle(vertex, color)) {
      return true;
    }
  }
  return false;
}

bool DirectedGraph::RecursiveHasCycle(Vertex vertex, std::vector<Color>& color) {
  color[vertex] = Color::kGrey;
  for (auto child : adj_[vertex]) {
    if (color[child] == Color::kGrey || (color[child] == Color::kWhite && RecursiveHasCycle(child, color))) {
      return true;
    }
  }
  color[vertex] = Color::kBlack;
  return false;
}

///////////////////////////////////////////////
////////  W E I G H T E D   G R A P H  ////////
///////////////////////////////////////////////

UndirectedWeightedGraph::UndirectedWeightedGraph(size_t n) : adj_(n) {
}
UndirectedWeightedGraph::UndirectedWeightedGraph(const std::vector<std::pair<Edge, Weight>>& edges) : adj_{} {
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
std::vector<size_t> UndirectedWeightedGraph::BFS_01(Vertex start) {
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
std::vector<size_t> UndirectedWeightedGraph::BFS_0k(Vertex start, size_t k) {
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