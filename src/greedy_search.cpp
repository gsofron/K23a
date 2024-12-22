#include <unordered_set>
#include <vector>
#include <algorithm> 
#include "utils.hpp"
#include "greedy_search.hpp"

std::pair<std::vector<int>, std::set<std::pair<float, int>>> 
GreedySearch(DirectedGraph& graph, Vectors& vectors, int *Pf, int n, int start, int query, int k, int L) {
    // Initialize result set and visited marker array
    std::set<std::pair<float, int>> L_set;
    bool *visited = new bool[n];
    std::fill(visited, visited + n, false);

    // Start with the initial node distance
    L_set.insert({vectors.euclidean_distance(Pf[query], Pf[start]), start});  
  
    // Main search loop
    while (true) {
        // Find first unvisited node in L_set
        auto p_star = std::find_if(L_set.begin(), L_set.end(), [&](const auto& pair) {
            return !visited[pair.second];
        });
        if (p_star == L_set.end()) {
            break; // Exit if all nodes in L_set have been visited
        }
        
        visited[p_star->second] = true;

        // Insert neighbors with distances
        const auto& neighbors = graph.get_neighbors(p_star->second);
        for (auto neighbor : neighbors) {
            L_set.insert({vectors.euclidean_distance(Pf[query], Pf[neighbor]), neighbor});
        }

        // Restrict L_set to a maximum size of L
        if (L_set.size() > static_cast<unsigned long int>(L)) {
            auto it = L_set.end();
            std::advance(it, -(L_set.size() - L));
            L_set.erase(it, L_set.end());
        }
    }

    // Collect top k results
    std::vector<int> result;
    auto it = L_set.begin();
    for (int i = 0; i < k && it != L_set.end(); i++, it++) {
        result.push_back(it->second);
    }

    // Add the deleted visited indeces to L_set
    for (int i = 0; i < n; i++) {
        if (visited[i]) {
            L_set.insert({vectors.euclidean_distance(Pf[query], Pf[i]), i});
        }
    }

    delete[] visited;
    return {result, L_set};
}