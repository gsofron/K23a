#pragma once

#include <set>
#include "vectors.hpp"        
#include "directed_graph.hpp" 

std::pair<std::vector<int>, std::set<std::pair<float, int>>> 
FilteredGreedySearch(DirectedGraph& graph, Vectors& vectors, int *starts, int starts_num, int query, int k, int L); 