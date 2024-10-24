#pragma once

#include <set>      // std::set etc.

#include "directed_graph.hpp"
#include "vector.hpp"

// Modifies graph 'g' by setting at most 'R' new out-neighbors for point 'p'
// 'V' is a candidate set containing vertices and 'a' is the distance threshold where a >= 1
template <typename T>
void robust_prune(DirectedGraph<T> *g, MathVector<T> *p, std::set<MathVector<T>*, CompareDistance<T>>& V, float a, int R) {
    ERROR_EXIT(a < 1.0, "a cannot be less than 1");

    const std::unordered_set<MathVector<T> *>& n_out_p = g->get_neighbors(p);

    // V <- (V U Nout(p)) \ {p}
    for (auto vector : n_out_p) {
        V.insert(vector);
    }
    V.erase(p);

    // Nout(p) <- empty set
    for (auto it = n_out_p.begin() ; it != n_out_p.end() ; it = n_out_p.begin()) {
        MathVector<T> *vector = *it;
        g->remove(p, vector);
    }

    // while V not empty
    while (!V.empty()) {
        // p* <- arg min d(p, p') where p' in V
        // NOTE: Since V was sorted, this is done in O(1) time
        MathVector<T> *p_star = *V.begin();

        // Nout(p) <- Nout(p) U {p*}
        g->insert(p, p_star);

        // if |Nout(p)| = R then break
        if ((int)n_out_p.size() == R) break;

        // for p' in V do
        for (auto it = V.begin() ; it != V.end() ; ) {
            MathVector<T> *point = *it;

            // if a ⋅ d(p*, p') <= d(p, p') then remove p' from V
            if (a * p_star->euclidean_distance(*point) <= p->euclidean_distance(*point)) {
                it = V.erase(it);
            }
            else it++;
        }
    }
}