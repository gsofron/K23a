#include "acutest.h"
#include "robust_prune.hpp"

#define NUM_OF_VECS 100
#define VECTOR_DIMENSION 128
#define R 10
#define A 1.5

// General test for nearest_point_in_set
void test_nearest_point_in_set(void) {
    srand(time(NULL));

    // Create random vectors
    std::vector<MathVector <int> *> vectors;
    for (int i = 0 ; i < NUM_OF_VECS ; i++) {
        MathVector<int> *random_vector = DirectedGraph<int>::random_vector(VECTOR_DIMENSION);
        vectors.push_back(random_vector);
    }

    // If an empty set is given, nearest_point_in_set should return nullptr
    TEST_CHECK(nearest_point_in_set(vectors[0], {}) == nullptr);

    // For all vectors
    for (int i = 0 ; i < NUM_OF_VECS ; i++) {
        // Get the set containing all vertices except the current one
        std::unordered_set<MathVector<int> *> s(vectors.begin(), vectors.end());
        s.erase(vectors[i]);

        // Get nearest point and distance
        MathVector<int> *nearest = nearest_point_in_set(vectors[i], s);
        float nearest_distance = vectors[i]->euclidean_distance(*nearest);


        // Make sure no other points are closer
        for (int j = 0 ; j < NUM_OF_VECS ; j++) {
            if (i == j) continue;

            TEST_CHECK(vectors[i]->euclidean_distance(*vectors[j]) >= nearest_distance);
        }
    }
}

// General test for robust_prune
void test_robust_prune_general(void) {
    srand(time(NULL));

    // Create random vectors
    std::vector<MathVector <int> *> vectors;
    for (int i = 0 ; i < NUM_OF_VECS ; i++) {
        MathVector<int> *random_vector = DirectedGraph<int>::random_vector(VECTOR_DIMENSION);
        vectors.push_back(random_vector);
    }

    // Create a graph where each vertex points to all other vertices
    DirectedGraph<int> *g = DirectedGraph<int>::random_graph(vectors, NUM_OF_VECS-1);

    // Convert graph into an R-regular graph
    for (int i = 0 ; i < NUM_OF_VECS ; i++) {
        robust_prune(g, vectors[i], {}, A, R);

        // Check if robust prune successfully returned no more than R neighbors
        const auto& neighbors = g->get_neighbors(vectors[i]);
        TEST_CHECK(neighbors.size() <= R);
    }

    // Deallocate memory
    for (int i = 0 ; i < NUM_OF_VECS ; i++) {
        delete vectors[i];
    }
    delete g;
}

// Test case: V = {}
void test_robust_prune_empty_set(void) {
    srand(time(NULL));

    // Create random vectors
    std::vector<MathVector <int> *> vectors;
    for (int i = 0 ; i < NUM_OF_VECS ; i++) {
        MathVector<int> *random_vector = DirectedGraph<int>::random_vector(VECTOR_DIMENSION);
        vectors.push_back(random_vector);
    }

    // Create a graph where each vertex points to all other vertices
    DirectedGraph<int> *g = DirectedGraph<int>::random_graph(vectors, NUM_OF_VECS-1);

    // Repeat for all vertices
    for (int i = 0 ; i < NUM_OF_VECS ; i++) {
        // Even though an empty set is given, Robust Prune should prune all neighbors except from the actual
        // nearest neighbor of the current vertex. This happens because each vertex points to all other vertices
        robust_prune(g, vectors[i], {}, A, 1);

        // Check if robust prune successfully returned only 1 neighbor
        const auto& neighbors = g->get_neighbors(vectors[i]);
        TEST_CHECK(neighbors.size() == 1);

        // Get the set containing all vertices except the current one
        std::unordered_set<MathVector<int> *> s(vectors.begin(), vectors.end());
        s.erase(vectors[i]);

        // Find the actual nearest point
        MathVector<int> *nearest = nearest_point_in_set(vectors[i], s);

        // Check if this is the only neighbor of current vertex
        TEST_CHECK(neighbors.find(nearest) != neighbors.end());
    }

    // Deallocate memory
    for (int i = 0 ; i < NUM_OF_VECS ; i++) {
        delete vectors[i];
    }
    delete g;
}

// Test case: V = set containing all vertices except p
void test_robust_prune_full_set(void) {
    srand(time(NULL));

    // Create random vectors
    std::vector<MathVector <int> *> vectors;
    for (int i = 0 ; i < NUM_OF_VECS ; i++) {
        MathVector<int> *random_vector = DirectedGraph<int>::random_vector(VECTOR_DIMENSION);
        vectors.push_back(random_vector);
    }

    // Create a graph where each vertex points to only one random vertex
    DirectedGraph<int> *g = DirectedGraph<int>::random_graph(vectors, 1);

    // Repeat for all vertices
    for (int i = 0 ; i < NUM_OF_VECS ; i++) {
        // Get the set containing all vertices except the current one
        std::unordered_set<MathVector<int> *> s(vectors.begin(), vectors.end());
        s.erase(vectors[i]);
        
        // Even though each vertex only has one random neighbor, Robust Prune should replace it with the
        // actual nearest neighbor of the current vertex since set 's' contains all other vertices
        robust_prune(g, vectors[i], s, A, 1);

        // Check if robust prune successfully returned only 1 neighbor
        const auto& neighbors = g->get_neighbors(vectors[i]);
        TEST_CHECK(neighbors.size() == 1);

        // Find the actual nearest point
        MathVector<int> *nearest = nearest_point_in_set(vectors[i], s);

        // Check if this is the only neighbor of current vertex
        TEST_CHECK(neighbors.find(nearest) != neighbors.end());
    }

    // Deallocate memory
    for (int i = 0 ; i < NUM_OF_VECS ; i++) {
        delete vectors[i];
    }
    delete g;
}

TEST_LIST = {
    { "test_nearest_point_in_set", test_nearest_point_in_set },
    { "test_robust_prune_general", test_robust_prune_general },
    { "test_robust_prune_empty_set", test_robust_prune_empty_set },
    { "test_robust_prune_full_set", test_robust_prune_full_set },
    { NULL, NULL } // Terminate test list with NULL
};
