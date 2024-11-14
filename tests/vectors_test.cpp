#include "acutest.h"               // Acutest testing framework
#include "vectors.hpp"             // Vectors class for data management

#include <vector>                  // Standard vector library
#include <cstring>
#include <memory>
#include <iostream>
#include <iomanip> 

// Test for Vectors constructor 
void test_vectors_constructor(void) {
    Vectors vectors("dummy/dummy-data.bin", 100, 100, 0);  // Initialize Vectors from file

    // Expected values for the first vector
    const float init_values[] = {
        2.15454, -3.02897, 0.698244, -0.415859, 2.10826, -0.423406, -0.265021,
        1.48845, -0.635659, -0.322034, 0.288786, 1.50423, 0.60534, -0.0444409,
        1.72059, 0.847015, -0.307805, 0.759683, 1.29945, -0.181562, 0.463562,
        -0.193746, 0.649952, -1.15263, 0.048541, -0.627288, 0.398051, 0.0511783,
        0.545563, 0.0495269, 0.0687726, -0.631042, 0.0826486, 0.731219, -0.226549,
        0.215328, -0.148697, 0.413595, 0.0921035, 0.458715, -0.253046, -0.0336558,
        -0.337412, -0.170269, 0.313249, 0.498627, 0.464264, 0.672622, 0.725523,
        0.003511, -0.345974, 0.498223, -0.345793, 0.161103, -0.00469103, 0.0290951,
        -0.28487, -0.35776, -0.37224, -0.371342, 0.515504, 0.162517, 0.0253236,
        0.560636, 0.49534, 0.166332, -0.346384, 0.416897, 0.250396, -0.552659,
        0.50076, 0.157336, -0.0468045, -0.460763, 0.540153, 0.0880996, -0.259423,
        -0.285234, 0.442767, 0.17183, 0.0573628, -0.44357, -0.247626, 0.135093,
        0.238867, 0.00215004, -0.139578, -0.425506, 0.164526, 0.342528, 0.432775,
        -0.433014, -0.301049, -0.487173, 0.0114285, 0.386267, 0.16443, 0.0441171,
        0.269416, -0.487473
    };

    // Verify the first vector values in 'vectors' match expected values
    const float epsilon = 1e-5;
    for (int i = 0; i < 100; i++) {
        TEST_CHECK(std::abs(init_values[i] - vectors[0][i]) < epsilon);
    }
}

void test_vectors_size(void) {
    Vectors vectors100(100, 0);  
    TEST_CHECK(vectors100.size() == 100); 

    Vectors vectors50(50, 0);  
    TEST_CHECK(vectors50.size() == 50); 
}

void test_vectors_dimension(void) {
    Vectors vectors(100, 0);  
    TEST_CHECK(vectors.dimension() == 3); 
}

// Test for Vectors read queries
void test_vectors_read_queries(void) {
    Vectors vectors("dummy/dummy-data.bin", 100, 100, 100);  // Initialize Vectors from file
    vectors.read_queries("dummy/dummy-queries.bin", 100);

    // Expected values for the first query vector
    const float init_values[] = {
        0.0892586, -0.150291, -0.821048, 0.344949, -0.589355, 0.944352, -0.667804,
        0.305992, -2.13121, 1.73406, -0.949898, 2.06183, 0.537979, 0.328789, -0.0176721,
        -0.562069, -0.621625, 0.840873, 0.591859, 0.275412, 0.937918, -0.0616842,
        0.809946, -1.05696, 1.19051, -0.0321672, -0.369298, 0.679628, -1.07731, 0.390322,
        -0.454751, -0.32243, -0.615295, 1.18967, 0.721884, 0.104513, -0.0959466, 0.0282482,
        0.0742209, 0.341131, 0.257043, 0.0100733, -0.0585112, -0.417733, -0.613679, 
        0.805337, -0.203679, 0.155731, 0.184432, -0.253783, 0.399551, 0.321999, 
        -0.440549, -0.303851, 0.0522588, 0.177387, 0.334058, 0.197412, 0.00448788, 
        0.990222, 0.494329, -0.448112, 0.206554, -0.730439, 0.170579, 0.0226756, 
        0.0322761, 0.00679316, -0.324175, -0.556499, 0.353944, -0.355755, -0.542616, 
        -0.244421, 0.439767, -0.309919, -0.0356684, 0.200207, -0.387314, -0.357522, 
        0.0969972, 0.615141, 0.361333, 0.208315, 0.0530467, 0.395783, 0.294819, 
        0.558363, -0.487409, 0.526882, 0.497473, 0.17107, 0.0201734, -0.179488, 
        0.0901327, -0.490539, -0.358719, -0.236438, 0.0852151, -0.542886
    };

    const float epsilon = 1e-5;
    for (int i = 0; i < 100; i++) {
        TEST_CHECK(std::abs(init_values[i] - vectors[100][i]) < epsilon);
    }
}

void test_vectors_same_filter(void) {
    Vectors vectors(100, 0);  

    TEST_CHECK(vectors.same_filter(2, 4)); 

    TEST_CHECK(vectors.same_filter(15, 23)); 
}

// Test for Vectors' Euclidean distance function
void test_vectors_euclidean_distance(void) {
    Vectors vectors(100, 0);  

    // Check that the Euclidean distance between vector 0 and vector 1 is 27
    TEST_CHECK(vectors.euclidean_distance(0, 1) == 27); 
}

// List of test functions for the test runner
TEST_LIST = {
    { "test_vectors_constructor", test_vectors_constructor },
    { "test_vectors_size", test_vectors_size},
    { "test_vectors_dimension", test_vectors_dimension},
    { "test_vectors_read_queries", test_vectors_read_queries},
    { "test_vectors_same_filter", test_vectors_same_filter},
    { "test_vectors_euclidean_distance", test_vectors_euclidean_distance },
    { NULL, NULL } 
};