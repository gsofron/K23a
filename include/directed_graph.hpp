#pragma once

#include <unordered_map>
#include <vector>

// Directed Graph Implementation using hashmaps and vectors
// Unordered maps are used to map vertices(integers) to neighbors(integer vectors)
// Graphs are 0-indexed, meaning a graph with 10 vertices has vertices labeled from 0 to 9

typedef int Vertex;

class DirectedGraph {
public:
    // Initialize a graph with 'vertex_count' vertices
    DirectedGraph(int vertex_count);
    // Insert edge ('source', 'destination') to graph
    void insert(Vertex source, Vertex destination);
    // Remove edge ('source', 'destination') from graph
    void remove(Vertex source, Vertex destination);
    // Print the graph
    void print() const;
    // Returns a reference to the neighbors of vertex 'v'
    std::vector<Vertex> get_neighbors(Vertex v) const;
    // Creates a random graph by adding 'count' neighbors to each vertex
    void random(int count);

private:
    int n;                                                          // Maximum amount of vertices
    std::unordered_map<Vertex, std::vector<Vertex>> neighbors;      // Unordered map representing the adjacency list
};