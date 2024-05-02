#ifndef GRAPH_H_
#define GRAPH_H_

#include "./_graph_primitives.h"
#include "./_graph_class.h"
#include "./graph_bfs.h"
#include "./graph_dfs.h"
#include "./graph_euler.h"
#include "./graph_ap_bridge.h"
#include "./graph_mst.h"
#include "./graph_distance.h"
#include "./graph_flows.h"

template <class Weight = void>
using UndirectedGraph = Graph<false, Weight>;

template <class Weight = void>
using DirectedGraph = Graph<true, Weight>;

#endif