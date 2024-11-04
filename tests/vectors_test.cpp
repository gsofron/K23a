#include "acutest.h"               // Acutest testing framework
#include "vectors.hpp"             // Vectors class for data management
#include <vector>                  // Standard vector library
#include <cstring>
#include <memory>
#include <iostream>

#define N 10000                    // Maximum number of vectors

// Test for Vectors constructor 
void test_vectors_constructor(void) {
    int num_vectors;
    Vectors<float> vectors("siftsmall/siftsmall_base.fvecs", num_vectors, N, 0);  // Initialize Vectors from file
    int vector_dimension = 128;

    // Expected values for the first vector
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

    // Verify the first vector values in 'vectors' match expected values
    for (int i = 0; i < vector_dimension; i++) {
        TEST_CHECK(init_values[i] == vectors[0][i]);
    }
}

// Test for Vectors query solutions
void test_vectors_query_solutions(void) {
    int num_vectors;
    Vectors<float> vectors("siftsmall/siftsmall_base.fvecs", num_vectors, N, 0);

    std::vector<int> indeces = vectors.query_solutions("siftsmall/siftsmall_groundtruth.ivecs", 0);

    // Expected values for the k-nearest neighbors indices of the first query
    const int solutions[] = {
        2176, 3752, 882, 4009, 2837, 190, 3615, 816, 1045, 1884, 224,
        3013, 292, 1272, 5307, 4938, 1295, 492, 9211, 3625, 1254,
        1292, 1625, 3553, 1156, 146, 107, 5231, 1995, 9541, 3543,
        9758, 9806, 1064, 9701, 4064, 2456, 2763, 3237, 1317, 3530,
        641, 1710, 8887, 4263, 1756, 598, 370, 2776, 121, 4058,
        7245, 1895, 124, 8731, 696, 4320, 4527, 4050, 2648, 1682,
        2154, 1689, 2436, 2005, 3210, 4002, 2774, 924, 6630, 3449,
        9814, 3515, 5375, 287, 1038, 4096, 4094, 942, 4321, 123,
        3814, 97, 4293, 420, 9734, 1916, 2791, 149, 6139, 9576,
        6837, 2952, 3138, 2890, 3066, 2852, 348, 3043, 3687
    };

    for (auto i = 0 ; i < 100 ; i++) {
        TEST_CHECK(solutions[i] == indeces[i]);
    }
}

// Test for Vectors read queries
void test_vectors_read_queries(void) {
    int num_vectors;
    Vectors<float> vectors("siftsmall/siftsmall_base.fvecs", num_vectors, N, 100);  // Initialize Vectors from file
    vectors.read_queries("siftsmall/siftsmall_query.fvecs", 100);
    int vector_dimension = 128;

    // Expected values for the first query vector
    const float vector[] = {
        1, 3, 11, 110, 62, 22, 4, 0, 43, 21, 22,
        18, 6, 28, 64, 9, 11, 1, 0, 0, 1,
        40, 101, 21, 20, 2, 4, 2, 2, 9, 18,
        35, 1, 1, 7, 25, 108, 116, 63, 2, 0,
        0, 11, 74, 40, 101, 116, 3, 33, 1, 1,
        11, 14, 18, 116, 116, 68, 12, 5, 4, 2,
        2, 9, 102, 17, 3, 10, 18, 8, 15, 67,
        63, 15, 0, 14, 116, 80, 0, 2, 22, 96,
        37, 28, 88, 43, 1, 4, 18, 116, 51, 5,
        11, 32, 14, 8, 23, 44, 17, 12, 9, 0,
        0, 19, 37, 85, 18, 16, 104, 22, 6, 2,
        26, 12, 58, 67, 82, 25, 12, 2, 2, 25,
        18, 8, 2, 19, 42, 48, 11
    };

    // Verify the first query vector values in 'vectors' match expected values
    for (int i = 0; i < vector_dimension; i++) {
        TEST_CHECK(vector[i] == vectors[N][i]);
    }
}

// Test for Vectors' Euclidean distance function
void test_vectors_euclidean_distance(void) {
    Vectors<float> vectors(100, 0);  

    // Check that the Euclidean distance between vector 0 and vector 1 is 27
    TEST_CHECK(vectors.euclidean_distance(0, 1) == 27); 
}

// List of test functions for the test runner
TEST_LIST = {
    { "test_vectors_constructor", test_vectors_constructor },
    { "test_vectors_query_solutions", test_vectors_query_solutions},
    { "test_vectors_read_queries", test_vectors_read_queries},
    { "test_vectors_euclidean_distance", test_vectors_euclidean_distance },
    { NULL, NULL } 
};