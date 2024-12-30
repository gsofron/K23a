#include <unordered_set>
#include <vector>
#include <algorithm> 
#include "utils.hpp"
#include "greedy_search.hpp"

std::pair<std::vector<int>, std::set<std::pair<float, int>>> 
FilteredGreedySearch(DirectedGraph& graph, Vectors& vectors, int start, int query, int k, int L) {
    size_t vectors_size = vectors.size();

    // Initialize result set and visited marker array
    std::set<std::pair<float, int>> L_set;
    bool *visited = new bool[vectors_size];
    std::fill(visited, visited + vectors_size, false);

    // Insert to L_set the start node if it has the same filter with the query
    if (vectors.same_filter(query, start)) {
        L_set.insert({vectors.euclidean_distance(query, start), start});
    }

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
            if (!visited[neighbor]) {
                L_set.insert({vectors.euclidean_distance(query, neighbor), neighbor});
            }
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
    for (size_t i = 0; i < vectors_size; i++) {
        if (visited[i]) {
            L_set.insert({vectors.euclidean_distance(query, i), i});
        }
    }

    delete[] visited;
    return {result, L_set};
}

std::pair<std::vector<int>, std::set<std::pair<float, int>>> 
FilteredGreedySearch(DirectedGraph& graph, Vectors& vectors, int start, int query, int k, int L, int limit) {
    size_t vectors_size = vectors.size();

    // Initialize result set and visited marker array
    std::set<std::pair<float, int>> L_set;
    bool *visited = new bool[vectors_size];
    std::fill(visited, visited + vectors_size, false);

    // Insert to L_set the start node if it has the same filter with the query
    if (vectors.same_filter(query, start)) {
        L_set.insert({vectors.euclidean_distance(query, start), start});
    }

    // Main search loop
    while (--limit) {
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
            if (!visited[neighbor]) {
                L_set.insert({vectors.euclidean_distance(query, neighbor), neighbor});
            }
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
    for (size_t i = 0; i < vectors_size; i++) {
        if (visited[i]) {
            L_set.insert({vectors.euclidean_distance(query, i), i});
        }
    }

    delete[] visited;
    return {result, L_set};
}