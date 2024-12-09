#pragma once

#include <set>
#include "vectors.hpp"        
#include "directed_graph.hpp" 

std::pair<std::vector<int>, std::set<std::pair<float, int>>> GreedySearch(DirectedGraph& graph, Vectors& vectors, int *Pf, int n, int start, int query, int k, int L); 