#include <algorithm>    // std::random_shuffle
#include <ctime>        // std::time for srand()
#include <vector>       // std::vector

#include "acutest.h"
#include "directed_graph.hpp"

#define NUM_OF_ENTRIES 1000

void test_directed_graph_init(void) {
    // Check if private members initialized correctly
    DirectedGraph *g = new DirectedGraph(NUM_OF_ENTRIES);
    TEST_CHECK(g->get_size() == NUM_OF_ENTRIES);

    // All sets should be empty
    for (int i = 0 ; i < NUM_OF_ENTRIES ; i++) {
        TEST_CHECK(g->get_neighbors(i).empty());
    }
    
    delete g;
}

void test_directed_graph_insert(void) {
    std::srand(unsigned(std::time(0)));

    DirectedGraph *g = new DirectedGraph(NUM_OF_ENTRIES);

    // Store 0, 1, ..., NUM_OF_ENTRIES-1 to a vector and randomly shuffle them
    std::vector<int> random_nums;
    for (int i = 0 ; i < NUM_OF_ENTRIES ; i++) {
        random_nums.push_back(i);
    }
    std::random_shuffle(random_nums.begin(), random_nums.end());
    
    // Insert vertices as pairs
    for (int i = 0 ; i < NUM_OF_ENTRIES ; i += 2) {
        int j = i+1;
        if (j == NUM_OF_ENTRIES) j = 0;

        g->insert(random_nums[i], random_nums[j]);

        // Check if j was inserted as a value to the corresponding key
        auto& neighbors = g->get_neighbors(random_nums[i]);
        auto it = neighbors.find(random_nums[j]);
        TEST_CHECK(it != neighbors.end());
    }

    // Try adding more than one neighbor to the first vertex
    auto& neighbors = g->get_neighbors(random_nums[0]);
    for (int i = 2 ; i < NUM_OF_ENTRIES ; i++) {
        g->insert(random_nums[0], random_nums[i]);

        // Check if i was inserted as a value to the corresponding key
        auto it = neighbors.find(random_nums[i]);
        TEST_CHECK(it != neighbors.end());
    }

    delete g;
}

void test_directed_graph_remove(void) {
    // Create a vector of ints to use as a dataset
    std::vector<int> vecs;
    for (int i = 0; i < NUM_OF_ENTRIES; i++) {
        vecs.push_back(i);
    }

    // Initialize a directed graph
    // Create edges as follows: 0->1, 2->3, 4->5, ...
    DirectedGraph *g = new DirectedGraph(NUM_OF_ENTRIES);
    for (int i = 0; i < NUM_OF_ENTRIES; i += 2) {
        g->insert(vecs[i], vecs[i+1]);
    }

    // Try to remove non-existent edges that were never inserted
    TEST_CHECK(!g->remove(vecs[0], vecs[4]));
    TEST_CHECK(!g->remove(vecs[1], vecs[0]));

    // Remove every edge and test
    for (int i = 0; i < NUM_OF_ENTRIES; i += 2) {
        TEST_CHECK(g->get_neighbors(vecs[i]).size() == 1);

        TEST_CHECK(g->remove(vecs[i], vecs[i+1]));
        TEST_CHECK(g->get_neighbors(vecs[i]).size() == 0);
        
        TEST_CHECK(!g->remove(vecs[i], vecs[i+1])); // Try to remove non-existent edges that were previously inserted
        TEST_CHECK(g->get_neighbors(vecs[i]).size() == 0);
    }

    delete g;
}

void test_directed_graph_get_neighbors(void) {
    // Init a directed graph
    DirectedGraph *g = new DirectedGraph(NUM_OF_ENTRIES);

    // Add multiple neighbors to vertex 0
    int neighbors_count = 0;
    for (int i = 1; i < NUM_OF_ENTRIES; i += 2) {
        const auto& neighbors = g->get_neighbors(0);

        TEST_CHECK(neighbors.find(i) == neighbors.end()); // Neighbor shouldn't exist (yet)
        g->insert(0, i);
        neighbors_count++;
        // Assure that the neighbors' set of vertex 0 increases in size and that each new neighbor-vertex is present
        TEST_CHECK((int)neighbors.size() == neighbors_count);
        TEST_CHECK(neighbors.find(i) != neighbors.end());
    }

    // Remove a neighbor and make sure it doesn't exist in the set anymore
    const auto& neighbors = g->get_neighbors(0);
    int size = neighbors.size();
    TEST_CHECK(neighbors.find(1) != neighbors.end());
    g->remove(0, 1);
    TEST_CHECK(neighbors.find(1) == neighbors.end());
    TEST_CHECK((int)neighbors.size() == size - 1);

    delete g;
}

TEST_LIST = {
    { "test_directed_graph_init", test_directed_graph_init },
    { "test_directed_graph_insert", test_directed_graph_insert },
    { "test_directed_graph_remove", test_directed_graph_remove },
    { "test_directed_graph_get_neighbors", test_directed_graph_get_neighbors },
    { NULL, NULL } // Terminate test list with NULL
};