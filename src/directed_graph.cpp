#include <algorithm>        // std::find()
#include <cstdlib>          // rand()
#include <iostream>

#include "directed_graph.hpp"
#include "utils.hpp"

// // Insert edge ('source', 'destination') to graph
// template <typename T>
// void DirectedGraph<T>::insert(MathVector<T> *source, MathVector<T> *destination) {
//     // All MathVectors in the graph have the same dimension
//     ERROR_EXIT((int) source->dimension() != this->dimension, "Source vertex has different dimension");
//     ERROR_EXIT((int) destination->dimension() != this->dimension, "Destination vertex has different dimension");

//     // Vertex cannot point to itself
//     ERROR_EXIT(*source == *destination, "Vertex cannot point to itself");

//     // Insert edge. Insertion won't take place if the edge already exists. Insertion also adds destination as a vertex
//     neighbors[source].insert(destination);
//     neighbors[destination];
// }

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