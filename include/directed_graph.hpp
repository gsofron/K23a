#pragma once

#include <unordered_set>    // std::unordered_set etc.

// Representation of a Vertex is an index (int)
typedef int Vertex;

// Directed Graph Implementation using an array of unordered-sets
// The graph stores indexes of math-vectors to the array (vector) contatining the actual data
// Unordered sets are used to store the indexes of each vertex's neighbors
class DirectedGraph {
public:
    // Create a directed graph with 'num_of_vertices' number of vertices
    DirectedGraph(int num_of_vertices);

    // De-allocate memory
    ~DirectedGraph();

    // Insert edge ('source', 'destination') to graph
    void insert(Vertex source, Vertex destination);

    // Remove edge ('source', 'destination') from graph if present
    // Returns true/false based on the removal's success
    bool remove(Vertex source, Vertex destination);

    // Stitch another graph to the existing graph. This is done by unionizing their edge sets
    void stitch(DirectedGraph *g);

    // Returns a reference to an unordered-set that contains the neighbors of vertex 'v'. 'const' is used to prevent data modification
    const std::unordered_set<Vertex>& get_neighbors(Vertex v) const;

    // Size accessor
    int get_size() const { return neighbors_size; }

private:
    // Array of sets. Each set contains the neighbors of the vector in the corresponding array index
    // Example: Neighbors of vector with index 4 are in neighbors[4] unordered set
    // Neighbors are also stored in index-format
    std::unordered_set<Vertex> *neighbors;
    
    // 'neighbors' array size (num of rows/vertices)
    int neighbors_size;
};