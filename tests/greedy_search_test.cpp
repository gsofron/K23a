#include "acutest.h"             // Acutest unit testing library
#include <vector>
#include <iostream>
#include "vectors.hpp"           // For Vectors class
#include "directed_graph.hpp"     // For DirectedGraph class
#include "greedy_search.hpp"      // For GreedySearch function

// Creates a sample directed graph with sequential and near-sequential edges
DirectedGraph create_graph(int num) {
    DirectedGraph graph(10000);  

    for (int i = 0; i < num - 2; i++) {  
        graph.insert(i, i + 1);  // Sequential edges
        graph.insert(i, i + 2);  // Near-sequential edges
    }
    return graph;
}

// Tests the GreedySearch function for a specific graph structure
void test_greedy_search(void) {
    Vectors<int> vectors(1000, 1);  // Initialize Vectors object for testing
    DirectedGraph graph = create_graph(1000);  // Create test graph with 1000 nodes

    // Define a single query vector
    int query_values[] = {3000, 2000, 1000};
    vectors.add_query(query_values);  // Add query to vectors

    // Expected nearest neighbor sequences for verification
    int s1[] = {1999, 2000, 2001};
    int s2[] = {1996, 1997, 1998};
    int s3[] = {2002, 2003, 2004};
    int s4[] = {1993, 1994, 1995};
    int s5[] = {2005, 2006, 2007};
    
    int k = 5;                    // Number of nearest neighbors to retrieve
    unsigned long L = 500;        // Maximum allowed search steps

    // Execute GreedySearch and store the result
    auto result = GreedySearch(graph, vectors, 0, 1000, k, L);

    // Check each set of results against expected neighbor sequences
    TEST_CHECK(vectors.equal(result.first[0], s1));
    TEST_CHECK(vectors.equal(result.first[1], s2));
    TEST_CHECK(vectors.equal(result.first[2], s3));
    TEST_CHECK(vectors.equal(result.first[3], s4));
    TEST_CHECK(vectors.equal(result.first[4], s5));
}

// List of tests for the test runner
TEST_LIST = {
    { "test_greedy_search", test_greedy_search },
    { NULL, NULL } 
};
