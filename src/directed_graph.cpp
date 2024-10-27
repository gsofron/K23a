#include <iostream>

#include "directed_graph.hpp"
#include "utils.hpp"

// Create a directed graph with 'num_of_vertices' number of vertices
DirectedGraph::DirectedGraph(int num_of_vertices) {
    neighbors_size = num_of_vertices;
    neighbors = new std::unordered_set<Vertex>[num_of_vertices];
}

// De-allocate memory
DirectedGraph::~DirectedGraph() {
    delete [] neighbors;
}

// Insert edge ('source', 'destination') to graph
void DirectedGraph::insert(Vertex source, Vertex destination) {
    // Check if both vertices are in bounds
    ERROR_EXIT(source < 0 || source >= neighbors_size, "Source vertex is out of bounds");
    ERROR_EXIT(destination < 0 || destination >= neighbors_size, "Destination vertex is out of bounds");

    // Vertex cannot point to itself
    ERROR_EXIT(source == destination, "Vertex cannot point to itself");

    // Insert edge. Insertion won't take place if the edge already exists.
    neighbors[source].insert(destination);
}

bool DirectedGraph::remove(Vertex source, Vertex destination) {
    ERROR_EXIT(source < 0 || source >= neighbors_size, "Invalid index (vertex)")
    ERROR_EXIT(destination < 0 || destination >= neighbors_size, "Invalid index (vertex)")
    ERROR_EXIT(source == destination, "Source and destination vertices cannot be the same")
    // Remove the edge, if present
    return neighbors[source].erase(destination) > 0;
}

const std::unordered_set<Vertex>& DirectedGraph::get_neighbors(Vertex v) {
    ERROR_EXIT(v < 0 || v >= neighbors_size, "Invalid index (vertex)")
    return neighbors[v];
}

// // Overload '<<' operator to print the graph with ease
// std::ostream& operator<<(std::ostream& os, const DirectedGraph& g) {
//     for (int i = 0; i < neighbors_size; i++) {
//         os << "Vector's " << *pair.first << " KNNs: " << pair.second << "\n";
//     }
//     return os;
// }