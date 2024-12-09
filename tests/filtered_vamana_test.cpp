#include "acutest.h"
#include "filtered_vamana.hpp"

// Used for testing
#define NUM_OF_ENTRIES 700
#define A 1.1f
#define L 20
#define R 3
#define T (NUM_OF_ENTRIES / 3)

void test_filtered_vamana(void) {
    // Create random vectors to use as a dataset
    Vectors vectors = Vectors(NUM_OF_ENTRIES, 0);

    // Create the Filtered Vamana graph
    DirectedGraph *g = filtered_vamana(vectors, A, L, R, T);

    // Test the out-degree of each vertex. It should be <= R
    int n = vectors.size();
    for (int i = 0; i < n; i++) {
        TEST_CHECK(g->get_neighbors(i).size() <= R);
    }

    delete g;
}

TEST_LIST = {
    { "test_filtered_vamana", test_filtered_vamana },
    { NULL, NULL }
};