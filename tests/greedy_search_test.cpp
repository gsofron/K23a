#include "acutest.h"         
#include <vector>
#include <iostream> 
#include "greedy_search.hpp"      

// Creates a sample directed graph with sequential and near-sequential edges
DirectedGraph create_graph(int num) {
    DirectedGraph graph(1000);  

    for (int i = 0; i < num - 2; i++) {  
        graph.insert(i, i + 1);  
        graph.insert(i, i + 2);  
    }
    return graph;
}

// Tests the GreedySearch function for a specific graph structure
void test_greedy_search(void) {
    Vectors vectors(1000, 1);  // Initialize Vectors object for testing
    DirectedGraph graph = create_graph(1000);  // Create test graph with 1000 nodes

    // Define a single query vector
    float query_values[] = {3000, 2000, 1000};
    vectors.add_query(query_values);  // Add query to vectors
    
    int k = 5;                    // Number of nearest neighbors to retrieve
    unsigned long L = 10;        // Maximum allowed search steps

    // Execute GreedySearch and store the result
    auto result = GreedySearch(graph, vectors, 0, 1000, k, L);

    // Check results' validity
    TEST_CHECK(result.first[0] == 666);
    TEST_CHECK(result.first[1] == 665);
    TEST_CHECK(result.first[2] == 667);
    TEST_CHECK(result.first[3] == 664);
    TEST_CHECK(result.first[4] == 668);
}

// List of tests for the test runner
TEST_LIST = {
    { "test_greedy_search", test_greedy_search },
    { NULL, NULL } 
};
