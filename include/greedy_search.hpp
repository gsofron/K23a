#pragma once

#include <set>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include "vectors.hpp"        
#include "directed_graph.hpp"  
#include "utils.hpp"

template <typename T>
std::pair<std::vector<int>, std::set<int, CompareDistance<T>>> 
GreedySearch(DirectedGraph& graph, Vectors<T>& vectors, int start, int query, int k, int L) {
    CompareDistance<T> comparator(query, vectors);  

    std::set<int, CompareDistance<T>> L_set(comparator);

    std::set<int, CompareDistance<T>> visited(comparator);
    
    L_set.insert(start);  

    // Continue searching while there are unvisited nodes in L_set
    while (!L_set.empty()) {
       
        int p_star = *L_set.begin();
        L_set.erase(L_set.begin()); 

        if (visited.find(p_star) != visited.end()) {
            continue;
        }

        visited.insert(p_star);  

        auto neighbors = graph.get_neighbors(p_star);
        for (auto neighbor : neighbors) {
            L_set.insert(neighbor);
        }

        if (L_set.size() > static_cast<unsigned long int>(L)) {
            auto it = L_set.end();
            std::advance(it, -(L_set.size() - L)); 
            L_set.erase(it, L_set.end());          
        }
    }

    // Collect the k-nearest neighbors 
    std::vector<int> result;

    return {result, visited};
}