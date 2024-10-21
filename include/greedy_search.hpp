// greedy_search.hpp

#ifndef GREEDY_SEARCH_HPP
#define GREEDY_SEARCH_HPP

#include <set>
#include <unordered_set>
#include <vector>
#include "vector.hpp"          // Your MathVector class implementation
#include "directed_graph.hpp"  // Your DirectedGraph class implementation

// Custom comparator for sorting by distance to the query vector
template <typename T>
struct CompareDistance {
    MathVector<T>* query;

    CompareDistance(MathVector<T>* query) : query(query) {}

    bool operator()(const MathVector<T>* a, const MathVector<T>* b) const {
        return (a->euclidean_distance(*query) < b->euclidean_distance(*query) || (a->euclidean_distance(*query) == b->euclidean_distance(*query) && a < b));
    }
};

template <typename T>
std::pair<std::vector<MathVector<T>*>, std::unordered_set<MathVector<T>*>> 
GreedySearch(DirectedGraph<T>& graph, MathVector<T>* start, MathVector<T>* query, int k, long unsigned int L) {
    CompareDistance<T> comparator(query);
    
    std::set<MathVector<T>*, CompareDistance<T>> L_set(comparator);
    std::set<MathVector<T>*, CompareDistance<T>> visited(comparator);
    
    L_set.insert(start);

    while (!std::all_of(L_set.begin(), L_set.end(), [&](MathVector<T>* node) { return visited.count(node); })) {
        MathVector<T>* p_star = *L_set.begin();
        L_set.erase(L_set.begin());


        if (visited.find(p_star) != visited.end()) {
            continue;
        }

        visited.insert(p_star);
        auto neighbors = graph.get_neighbors(p_star);
        
        for (auto neighbor : neighbors) {
            L_set.insert(neighbor);
        }

        if (L_set.size() > L) {
            auto it = L_set.end();
            std::advance(it, -(L_set.size() - L)); 
            L_set.erase(it, L_set.end());         
        }
    }

    std::vector<MathVector<T>*> result;
    auto it = visited.begin();
    for (int i = 0; i < k && it != visited.end(); ++i, ++it) {
        result.push_back(*it);
    }
    std::unordered_set<MathVector<T>*> return_visited(visited.begin(), visited.end());
    return {result, return_visited};
}

#endif  // GREEDY_SEARCH_HPP
