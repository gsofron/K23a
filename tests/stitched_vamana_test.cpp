#include "acutest.h"
#include "vamana.hpp"
#include "stitched_vamana.hpp"

// Used for testing
#define NUM_OF_ENTRIES 1000
#define A 1.1f
#define L 20
#define R_SMALL 12
#define R_STITCHED 24


void test_stitched_vamana(void) {
    // Create random vectors to use as a dataset
    Vectors vectors = Vectors(NUM_OF_ENTRIES, 0);

    // Create the Vamana graph
    DirectedGraph *g = stitched_vamana(vectors, A, L, R_SMALL, R_STITCHED, false, false, false);

    // Test the out-degree of each vertex. It should be <= R
    int n = vectors.size();
    for (int i = 0; i < n; i++) {
        TEST_CHECK(g->get_neighbors(i).size() <= R_STITCHED);
    }

    delete g;
}

TEST_LIST = {
    { "test_stitched_vamana", test_stitched_vamana },
    { NULL, NULL }
};