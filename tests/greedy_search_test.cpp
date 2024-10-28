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
    auto vectors = Vectors<int>(10000);
    auto graph = create_graph(10000);

    std::vector<int> query_values{9000, 8000, 7000};
    vectors.add_query(query_values);

    std::vector<int> s1{7999, 8000, 8001};
    std::vector<int> s2{7996, 7997, 7998};
    std::vector<int> s3{8002, 8003, 8004};
    std::vector<int> s4{7993, 7994, 7995};
    std::vector<int> s5{8005, 8006, 8007};
    
    int k = 5; 
    unsigned long L = 10000; 

    auto result = GreedySearch(graph, vectors, 0, 10000, k, L);
  
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
