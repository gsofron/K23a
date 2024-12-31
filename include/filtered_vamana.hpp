#pragma once

#include "directed_graph.hpp"
#include "vectors.hpp"

DirectedGraph *filtered_vamana(Vectors& P, float a, int L, int R, int threshold, std::unordered_map<float, int> *M, bool random_graph_flag, int limit);