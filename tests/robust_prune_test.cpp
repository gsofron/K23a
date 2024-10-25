#include <limits>   // float max

#include "acutest.h"
#include "robust_prune.hpp"
#include "vamana.hpp"

#define NUM_OF_VECS 1000
#define VECTOR_DIMENSION 5
#define R 3
#define A 1.5f

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
    DirectedGraph<int> *g = random_graph(vectors, NUM_OF_VECS-1);

    // Convert graph into an R-regular graph
    for (int i = 0 ; i < NUM_OF_VECS ; i++) {
        CompareDistance<int> comparator(vectors[i]); 
        std::set<MathVector<int>*, CompareDistance<int>> empty(comparator);
        robust_prune(g, vectors[i], empty, A, R);

        // Check if Robust Prune successfully returned no more than R neighbors
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
    DirectedGraph<int> *g = random_graph(vectors, NUM_OF_VECS-1);

    // Repeat for all vertices
    for (int i = 0 ; i < NUM_OF_VECS ; i++) {
        // Even though an empty set is given, Robust Prune should prune all neighbors except from the actual
        // nearest neighbor of the current vertex. This happens because each vertex points to all other vertices
        CompareDistance<int> comparator(vectors[i]); 
        std::set<MathVector<int>*, CompareDistance<int>> empty(comparator);
        robust_prune(g, vectors[i], empty, A, 1);

        // Check if robust prune successfully returned only 1 neighbor
        const auto& neighbors = g->get_neighbors(vectors[i]);
        TEST_CHECK(neighbors.size() == 1);
        MathVector<int> *neighbor = *neighbors.begin();

        // Find the actual nearest point
        std::set<MathVector<int>*, CompareDistance<int>> s(comparator);
        for (auto v : vectors) {
            s.insert(v);
        }
        // First point of set s is vectors[i], so the actual nearest point is the second one
        MathVector<int> *nearest = *(++s.begin());

        // Check if they are the same. If not, they should at least have the same distance
        float dist1 = vectors[i]->euclidean_distance(*neighbor);
        float dist2 = vectors[i]->euclidean_distance(*nearest);

        TEST_CHECK(*neighbor == *nearest || dist1 == dist2);
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
    DirectedGraph<int> *g = random_graph(vectors, 1);

    // Repeat for all vertices
    for (int i = 0 ; i < NUM_OF_VECS ; i++) {
        // Get the set containing all vertices
        CompareDistance<int> comparator(vectors[i]); 
        std::set<MathVector<int>*, CompareDistance<int>> s(comparator);
        for (auto v : vectors) {
            s.insert(v);
        }
        
        // Even though each vertex only has one random neighbor, Robust Prune should replace it with the
        // actual nearest neighbor of the current vertex since set 's' contains all vertices
        robust_prune(g, vectors[i], s, A, 1);

        // Check if robust prune successfully returned only 1 neighbor
        const auto& neighbors = g->get_neighbors(vectors[i]);
        TEST_CHECK(neighbors.size() == 1);
        MathVector<int> *neighbor = *neighbors.begin();

        // s was passed by reference in robust_prune, so rebuild it
        s.clear();
        for (auto v : vectors) {
            s.insert(v);
        }
        // First point of set s is vectors[i], so the actual nearest point is the second one
        MathVector<int> *nearest = *(++s.begin());

        // Check if they are the same. If not, they should at least have the same distance
        float dist1 = vectors[i]->euclidean_distance(*neighbor);
        float dist2 = vectors[i]->euclidean_distance(*nearest);

        TEST_CHECK(*neighbor == *nearest || dist1 == dist2);
    }

    // Deallocate memory
    for (int i = 0 ; i < NUM_OF_VECS ; i++) {
        delete vectors[i];
    }
    delete g;
}

TEST_LIST = {
    { "test_robust_prune_general", test_robust_prune_general },
    { "test_robust_prune_empty_set", test_robust_prune_empty_set },
    { "test_robust_prune_full_set", test_robust_prune_full_set },
    { NULL, NULL } // Terminate test list with NULL
};
