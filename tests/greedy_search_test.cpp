#include "acutest.h"  
#include <vector>
#include <iostream>
#include "vector.hpp"         
#include "directed_graph.hpp"   
#include "greedy_search.hpp"

std::vector<MathVector<int>*> *generate_vectors() {
    std::vector<MathVector<int>*> *vectors = new std::vector<MathVector<int>*>;
    vectors->reserve(10000);

    for (int i = 0; i < 10000; ++i) {
        int *values = new int[3]; 
        values[0] = i * 3 + 1;
        values[1] = i * 3 + 2;
        values[2] = i * 3 + 3;

        MathVector<int> *vec = new MathVector<int>(3, values);
        vectors->push_back(vec);

        delete[] values;
    }
    return vectors;
}

DirectedGraph<int> create_graph(const std::vector<MathVector<int>*>& vectors) {
    DirectedGraph<int> graph(3);  

    for (int i = 0; i < 9998; i++) {  
        graph.insert(vectors[i], vectors[i + 1]);  
        graph.insert(vectors[i], vectors[i + 2]); 
    }
    return graph;
}

void test_greedy_search(void) {
    auto vectors = generate_vectors();
    DirectedGraph<int> graph = create_graph(*vectors);
 
   
    MathVector<int> *start = vectors->at(0); 
    int query_values[] = {9000, 8000, 7000};  
    MathVector<int> *query = new MathVector<int>(3, query_values);

    int values1[] = {7999, 8000, 8001};
    int values2[] = {7996, 7997, 7998};
    int values3[] = {8002, 8003, 8004};
    int values4[] = {7993, 7994, 7995};
    int values5[] = {8005, 8006, 8007};

    std::vector<MathVector<int>*> solutions;
    solutions.push_back(new MathVector<int>(3, values1));
    solutions.push_back(new MathVector<int>(3, values2));
    solutions.push_back(new MathVector<int>(3, values3));
    solutions.push_back(new MathVector<int>(3, values4));
    solutions.push_back(new MathVector<int>(3, values5));
    
    int k = 5; 
    unsigned long L = 10000; 

    auto result = GreedySearch(graph, start, query, k, L);

    TEST_CHECK(*(result.first[0]) == *(solutions.at(0)));
    TEST_CHECK(*(result.first[1]) == *(solutions.at(1)));
    TEST_CHECK(*(result.first[2]) == *(solutions.at(2)));
    TEST_CHECK(*(result.first[3]) == *(solutions.at(3)));
    TEST_CHECK(*(result.first[4]) == *(solutions.at(4)));

    destroy_vector(vectors);
    delete query;
    for (auto i = 0 ; i < 5 ; i++) {
        delete solutions.at(i);
    }
}

TEST_LIST = {
    { "test_greedy_search", test_greedy_search },
    { NULL, NULL } 
};
