#include "stitched_vamana.hpp"
#include "filtered_robust_prune.hpp"
#include "vamana.hpp"

DirectedGraph *stitched_vamana(Vectors& P, float a, int L_small, int R_small, int R_stitched, bool random_graph_flag, bool random_medoid_flag, bool random_subset_medoid_flag) {
    int n = P.size();
    // Initialize G to an empty or random graph
    DirectedGraph *G; 
    if (random_graph_flag) G = random_graph(n, R_stitched);
    else G = new DirectedGraph(n);

    // For each filter find the corresponding graph and stitch them together
    for (const auto& pair : P.filters_map) {
        int size = pair.second.size();
        if (size == 1) {
            continue;
        }
        int *P_f = new int[size];

        size_t index = 0;
        for (int value : pair.second) {
            P_f[index++] = value;
        }
        DirectedGraph *G_f = vamana(P, P_f, index, a, L_small, R_small, random_medoid_flag, random_subset_medoid_flag);
        G->stitch(G_f, P_f);

        delete[] P_f;
        delete G_f;
    }

    // For each vertex call the filtered robust prune
    int size = P.size();
    for (int i = 0 ; i < size ; i++) {
        std::unordered_set<int> neighbors_i = G->get_neighbors(i);

        std::set<std::pair<float, int>> ordered_neighbors_i;
        for (auto v : neighbors_i) {
            ordered_neighbors_i.insert({P.euclidean_distance(i, v), v});
        }

        filtered_robust_prune(G, P, i, ordered_neighbors_i, a, R_stitched);
    }

    return G;
}