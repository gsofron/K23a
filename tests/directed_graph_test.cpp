#include "../include/acutest/acutest.h"
#include "directed_graph.hpp"

void test_directed_graph_insert(void) {
    int vector_dimension = 3;
    DirectedGraph<int> *g = new DirectedGraph<int>(vector_dimension);

    // Create 6 random MathVectors
    std::vector<MathVector <int> *> vectors;
    for (int i = 0 ; i < 6 ; i++) {
        MathVector<int> *random_vector = DirectedGraph<int>::random_vector(vector_dimension);
        vectors.push_back(random_vector);
    }

    // Insert them as pairs to the graph
    for (int i = 0 ; i < 6 ; i += 2) {
        g->insert(vectors[i], vectors[i+1]);
        auto neighbors_umap = g->get_umap();

        // Check if vectors[i] was inserted as a key
        auto it = neighbors_umap.find(vectors[i]);
        TEST_CHECK(it != neighbors_umap.end());

        // Check if vectors[i+1] was inserted as a value to the corresponding key
        auto& neighbors_uset = neighbors_umap[vectors[i]];
        auto it2 = neighbors_uset.find(vectors[i+1]);
        TEST_CHECK(it2 != neighbors_uset.end());
    }

    // Try adding more than one neighbor to the first vertex
    for (int i = 2 ; i < 6 ; i++) {
        g->insert(vectors[0], vectors[i]);
        auto neighbors_umap = g->get_umap();

        // Check if vectors[i] was inserted as a value to the corresponding key
        auto& neighbors_uset = neighbors_umap[vectors[0]];
        auto it = neighbors_uset.find(vectors[i]);
        TEST_CHECK(it != neighbors_uset.end());
    }

    // Deallocate memory
    for (int i = 0 ; i < 6 ; i++) {
        delete vectors[i];
    }
    delete g;
}

TEST_LIST = {
    { "test_directed_graph_insert", test_directed_graph_insert },
    { NULL, NULL } // Terminate test list with NULL
};