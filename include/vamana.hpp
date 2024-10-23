#pragma once

#include <limits>
#include <random>

#include "directed_graph.hpp"
#include "greedy_search.hpp"
#include "robust_prune.hpp"

// Creates a random R-regular out-degree directed graph, using a vector of MathVectors
template <typename T>
DirectedGraph<T> *random_graph(std::vector<MathVector<T> *>& vectors, int R) {
    int vector_count = vectors.size(), index;
    // If there are k vertices, each vertex can have at most k-1 neighbours
    ERROR_EXIT(R > vector_count - 1, "Too many neighbors")
    ERROR_EXIT(R <= 0, "Invalid R")

    DirectedGraph<T> *random_graph = new DirectedGraph<T>(vectors[0]->dimension());
    // In each iteration, save vertices that have already been picked
    bool *picked_indexes = new bool[vector_count];
    // Repeat for all vertices
    for (int i = 0; i < vector_count; i++) {
        // (Re-)init picked_indexes[] array
        std::fill(picked_indexes, picked_indexes + vector_count, false);
        // Vertex cannot point to itself
        picked_indexes[i] = true;
        // Choose a non-repeating new neighbor randomly
        for (int j = 0; j < R; j++) {
            do { index = rand() % vector_count; } while (picked_indexes[index]);
            picked_indexes[index] = true;
            random_graph->insert(vectors[i], vectors[index]);
        }
    }
    delete[] picked_indexes;

    return random_graph;
}

// Returns the medoid point of given dataset
template <typename T>
MathVector<T> *medoid(std::vector<MathVector<T> *>& vectors) {
    // Simple brute-force algorithm
    float min = std::numeric_limits<float>::max();
    MathVector<T> *v = nullptr;
    for (auto it1 = vectors.begin(); it1 != vectors.end(); it1++) {
        float sum = 0.0;
        for (auto it2 = vectors.begin(); it2 != vectors.end(); it2++) {
            if (it1 == it2) continue;
            sum += (*it1)->euclidean_distance(**it2);
        }
        if (sum < min) {
            min = sum;
            v = *it1;
        }
    }
    return v;
}

// Vamana Indexing Algorithm implementation, based on provided paper
template <typename T>
DirectedGraph<T> *vamana(std::vector<MathVector<T> *> *P, float a, int L, int R) {
    // Init the R-regular (counting out-degree only) graph
    DirectedGraph<T> *G = random_graph(*P, R);
    
    // Init vectors' medoid
    MathVector<T> *s = medoid(*P);
    
    // Create the random permutation sigma (σ)
    std::vector<MathVector<T> *> sigma = *P;
    // Source for how to shuffle a vector in C++: https://stackoverflow.com/a/6926473
    auto rd = std::random_device {}; 
    auto rng = std::default_random_engine { rd() };
    std::shuffle(sigma.begin(), sigma.end(), rng);
    
    int n = P->size();
    for (int i = 0; i < n; i++) {
        auto [Lset, V] = GreedySearch(*G, s, sigma[i], 1, L);
        robust_prune(G, sigma[i], V, a, R);

        const auto& N_out_sigma_i = G->get_neighbors(sigma[i]);
        for (auto j : N_out_sigma_i) {
            const auto& N_out_j = G->get_neighbors(j);
            if ((int)N_out_j.size() + 1 > R) {
                auto new_N_out_j = N_out_j;
                new_N_out_j.insert(sigma[i]);
                robust_prune(G, j, new_N_out_j, a, R);
            } else {
                G->insert(j, sigma[i]);
            }
        }
    }

    return G;
}