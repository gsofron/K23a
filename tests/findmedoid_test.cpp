#include "acutest.h"
#include "findmedoid.hpp"

void test_find_medoid(void) {
    /* The testing-constructor of the Vectors class creates vectors using
       the following deterministic formula: vectors[i][j] = i * 3 + j + 1,
       where the i-th row contains the vector of (const) dimesion d=3
       and j is the corresponding value in each dimension. */
    Vectors vectors = Vectors(50, 0);
    /* Two filters are used (0, 1) storing even and odd i-numbers respectively.
       Having created 50 vectors, their values are:
       vectors[0] = [1, 2, 3], vectors[1] = [4, 5, 6], vectors[2] = [7, 8, 9], ...
       Even numbered i (0, 2, 4, ...) are a part of filter 0 and odd numbered of filter 1. */

    auto *M = find_medoid(vectors, 50 / 3); // 50/3 is a random threshold < 25=50/2 = Num of vectors per filter
    
    // Assure that there are 2 filters, as already highlighted
    TEST_CHECK(M->size() == 2);

    // For filter 0, assure that the medoid is chosen from filter's vectors (indexes)
    int m = M->at(0);
    TEST_CHECK(m % 2 == 0 && m >= 0 && m < 50);
    // For filter 1, assure that the medoid is chosen from filter's vectors (indexes)
    m = M->at(1);
    TEST_CHECK(m % 2 == 1 && m > 0 && m < 50);

    delete M;
}

TEST_LIST = {
    { "test_find_medoid", test_find_medoid },
    { NULL, NULL }
};