#include <algorithm>
#include <random>

#include "filtered_greedy_search.hpp"
#include "filtered_robust_prune.hpp"
#include "filtered_vamana.hpp"
#include "findmedoid.hpp"

DirectedGraph *filtered_vamana(Vectors& P, float a, int L, int R, int threshold) {
    int n = P.size();
    // Initialize G to an empty graph
    DirectedGraph *G = new DirectedGraph(n);

    // Start node (index) of every filter
    auto *st = find_medoid(P, threshold);

    // Create the random permutation sigma (σ)
    int *sigma = new int[n];
    for (int i = 0; i < n; i++)
        sigma[i] = i;
    // Shuffle to create the random permutation
    // Source for how to shuffle: https://stackoverflow.com/a/6926473
    auto rd = std::random_device {}; 
    auto rng = std::default_random_engine { rd() };
    std::shuffle(sigma, sigma + n, rng);

    for (int i = 0; i < n; i++) {
        float Fx = P.filters[sigma[i]]; // Filter of current index
        int s = st->at(Fx); // Medoid point of this filter

        auto V_Fx = FilteredGreedySearch(*G, P, s, sigma[i], 0, L).second;
        filtered_robust_prune(G, P, sigma[i], V_Fx, a, R);

        const auto& N_out_sigma_i = G->get_neighbors(sigma[i]);
        for (auto j : N_out_sigma_i) {
            G->insert(j, sigma[i]);
            const auto& N_out_j = G->get_neighbors(j);

            if ((int)N_out_j.size() > R) {
                std::set<std::pair<float, int>> new_N_out_j;
                for (auto v : N_out_j)
                    new_N_out_j.insert({P.euclidean_distance(j, v), v});

                filtered_robust_prune(G, P, j, new_N_out_j, a, R);
            }
        }
    }

    delete[] sigma;
    delete st;

    return G;
}