#include "acutest.h"
#include "vamana.hpp"

// Used for testing
#define NUM_OF_ENTRIES 700
#define A 1.1f
#define L 20
#define R 3

void test_vamana(void) {
    // Create random vectors of ints to use as a dataset
    Vectors<int> vectors = Vectors<int>(NUM_OF_ENTRIES, 0);

    // Create the Vamana graph
    DirectedGraph *g = vamana(vectors, A, L, R);

    // Test the out-degree of each vertex. It should be <= R
    int n = vectors.size();
    for (int i = 0; i < n; i++) {
        TEST_CHECK(g->get_neighbors(i).size() <= R);
    }

    delete g;
}

void test_read_and_write_file(void) {
    // Create a random vamana graph and test the read_from and write_to functions
    Vectors<int> vectors = Vectors<int>(NUM_OF_ENTRIES, 0);
    DirectedGraph *g1 = vamana(vectors, A, L, R);

    const std::string file_name = "build/test_vamana_file";

    // Write the graph to the file
    write_vamana_to_file(*g1, file_name);

    // Load the graph from the file into a new object
    DirectedGraph *g2 = read_vamana_from_file(file_name);

    // Check data coherence
    int g1_size = g1->get_size(), g2_size = g2->get_size();
    TEST_CHECK(g1_size == g2_size);
    for (int i = 0; i < g1_size; i++) {
        const auto& g1_neighbors = g1->get_neighbors(i);
        int g1_n_size = g1_neighbors.size();

        const auto& g2_neighbors = g2->get_neighbors(i);
        int g2_n_size = g2_neighbors.size();

        TEST_CHECK(g1_n_size == g2_n_size);

        // Assure that same vertices have the same neighbors
        for (auto it1 = g1_neighbors.begin(); it1 != g1_neighbors.end(); it1++) {
            // Search needs to happen using find(), because the sets are unordered (hash tables)
            TEST_CHECK(g2_neighbors.find(*it1) != g2_neighbors.end());
        }
    }

    delete g1;
    delete g2;
}

TEST_LIST = {
    { "test_vamana", test_vamana },
    { "test_read_and_write_file", test_read_and_write_file },
    { NULL, NULL }
};