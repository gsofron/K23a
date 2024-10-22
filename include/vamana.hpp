#pragma once

#include "directed_graph.hpp"

// Creates a random R-regular out-degree directed graph, using a vector of MathVectors
template <typename T>
DirectedGraph<T> *random_graph(std::vector<MathVector<T> *> vectors, int R) {
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
MathVector<T> *medoid(std::vector<MathVector<T> *> *vectors) {
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