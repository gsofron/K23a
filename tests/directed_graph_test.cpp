#include <algorithm>    // std::random_shuffle
#include <ctime>        // std::time for srand()
#include <vector>       // std::vector

#include "acutest.h"
#include "directed_graph.hpp"

#define NUM_OF_VECS 1000

void test_directed_graph_init(void) {
    // Check if private members initialized correctly
    DirectedGraph *g = new DirectedGraph(NUM_OF_VECS);
    TEST_CHECK(g->get_size() == NUM_OF_VECS);

    // All sets should be empty
    for (int i = 0 ; i < NUM_OF_VECS ; i++) {
        TEST_CHECK(g->get_neighbors(i).empty());
    }
    
    delete g;
}

void test_directed_graph_insert(void) {
    std::srand(unsigned(std::time(0)));

    DirectedGraph *g = new DirectedGraph(NUM_OF_VECS);

    // Store 0, 1, ..., NUM_OF_VECS-1 to a vector and randomly shuffle them
    std::vector<int> random_nums;
    for (int i = 0 ; i < NUM_OF_VECS ; i++) {
        random_nums.push_back(i);
    }
    std::random_shuffle(random_nums.begin(), random_nums.end());
    
    // Insert vertices as pairs
    for (int i = 0 ; i < NUM_OF_VECS ; i += 2) {
        int j = i+1;
        if (j == NUM_OF_VECS) j = 0;

        g->insert(random_nums[i], random_nums[j]);

        // Check if j was inserted as a value to the corresponding key
        auto& neighbors = g->get_neighbors(random_nums[i]);
        auto it = neighbors.find(random_nums[j]);
        TEST_CHECK(it != neighbors.end());
    }

    // Try adding more than one neighbor to the first vertex
    auto& neighbors = g->get_neighbors(random_nums[0]);
    for (int i = 2 ; i < NUM_OF_VECS ; i++) {
        g->insert(random_nums[0], random_nums[i]);

        // Check if i was inserted as a value to the corresponding key
        auto it = neighbors.find(random_nums[i]);
        TEST_CHECK(it != neighbors.end());
    }

    delete g;
}

void test_directed_graph_remove(void) {
    // srand(time(NULL));

    // // Create several random math-vectors and push them into a dynamic array (vector)
    // std::vector<MathVector<int> *> vecs;
    // for (int i = 0; i < NUM_OF_VECS; i++) {
    //     vecs.push_back(DirectedGraph<int>::random_vector(VECTOR_DIMENSION));
    // }

    // // Initialize a directed graph with 3D vectors of ints
    // // Insert them in pais as follows: 0->1, 2->3, 4->5, ...
    // DirectedGraph<int> *g = new DirectedGraph<int>(VECTOR_DIMENSION);
    // for (int i = 0; i < NUM_OF_VECS; i += 2) {
    //     g->insert(vecs[i], vecs[i+1]);
    // }

    // // Try to remove non-existent edges that were never inserted
    // TEST_CHECK(!g->remove(vecs[0], vecs[4]));
    // TEST_CHECK(!g->remove(vecs[1], vecs[0]));

    // // Remove every edge an test
    // for (int i = 0; i < NUM_OF_VECS; i += 2) {
    //     TEST_CHECK(g->remove(vecs[i], vecs[i+1]));
    //     TEST_CHECK(!g->remove(vecs[i], vecs[i+1])); // Try to remove non-existent edges that were previously inserted
    // }

    // for (int i = 0; i < NUM_OF_VECS; i++) {
    //     delete vecs[i];
    // }
    // delete g;
}

TEST_LIST = {
    { "test_directed_graph_init", test_directed_graph_init },
    { "test_directed_graph_insert", test_directed_graph_insert },
    { "test_directed_graph_remove", test_directed_graph_remove },
    { NULL, NULL } // Terminate test list with NULL
};