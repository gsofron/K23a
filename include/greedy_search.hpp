#pragma once

#include <set>

std::pair<std::vector<int>, std::set<std::pair<float, int>>> GreedySearch(DirectedGraph& graph, Vectors& vectors, int start, int query, int k, int L); 