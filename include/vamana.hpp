#pragma once

#include <algorithm>
#include <limits>
#include <random>

#include "directed_graph.hpp"
#include "greedy_search.hpp"
#include "robust_prune.hpp"
#include "vectors.hpp"

// Creates a random R-regular out-degree directed graph
DirectedGraph *random_graph(int num_of_vertices, int R) {
    // If there are k vertices, each vertex can have at most k-1 neighbours
    ERROR_EXIT(R > num_of_vertices - 1, "Too many neighbors")
    ERROR_EXIT(R <= 0, "Invalid R")

    DirectedGraph *g = new DirectedGraph(num_of_vertices);
    // In each iteration, save vertices that have already been picked
    bool *picked_indexes = new bool[num_of_vertices];
    // Repeat for all vertices
    for (int i = 0, index; i < num_of_vertices; i++) {
        // (Re-)init picked_indexes[] array
        std::fill(picked_indexes, picked_indexes + num_of_vertices, false);
        // Vertex cannot point to itself
        picked_indexes[i] = true;
        // Choose a non-repeating new neighbor randomly
        for (int j = 0; j < R; j++) {
            do { index = rand() % num_of_vertices; } while (picked_indexes[index]);
            picked_indexes[index] = true;
            g->insert(i, index);
        }
    }
    delete[] picked_indexes;

    return g;
}

// Returns the medoid vertex (vector index) of given dataset
template <typename T>
Vertex medoid(const Vectors<T>& vectors) {
    // Simple brute-force algorithm
    float min = std::numeric_limits<float>::max();
    Vertex m = -1;
    int n = vectors.size();
    for (int i = 0; i < n; i++) {
        float sum = 0.0;
        for (int j = 0; j < n; j++) {
            if (i == j) continue;
            sum += vectors.euclidean_distance(i, j);
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
    // Init the R-regular (counting out-degree only) graph
    int n = P.size();
    DirectedGraph *G = random_graph(n, R);
    
    // Init vectors' medoid
    Vertex s = medoid(P);
    
    // Create the random permutation sigma (σ)
    Vertex *sigma = new Vertex[n];
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
                CompareDistance<T> comparator(j, P);
                std::set<int, CompareDistance<T>> new_N_out_j(comparator);
                for (auto v : N_out_j) {
                    new_N_out_j.insert(v);
                }
                new_N_out_j.insert(sigma[i]);
                robust_prune(G, P, j, new_N_out_j, a, R);
            } else {
                G->insert(j, sigma[i]);
            }
        }
    }

    delete[] sigma;

    return G;
}