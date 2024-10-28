#pragma once

#include <set>      // std::set etc.

#include "directed_graph.hpp"
#include "utils.hpp"
#include "vectors.hpp"

// Modifies graph 'G' by setting at most 'R' new out-neighbors for point with index 'p'
// 'V' is an ordered candidate set containing indices of vertices and 'a' is the distance threshold where a >= 1
// 'V' is sorted in ascending euclidean distance between each point and point 'p'
template <typename T>
void robust_prune(DirectedGraph *G, Vectors<T>& vectors, int p, std::set<int, CompareDistance<T>>& V, float a, int R) {
    const std::unordered_set<int>& N_out_p = G->get_neighbors(p);

    // V <- (V U Nout(p)) \ {p}
    for (auto index : N_out_p) {
        V.insert(index);
    }
    V.erase(p);

    // Nout(p) <- empty set
    for (auto it = N_out_p.begin() ; it != N_out_p.end() ; it = N_out_p.begin()) {
        G->remove(p, *it);
    }

    // while V not empty
    while (!V.empty()) {
        // p* <- arg min d(p, p') where p' in V
        // NOTE: Since V was sorted, this is done in O(1) time
        int p_star = *V.begin();

        // Nout(p) <- Nout(p) U {p*}
        G->insert(p, p_star);

        // if |Nout(p)| = R then break
        if ((int)N_out_p.size() == R) break;

        // for p' in V do
        for (auto it = V.begin() ; it != V.end() ; ) {
            int p_prime = *it;

            // if a ⋅ d(p*, p') <= d(p, p') then remove p' from V
            if (a * vectors.euclidean_distance(p_star, p_prime) <= vectors.euclidean_distance(p, p_prime)) it = V.erase(it);
            else it++;
        }
    }
}