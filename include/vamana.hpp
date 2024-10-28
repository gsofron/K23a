#pragma once

#include <algorithm>
#include <limits>
#include <random>

#include "directed_graph.hpp"
// #include "greedy_search.hpp"
// #include "robust_prune.hpp"
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

