#pragma once

#include <algorithm>        // std::find()
#include <cstdlib>          // rand()
#include <unordered_map>    // std::unordered_map etc.
#include <unordered_set>    // std::unordered_set etc.

#include "vector.hpp"
#include "utils.hpp"

// Directed Graph Implementation using unordered-map and unordered-sets
// Unordered map is used to map vertices(MathVectors) to neighbors(unordered-set of MathVectors)

template <typename T>
class DirectedGraph {
public:
    // Create a directed graph with MathVectors as its vertices.
    // All vertices have the same given dimension
    DirectedGraph(int dimension) { this->dimension = dimension; }

    // Insert edge ('source', 'destination') to graph
    void insert(MathVector<T> *source, MathVector<T> *destination);

    // Remove edge ('source', 'destination') from graph if present
    void remove(MathVector<T> *source, MathVector<T> *destination);

    // Overload '<<' operator to print the graph with ease
    friend std::ostream& operator<<(std::ostream& os, const DirectedGraph<T>& dg) {
        for (const auto& pair : dg.neighbors) { // for every MathVector-Neighbors (key-value) pair
            os << "Vector's " << *pair.first << " KNNs: " << pair.second << "\n";
        }
        return os;
    }

    // Returns a reference to an unordered-set that contains the neighbors of vertex 'v'. 'const' is used to prevent data modification
    const std::unordered_set<MathVector<T> *>& get_neighbors(MathVector<T> *v) const { return neighbors[v]; }

    // Creates a random vector with the given dimension
    static MathVector<T> *random_vector(int dimension);
    // Creates a random graph using a vector of MathVectors. Each vertex will have 'n' neighbors
    static DirectedGraph<T> *random_graph(std::vector<MathVector <int> *> vectors, int n);

    // Accessor used for testing
    std::unordered_map<MathVector<T> *, std::unordered_set<MathVector<T> *>> get_umap() { return neighbors; }
private:
    int dimension;      // The dimension of all MathVectors
    std::unordered_map<MathVector<T> *, std::unordered_set<MathVector<T> *>> neighbors; // Unordered map representing the adjacency list
};

// Insert edge ('source', 'destination') to graph
template <typename T>
void DirectedGraph<T>::insert(MathVector<T> *source, MathVector<T> *destination) {
    // All MathVectors in the graph have the same dimension
    ERROR_EXIT((int) source->dimension() != this->dimension, "Source vertex has different dimension");
    ERROR_EXIT((int) destination->dimension() != this->dimension, "Destination vertex has different dimension");

    // Vertex cannot point to itself
    ERROR_EXIT(*source == *destination, "Vertex cannot point to itself");

    // We don't allow duplicate edges
    ERROR_EXIT(std::find(neighbors[source].begin(), neighbors[source].end(), destination) != neighbors[source].end(), "Edge already exists");

    neighbors[source].insert(destination);
}

template <typename T>
void DirectedGraph<T>::remove(MathVector<T> *source, MathVector<T> *destination) {
    ERROR_EXIT(*source == *destination, "Source and destination vertices cannot be the same")
    // Remove the edge, if present
    auto& neighbors_uset = neighbors[source];
    neighbors_uset.erase(destination);
}

// Creates a random vector with the given dimension
template <typename T>
MathVector<T> *DirectedGraph<T>::random_vector(int dimension) {
    // Get 'dimension' random values ranging from 0 to 99
    int *values = new int[dimension];
    for (int i = 0 ; i < dimension ; i++) {
        values[i] = rand() % 100;
    }
    // Create the MathVector and return it
    MathVector<T> *random = new MathVector<T>(dimension, values);
    delete[] values;
    return random;
}

// Creates a random graph using a vector of MathVectors. Each vertex will have 'n' neighbors
template <typename T>
DirectedGraph<T> *DirectedGraph<T>::random_graph(std::vector<MathVector <int> *> vectors, int n) {
    int vector_count = vectors.size();

    // If there are k vertices, each vertex can have at most k-1 neighbours
    ERROR_EXIT(n > vector_count-1, "Too many neighbors");

    DirectedGraph<T> *random_graph = new DirectedGraph<T>(vectors[0]->dimension());
    // Repeat for all vertices
    for (int i = 0 ; i < vector_count ; i++) {
        // Save vertices that have already been picked
        int picked_indexes[vector_count];
        for (int k = 0 ; k < vector_count ; k++) {
            picked_indexes[k] = 0;
        }
        // Vertex cannot point to itself
        picked_indexes[i] = 1;

        // Choose a non-repeating new neighbour randomly
        for (int j = 0 ; j < n ; j++) {
            int index = rand() % vector_count;
            while (picked_indexes[index]) index = rand() % vector_count;
            picked_indexes[index] = 1;

            random_graph->insert(vectors[i], vectors[index]);
        }
    }
    
    return random_graph;
}