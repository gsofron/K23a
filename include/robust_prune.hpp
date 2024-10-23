#pragma once

#include <set>      // std::set etc.
#include <tuple>    // std::tuple etc.

#include "directed_graph.hpp"
#include "vector.hpp"

// Custom compare function for tuples
// Input is 2 tuples with the following structure: (MathVector *, float)
// Tuple1 is lesser than Tuple2 if: float1 < float2
template <typename T>
struct CustomCompare {
    bool operator()(std::tuple<MathVector<T> *, float> a, std::tuple<MathVector<T> *, float> b) const {
        return std::get<1>(a) < std::get<1>(b);
    }
};

// Modifies graph 'g' by setting at most 'R' new out-neighbors for point 'p'
// 'V' is a candidate set containing vertices and 'a' is the distance threshold where a >= 1
template <typename T>
void robust_prune(DirectedGraph<T> *g, MathVector<T> *p, std::unordered_set<MathVector<T> *>& V, float a, int R) {
    ERROR_EXIT(a < 1.0, "a cannot be less than 1");
    ERROR_EXIT(R < 1, "R cannot be less than 1");

    // Create an ordered set containing tuples with the following structure: (MathVector *, float)
    // By doing this, we sort V by placing the nearest vertices of p first
    std::set<std::tuple<MathVector<T> *, float>, CustomCompare<T>> ordered_v;
    for (auto vector : V) {
        std::tuple<MathVector<T> *, float> vector_tuple(vector, p->euclidean_distance(*vector));
        ordered_v.insert(vector_tuple);
    }

    const std::unordered_set<MathVector<T> *>& n_out_p = g->get_neighbors(p);

    // V <- (V U Nout(p)) \ {p}
    for (auto vector : n_out_p) {
        std::tuple<MathVector<T> *, float> vector_tuple(vector, p->euclidean_distance(*vector));
        ordered_v.insert(vector_tuple);
    }
    std::tuple<MathVector<T> *, float> p_tuple(p, 0.0);
    ordered_v.erase(p_tuple);

    // Nout(p) <- empty set
    for (auto it = n_out_p.begin() ; it != n_out_p.end() ; it = n_out_p.begin()) {
        MathVector<T> *vector = *it;
        g->remove(p, vector);
    }

    // while V not empty
    while (!ordered_v.empty()) {
        // p* <- arg min d(p, p') where p' in V
        // NOTE: Since V was sorted, this is done in O(1) time
        auto vector_tuple = *ordered_v.begin();
        MathVector<T> *p_star = std::get<0>(vector_tuple);

        // Nout(p) <- Nout(p) U {p*}
        g->insert(p, p_star);

        // if |Nout(p)| = R then break
        if ((int)n_out_p.size() == R) break;

        // for p' in V do
        for (auto it = ordered_v.begin() ; it != ordered_v.end() ; ) {
            vector_tuple = *it;
            MathVector<T> *point = std::get<0>(vector_tuple);

            // if a ⋅ d(p*, p') <= d(p, p') then remove p' from V
            if (a * p_star->euclidean_distance(*point) <= p->euclidean_distance(*point)) {
                it = ordered_v.erase(it);
            }
            else it++;
        }
    }
}