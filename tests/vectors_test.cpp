#include "acutest.h"  
#include "vectors.hpp"
#include <iostream>
#include <vector>                      
#include <cstring>
#include <memory>
#include <iostream>

#define N 10000

void test_vectors_constructor(void) {
    int num_vectors;

    Vectors<float> vectors("siftsmall/siftsmall_base.fvecs", num_vectors, N, 0);
    int vector_dimension = 128;

    const float init_values[] = {
        0.00, 16.00, 35.00, 5.00, 32.00, 31.00, 14.00, 10.00, 11.00, 78.00, 
        55.00, 10.00, 45.00, 83.00, 11.00, 6.00, 14.00, 57.00, 102.00, 
        75.00, 20.00, 8.00, 3.00, 5.00, 67.00, 17.00, 19.00, 26.00, 5.00, 
        0.00, 1.00, 22.00, 60.00, 26.00, 7.00, 1.00, 18.00, 22.00, 84.00, 
        53.00, 85.00, 119.00, 119.00, 4.00, 24.00, 18.00, 7.00, 7.00, 
        1.00, 81.00, 106.00, 102.00, 72.00, 30.00, 6.00, 0.00, 9.00, 
        1.00, 9.00, 119.00, 72.00, 1.00, 4.00, 33.00, 119.00, 29.00, 
        6.00, 1.00, 0.00, 1.00, 14.00, 52.00, 119.00, 30.00, 3.00, 
        0.00, 0.00, 55.00, 92.00, 111.00, 2.00, 5.00, 4.00, 9.00, 
        22.00, 89.00, 96.00, 14.00, 1.00, 0.00, 1.00, 82.00, 59.00, 
        16.00, 20.00, 5.00, 25.00, 14.00, 11.00, 4.00, 0.00, 0.00, 
        1.00, 26.00, 47.00, 23.00, 4.00, 0.00, 0.00, 4.00, 38.00, 
        83.00, 30.00, 14.00, 9.00, 4.00, 9.00, 17.00, 23.00, 41.00, 
        0.00, 0.00, 2.00, 8.00, 19.00, 25.00, 23.00, 1.00
    };

    std::vector<float> expected_vector(init_values, init_values + vector_dimension);

    for (auto i = 0 ; i < 128 ; i++) {
        TEST_CHECK(expected_vector[i] == vectors[0][i]);
    }
}

void test_vectors_euclidean_distance(void) {
    int num_vectors;
    const float EPSILON = 0.001f;

    Vectors<float> vectors("siftsmall/siftsmall_base.fvecs", num_vectors, N, 0);
    
    float distance = vectors.euclidean_distance(0, 1);

    TEST_CHECK(std::fabs(distance - 368.957f) < EPSILON); 
}

TEST_LIST = {
    { "test_vectors_constructor", test_vectors_constructor },
    { "test_vectors_euclidean_distance", test_vectors_euclidean_distance },
    { NULL, NULL } 
};
