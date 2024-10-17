#include <algorithm>    // std::find()
#include <cstdlib>      // rand()
#include <iostream>     // std::cout

#include "directed_graph.hpp"
#include "utils.hpp"

// Initialize a graph with 'vertex_count' vertices
DirectedGraph::DirectedGraph(int vertex_count) {
    this->n = vertex_count;
    for (int i = 0 ; i < n ; i++) {
        this->neighbors[i] = {};
    }
}

// Insert edge ('source', 'destination') to graph
void DirectedGraph::insert(Vertex source, Vertex destination) {
    // Check if given vertices are valid
    ERROR_EXIT(source < 0 || source  > this->n, "Source vertex out of bounds");
    ERROR_EXIT(destination < 0 || destination  > this->n, "Destination vertex out of bounds");

    // Vertex cannot point to itself
    ERROR_EXIT(source == destination, "Vertex cannot point to itself");

    // We don't allow duplicate edges
    ERROR_EXIT(std::find(this->neighbors[source].begin(), this->neighbors[source].end(), destination) != \
    this->neighbors[source].end(), "Edge already exists");

    this->neighbors[source].push_back(destination);
}

// Creates a random graph by adding 'n' neighbors to each vertex
void DirectedGraph::random(int count) {
    ERROR_EXIT(count >= this->n, "Too many neighbors");

    // Repeat for all vertices
    for (int i = 0 ; i < this->n ; i++) {
        // Save vertices that have already been picked
        int picked[this->n];
        for (int j = 0 ; j < this->n ; j++) {
            picked[j] = 0;
        }
        // Vector cannot point to itself
        picked[i] = 1;

        // Choose a non-repeating new neighbour randomly
        for (int j = 0 ; j < count ; j++) {
            int index = rand() % this->n;
            while (picked[index]) index = rand() % this->n;
            picked[index] = 1;
            this->insert(i, index);
        }
    }
}