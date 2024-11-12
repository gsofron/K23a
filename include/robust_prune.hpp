#pragma once

#include <set>                  // std::set

#include "directed_graph.hpp"   // DirectedGraph
#include "vectors.hpp"          // Vectors

// Modifies graph 'G' by setting at most 'R' new out-neighbors for point with index 'p'
// 'V' is an ordered candidate set containing pairs of (euclidean distance, index) and 'a' is the distance threshold where a >= 1
// 'V' is sorted in ascending euclidean distance between each point and point 'p'
void robust_prune(DirectedGraph *G, Vectors& vectors, int p, std::set<std::pair<float, int>>& V, float a, int R);