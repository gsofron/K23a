#include <ctime>                // std::time for srand()
#include <limits>               // float max

#include "acutest.h"
#include "robust_prune.hpp"
#include "vamana.hpp"           // random_graph()

#define NUM_OF_VECS 100
#define R 3
#define A 1.5f

// General test for robust_prune
void test_robust_prune_general(void) {
    std::srand(unsigned(std::time(0)));

    // Get some vectors
    auto vectors = Vectors(NUM_OF_VECS, 0);

    // Create a random graph where each vertex points to all other vertices
    DirectedGraph *g = random_graph(NUM_OF_VECS, NUM_OF_VECS-1);

    // Construct Pf to include all points
    int Pf[NUM_OF_VECS];
    for (int i = 0 ; i < NUM_OF_VECS ; i++) {
        Pf[i] = i;
    }

    // Convert graph into an R-regular graph
    for (int i = 0 ; i < NUM_OF_VECS ; i++) {
        std::set<std::pair<float, int>> empty;
        robust_prune(g, vectors, Pf, i, empty, A, R);

        // Check if Robust Prune successfully returned no more than R neighbors
        const auto& neighbors = g->get_neighbors(i);
        TEST_CHECK(neighbors.size() <= R);
    }

    // Deallocate memory
    delete g;
}

// Test case: V = {}
void test_robust_prune_empty_set(void) {
    std::srand(unsigned(std::time(0)));

    // Get some vectors
    auto vectors = Vectors(NUM_OF_VECS, 0);

    // Create a random graph where each vertex points to all other vertices
    DirectedGraph *g = random_graph(NUM_OF_VECS, NUM_OF_VECS-1);

    // Construct Pf to include all points
    int Pf[NUM_OF_VECS];
    for (int i = 0 ; i < NUM_OF_VECS ; i++) {
        Pf[i] = i;
    }

    // Repeat for all vertices
    for (int i = 0 ; i < NUM_OF_VECS ; i++) {
        // Even though an empty set is given, Robust Prune should prune all neighbors except from the actual
        // nearest neighbor of the current vertex. This happens because each vertex points to all other vertices
        std::set<std::pair<float, int>> empty;
        robust_prune(g, vectors, Pf, i, empty, A, 1);

        // Check if robust prune successfully returned only 1 neighbor
        const auto& neighbors = g->get_neighbors(i);
        TEST_CHECK(neighbors.size() == 1);
        int neighbor = *neighbors.begin();

        // Find the actual nearest point
        std::set<std::pair<float, int>> s;
        for (int j = 0 ; j < NUM_OF_VECS ; j++) {
            s.insert({vectors.euclidean_distance(i, j), j});
        }

        // Since s is sorted with respect to i, first point of s is 'i' with euclidean_distance(i,i) = 0.0, 
        // so the actual nearest point is the one found in the second position
        int nearest = (++s.begin())->second;

        // Check if they are the same. If not, they should at least have the same distance
        float dist1 = vectors.euclidean_distance(i, neighbor);
        float dist2 = vectors.euclidean_distance(i, nearest);

        TEST_CHECK(neighbor == nearest || dist1 == dist2);
    }

    // Deallocate memory
    delete g;
}

// Test case: V = set containing all vertices except p
void test_robust_prune_full_set(void) {
    std::srand(unsigned(std::time(0)));

    // Get some vectors
    auto vectors = Vectors(NUM_OF_VECS, 0);

    // Create a graph where each vertex points to only one random vertex
    DirectedGraph *g = random_graph(NUM_OF_VECS, 1);

    // Construct Pf to include all points
    int Pf[NUM_OF_VECS];
    for (int i = 0 ; i < NUM_OF_VECS ; i++) {
        Pf[i] = i;
    }

    // Repeat for all vertices
    for (int i = 0 ; i < NUM_OF_VECS ; i++) {
        // Get the set containing all vertices
        std::set<std::pair<float, int>> s;
        for (int j = 0 ; j < NUM_OF_VECS ; j++) {
            s.insert({vectors.euclidean_distance(i, j), j});
        }
        // Since s is passed by reference and is modified, create copy s2
        std::set<std::pair<float, int>> s2 = s;
        
        // Even though each vertex only has one random neighbor, Robust Prune should replace it with the
        // actual nearest neighbor of the current vertex since set 's' contains all vertices
        robust_prune(g, vectors, Pf, i, s2, A, 1);

        // Check if robust prune successfully returned only 1 neighbor
        const auto& neighbors = g->get_neighbors(i);
        TEST_CHECK(neighbors.size() == 1);
        int neighbor = *neighbors.begin();
        
        // Since s is sorted with respect to i, first point of s is 'i' with euclidean_distance(i,i) = 0.0, 
        // so the actual nearest point is the one found in the second position
        int nearest = (++s.begin())->second;

        // Check if they are the same. If not, they should at least have the same distance
        float dist1 = vectors.euclidean_distance(i, neighbor);
        float dist2 = vectors.euclidean_distance(i, nearest);

        TEST_CHECK(neighbor == nearest || dist1 == dist2);
    }

    // Deallocate memory
    delete g;
}

TEST_LIST = {
    { "test_robust_prune_general", test_robust_prune_general },
    { "test_robust_prune_empty_set", test_robust_prune_empty_set },
    { "test_robust_prune_full_set", test_robust_prune_full_set },
    { NULL, NULL } // Terminate test list with NULL
};
