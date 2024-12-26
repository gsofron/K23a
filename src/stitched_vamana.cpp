#include "stitched_vamana.hpp"
#include "filtered_robust_prune.hpp"
#include "vamana.hpp"

DirectedGraph *stitched_vamana(Vectors& P, float a, int L_small, int R_small, int R_stitched) {
    int n = P.size();
    // Initialize G to an empty graph
    DirectedGraph *G = new DirectedGraph(n);

    // For each filter find the corresponding graph and stitch them together
    int filters_size = P.filters_map.size();
    float* filters = new float[filters_size];
    int i = 0;
    for (const auto& pair : P.filters_map) {
        filters[i++] = pair.first;
    }

    #pragma omp parallel for num_threads(4)
    for (int i = 0 ; i < filters_size ; i++) {
        std::unordered_set<int> list = P.filters_map[filters[i]];
        int size = list.size();
        if (size == 1) {
            continue;
        }
        int *P_f = new int[size];

        size_t index = 0;
        for (int value : list) {
            P_f[index++] = value;
        }
        DirectedGraph *G_f = vamana(P, P_f, index, a, L_small, R_small);
        #pragma omp critical
        {
            G->stitch(G_f, P_f);
        }

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