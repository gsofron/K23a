#include "acutest.h"
#include "vamana.hpp"
#include "vector.hpp"

// Used for testing
#define NUM_OF_VECS 1000
#define VECTOR_DIMENSION 3
#define A 1.5f
#define L 20
#define R 3

void test_vamana(void) {
    srand(time(NULL));

    std::vector<MathVector<int> *> vectors;
    for (int i = 0; i < NUM_OF_VECS; i++) {
        vectors.push_back(DirectedGraph<int>::random_vector(VECTOR_DIMENSION));
    }

    // Create the Vamana graph
    DirectedGraph<int> *g = vamana(&vectors, A, L, R);

    // Test the out-degree of each vertex. It should be <= R
    for (auto v : vectors) {
        TEST_CHECK(g->get_neighbors(v).size() <= R);
    }

    for (int i = 0; i < NUM_OF_VECS; i++) {
        delete vectors[i];
    }
    delete g;
}

TEST_LIST = {
    {"test_vamana", test_vamana},
    {NULL, NULL}
};