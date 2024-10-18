#pragma once

#include <algorithm>
#include <cstdlib>
#include <unordered_map>
#include <unordered_set>

#include "vector.hpp"
#include "utils.hpp"

// Directed Graph Implementation using unordered-map and unordered-sets
// Unordered map is used to map vertices(MathVectors) to neighbors(unordered-set of MathVectors)

template <typename T>
class DirectedGraph {
public:
    // Initialize a graph with 'vertex_count' vertices
    //DirectedGraph(int vertex_count);

    // Insert edge ('source', 'destination') to graph
    //void insert(MathVector<T> *source, MathVector<T> *destination);

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

    // Creates a random graph by adding 'count' neighbors to each vertex
    //void random(int count);

private:
    int n; // Maximum amount of vertices
    std::unordered_map<MathVector<T> *, std::unordered_set<MathVector<T> *>> neighbors; // Unordered map representing the adjacency list
};

// // Initialize a graph with 'vertex_count' vertices
// DirectedGraph::DirectedGraph(int vertex_count) {
//     this->n = vertex_count;
//     for (int i = 0 ; i < n ; i++) {
//         this->neighbors[i] = {};
//     }
// }

// // Insert edge ('source', 'destination') to graph
// void DirectedGraph::insert(Vertex source, Vertex destination) {
//     // Check if given vertices are valid
//     ERROR_EXIT(source < 0 || source  > this->n, "Source vertex out of bounds");
//     ERROR_EXIT(destination < 0 || destination  > this->n, "Destination vertex out of bounds");

//     // Vertex cannot point to itself
//     ERROR_EXIT(source == destination, "Vertex cannot point to itself");

//     // We don't allow duplicate edges
//     ERROR_EXIT(std::find(this->neighbors[source].begin(), this->neighbors[source].end(), destination) != \
//     this->neighbors[source].end(), "Edge already exists");

//     this->neighbors[source].push_back(destination);
// }

template <typename T>
void DirectedGraph<T>::remove(MathVector<T> *source, MathVector<T> *destination) {
    ERROR_EXIT(*source == *destination, "Source and destination vertices cannot be the same")
    // Remove the edge, if present
    auto& neighbors_uset = neighbors[source];
    neighbors_uset.erase(destination);
}

// // Creates a random graph by adding 'n' neighbors to each vertex
// void DirectedGraph::random(int count) {
//     ERROR_EXIT(count >= this->n, "Too many neighbors");

//     // Repeat for all vertices
//     for (int i = 0 ; i < this->n ; i++) {
//         // Save vertices that have already been picked
//         int picked[this->n];
//         for (int j = 0 ; j < this->n ; j++) {
//             picked[j] = 0;
//         }
//         // Vector cannot point to itself
//         picked[i] = 1;

//         // Choose a non-repeating new neighbour randomly
//         for (int j = 0 ; j < count ; j++) {
//             int index = rand() % this->n;
//             while (picked[index]) index = rand() % this->n;
//             picked[index] = 1;
//             this->insert(i, index);
//         }
//     }
// }