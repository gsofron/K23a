#include "robust_prune.hpp"
#include "utils.hpp"            // ERROR_CHECK()

void robust_prune(DirectedGraph *G, Vectors& vectors, int *Pf, int p, std::set<std::pair<float, int>>& V, float a, int R) {
    const std::unordered_set<int>& N_out_p = G->get_neighbors(p);

    // V <- (V U Nout(p)) \ {p}
    for (auto index : N_out_p) {
        V.insert({vectors.euclidean_distance_cached(Pf[p], Pf[index]), index});
    }
    V.erase({0.0, p});

    // Nout(p) <- empty set
    for (auto it = N_out_p.begin() ; it != N_out_p.end() ; it = N_out_p.begin()) {
        G->remove(p, *it);
    }
    // Save this here so we don't call N_out_p.size() on each iteration of the following loop
    int N_out_p_size = 0;

    // while V not empty
    while (!V.empty()) {
        // p* <- arg min d(p, p') where p' in V
        // NOTE: Since V was sorted, this is done in O(1) time
        int p_star = V.begin()->second;

        // Nout(p) <- Nout(p) U {p*}
        G->insert(p, p_star);
        N_out_p_size++;

        // if |Nout(p)| = R then break
        if (N_out_p_size == R) break;

        // for p' in V do
        for (auto it = V.begin() ; it != V.end() ; ) {
            int p_prime = it->second;

            // if a ⋅ d(p*, p') <= d(p, p') then remove p' from V
            if (a * vectors.euclidean_distance_cached(Pf[p_star], Pf[p_prime]) <= vectors.euclidean_distance_cached(Pf[p], Pf[p_prime])) it = V.erase(it);
            else it++;
        }
    }
}