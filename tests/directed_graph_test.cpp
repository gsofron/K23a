#include "acutest.h"
#include "directed_graph.hpp"

#define NUM_OF_VECS 1000
#define VECTOR_DIMENSION 5

void test_directed_graph_insert(void) {
    DirectedGraph<int> *g = new DirectedGraph<int>(VECTOR_DIMENSION);

    // Create 6 random MathVectors
    std::vector<MathVector <int> *> vectors;
    for (int i = 0 ; i < NUM_OF_VECS ; i++) {
        MathVector<int> *random_vector = DirectedGraph<int>::random_vector(VECTOR_DIMENSION);
        vectors.push_back(random_vector);
    }

    // Insert them as pairs to the graph
    for (int i = 0 ; i < NUM_OF_VECS ; i += 2) {
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
    for (int i = 2 ; i < NUM_OF_VECS ; i++) {
        g->insert(vectors[0], vectors[i]);
        auto neighbors_umap = g->get_umap();

        // Check if vectors[i] was inserted as a value to the corresponding key
        auto& neighbors_uset = neighbors_umap[vectors[0]];
        auto it = neighbors_uset.find(vectors[i]);
        TEST_CHECK(it != neighbors_uset.end());
    }

    // Deallocate memory
    for (int i = 0 ; i < NUM_OF_VECS ; i++) {
        delete vectors[i];
    }
    delete g;
}

void test_directed_graph_remove(void) {
    // Create several random math-vectors and push them into a dynamic array (vector)
    std::vector<MathVector<int> *> vecs;
    for (int i = 0; i < NUM_OF_VECS; i++) {
        vecs.push_back(DirectedGraph<int>::random_vector(VECTOR_DIMENSION));
    }

    // Initialize a directed graph with 3D vectors of ints
    // Insert them in pais as follows: 0->1, 2->3, 4->5, ...
    DirectedGraph<int> *g = new DirectedGraph<int>(VECTOR_DIMENSION);
    for (int i = 0; i < NUM_OF_VECS; i += 2) {
        g->insert(vecs[i], vecs[i+1]);
    }

    // Try to remove non-existent edges that were never inserted
    TEST_CHECK(!g->remove(vecs[0], vecs[4]));
    TEST_CHECK(!g->remove(vecs[1], vecs[0]));

    // Remove every edge an test
    for (int i = 0; i < NUM_OF_VECS; i += 2) {
        TEST_CHECK(g->remove(vecs[i], vecs[i+1]));
        TEST_CHECK(!g->remove(vecs[i], vecs[i+1])); // Try to remove non-existent edges that were previously inserted
    }

    for (int i = 0; i < NUM_OF_VECS; i++) {
        delete vecs[i];
    }
    delete g;
}

TEST_LIST = {
    { "test_directed_graph_insert", test_directed_graph_insert },
    { "test_directed_graph_remove", test_directed_graph_remove },
    { NULL, NULL } // Terminate test list with NULL
};