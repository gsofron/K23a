#include "acutest.h"
#include "vamana.hpp"

// Used for testing
#define NUM_OF_ENTRIES 1000
#define A 1.5f
#define L 20
#define R 3

void test_vamana(void) {
    // Create random vectors of ints to use as a dataset
    Vectors<int> vectors = Vectors<int>(NUM_OF_ENTRIES);

    // Create the Vamana graph
    DirectedGraph *g = vamana(vectors, A, L, R);

    // Test the out-degree of each vertex. It should be <= R
    int n = vectors.size();
    for (int i = 0; i < n; i++) {
        TEST_CHECK(g->get_neighbors(i).size() <= R);
    }

    delete g;
}

TEST_LIST = {
    { "test_vamana", test_vamana },
    { NULL, NULL }
};