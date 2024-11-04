#pragma once

#include <algorithm>
#include <limits>
#include <random>
#include <string>

#include "directed_graph.hpp"
#include "greedy_search.hpp"
#include "robust_prune.hpp"
#include "vectors.hpp"

// Creates a random R-regular out-degree directed graph
DirectedGraph *random_graph(int num_of_vertices, int R);

// Returns the medoid vertex (vector index) of given dataset
template <typename T>
int medoid(const Vectors<T>& vectors) {
    // Simple brute-force algorithm
    float min = std::numeric_limits<float>::max();
    int m = -1;
    int n = vectors.size();
    for (int i = 0; i < n; i++) {
        float sum = 0.0;
        for (int j = i + 1; j < n; j++) { // Calculate euclideian distances
            sum += vectors.euclidean_distance(i, j);
        }
        for (int j = 0; j < i && sum < min; j++) { // Cached elements; have already calculated symmetric cases
            sum += vectors.euclidean_distance_cached(i, j);
        }
        if (sum < min) {
            min = sum;
            m = i;
        }
    }
    return m;
}

// Vamana Indexing Algorithm implementation, based on provided paper
template <typename T>
DirectedGraph *vamana(Vectors<T>& P, float a, int L, int R) {
    int n = P.size();
    // Init the R-regular (counting out-degree only) graph
    DirectedGraph *G = random_graph(n, R);
    
    // Init vectors' medoid
    int s = medoid(P);
    
    // Create the random permutation sigma (σ)
    int *sigma = new int[n];
    for (int i = 0; i < n; i++) {
        sigma[i] = i;
    }
    // Shuffle to create the random permutation
    // Source for how to shuffle: https://stackoverflow.com/a/6926473
    auto rd = std::random_device {}; 
    auto rng = std::default_random_engine { rd() };
    std::shuffle(sigma, sigma + n, rng);
    
    for (int i = 0; i < n; i++) {
        auto [Lset, V] = GreedySearch(*G, P, s, sigma[i], 1, L);
        robust_prune(G, P, sigma[i], V, a, R);

        const auto& N_out_sigma_i = G->get_neighbors(sigma[i]);
        for (auto j : N_out_sigma_i) {
            const auto& N_out_j = G->get_neighbors(j);
            if ((int)N_out_j.size() + 1 > R) {
                std::set<std::pair<float, int>> new_N_out_j;
                for (auto v : N_out_j) {
                    new_N_out_j.insert({P.euclidean_distance_cached(j, v), v});
                }
                new_N_out_j.insert({P.euclidean_distance_cached(j, sigma[i]), sigma[i]});
                robust_prune(G, P, j, new_N_out_j, a, R);
            } else {
                G->insert(j, sigma[i]);
            }
        }
    }

    delete[] sigma;

    return G;
}

// Writes (stores) a vamana graph into a (binary) file
void write_vamana_to_file(DirectedGraph& g, const std::string& file_name);

// Reads (loads) a vamana graph from a (binary) file
// The function acts as a constructor for a vamana graph, returning a pointer to the allocated memory
DirectedGraph *read_vamana_from_file(const std::string& file_name);