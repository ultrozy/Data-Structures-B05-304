#ifndef _GRAPH_PRIMITIVES_H_
#define _GRAPH_PRIMITIVES_H_

#include <cstddef>
#include <functional>
#include <vector>
#include <ostream>

using Vertex = size_t;
enum class Color { kWhite, kGrey, kBlack };
using DFStimes = std::vector<std::pair<size_t, size_t>>;

template <bool dir, bool directed>
using EnifDirected = std::enable_if_t<(dir == directed) && directed, int>;
template <bool dir, bool directed>
using EnifUndirected = std::enable_if_t<(dir == directed) && !directed, int>;
template <class Wei, class Weight>
using EnifWeighted = std::enable_if_t<std::is_same_v<Wei, Weight> && !std::is_same_v<Weight, void>, int>;
template <class Wei, class Weight>
using EnifNoWeight = std::enable_if_t<std::is_same_v<Wei, Weight> && std::is_same_v<Weight, void>, int>;

template <class Weight = void>
struct Edge {
  Vertex src;
  Vertex dst;
  Weight weight;

  Edge(Vertex src, Vertex dst, Weight weight) : src(src), dst(dst), weight(weight) {
  }
};
template <>
struct Edge<void> {
  Vertex src;
  Vertex dst;

  Edge(Vertex src, Vertex dst) : src(src), dst(dst) {
  }
};

template <class Weight>
bool operator==(const Edge<Weight>& lhs, const Edge<Weight>& rhs) {
  return (lhs.src == rhs.src) && (lhs.dst == rhs.dst) && (lhs.weight == rhs.weight);
}
template <>
bool operator==(const Edge<void>& lhs, const Edge<void>& rhs) {
  return (lhs.src == rhs.src) && (lhs.dst == rhs.dst);
}

template <class Weight>
struct std::hash<Edge<Weight>> {
  size_t operator()(const Edge<Weight>& edge) const noexcept {
    size_t h_src = std::hash<Vertex>{}(edge.src);
    size_t h_dst = std::hash<Vertex>{}(edge.dst);
    size_t h_weight = std::hash<Weight>{}(edge.weight);
    h_src ^= ((h_dst << (4 * sizeof(size_t))) | (h_dst >> (4 * sizeof(size_t))));
    return h_weight ^ ((h_src << (4 * sizeof(size_t))) | (h_src >> (4 * sizeof(size_t))));
  }
};
template <>
struct std::hash<Edge<void>> {
  size_t operator()(const Edge<void>& edge) const noexcept {
    size_t h_src = std::hash<Vertex>{}(edge.src);
    size_t h_dst = std::hash<Vertex>{}(edge.dst);
    return h_src ^ ((h_dst << (4 * sizeof(size_t))) | (h_dst >> (4 * sizeof(size_t))));
  }
};

template <class Weight>
std::ostream& operator<<(std::ostream& os, const Edge<Weight>& edge) {
  return os << '(' << edge.src << " - " << edge.dst << " : " << edge.weight << ')';
}

template <>
std::ostream& operator<<(std::ostream& os, const Edge<void>& edge) {
  return os << '(' << edge.src << " - " << edge.dst << ')';
}

#endif