#pragma once

#include <set>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include "vectors.hpp"        
#include "directed_graph.hpp"  
#include "utils.hpp"

template <typename T>
std::pair<std::vector<int>, std::set<std::pair<int, int>>> 
GreedySearch(DirectedGraph& graph, Vectors<T>& vectors, int start, int query, int k, int L) {

    (void) k;
    std::set<std::pair<int, int>> L_set;
    size_t vectors_size = vectors.size();
    bool *visited = new bool[vectors_size];
    std::fill(visited, visited + vectors_size, false);
    L_set.insert({vectors.euclidean_distance_cached(query, start), start});  
  
    while (true) {
        auto p_star = std::find_if(L_set.begin(), L_set.end(), [&](const auto& pair) {
            return !visited[pair.second];
        });
        if (p_star == L_set.end()) {
            break;
        }
        
        visited[p_star->second] = true;  

        auto neighbors = graph.get_neighbors(p_star->second);
        for (auto neighbor : neighbors) {
            L_set.insert({vectors.euclidean_distance_cached(query, neighbor), neighbor});
        }

        if (L_set.size() > static_cast<unsigned long int>(L)) {
        
            auto it = L_set.end();
            std::advance(it, -(L_set.size() - L)); 
            L_set.erase(it, L_set.end());          
        }
    }

    std::vector<int> result;
    auto it = L_set.begin();
    for (int i = 0; i < k && it != L_set.end(); ++i, ++it) {
        result.push_back(it->second); 
    }
    delete[] visited;
    return {result, L_set};
}
