#pragma once

#include <set>
#include <unordered_set>
#include <vector>
#include "vector.hpp"        
#include "directed_graph.hpp"  
#include "utils.hpp"

template <typename T>
std::pair<std::vector<MathVector<T>*>, std::set<MathVector<T>*, CompareDistance<T>>> 
GreedySearch(DirectedGraph<T>& graph, MathVector<T> *start, MathVector<T> *query, int k, int L) {
    CompareDistance<T> comparator(query);  

    std::set<MathVector<T>*, CompareDistance<T>> L_set(comparator);

    std::set<MathVector<T>*, CompareDistance<T>> visited(comparator);
    
    L_set.insert(start);  

    // Continue searching while there are unvisited nodes in L_set
    while (!std::all_of(L_set.begin(), L_set.end(), [&](MathVector<T> *node) { return visited.count(node); })) {
       
        MathVector<T> *p_star = *L_set.begin();
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
    std::vector<MathVector<T>*> result;
    auto it = visited.begin();
    for (int i = 0; i < k && it != visited.end(); ++i, ++it) {
        result.push_back(*it); 
    }

    return {result, visited};
}
