#include "acutest.h"  
#include <vector>
#include <iostream>
#include "vectors.hpp"         
#include "directed_graph.hpp"  
#include "greedy_search.hpp"


DirectedGraph create_graph(int num) {
    DirectedGraph graph(10000);  

    for (int i = 0; i < num-2; i++) {  
        graph.insert(i, i + 1);  
        graph.insert(i, i + 2); 
    }
    return graph;
}

void test_greedy_search(void) {
    auto vectors = Vectors<int>(1000, 1); 
    auto graph = create_graph(1000);

    int query_values[] = {3000, 2000, 1000};
    vectors.add_query(query_values);
  
    int s1[] = {1999, 2000, 2001};
    int s2[] = {1996, 1997, 1998};
    int s3[] = {2002, 2003, 2004};
    int s4[] = {1993, 1994, 1995};
    int s5[] = {2005, 2006, 2007};
    
    int k = 5; 
    unsigned long L = 500; 
    auto result = GreedySearch(graph, vectors, 0, 1000, k, L);
   
    TEST_CHECK(vectors.equal(result.first[0], s1));
    TEST_CHECK(vectors.equal(result.first[1], s2));
    TEST_CHECK(vectors.equal(result.first[2], s3));
    TEST_CHECK(vectors.equal(result.first[3], s4));
    TEST_CHECK(vectors.equal(result.first[4], s5));
}

TEST_LIST = {
    { "test_greedy_search", test_greedy_search },
    { NULL, NULL } 
};
