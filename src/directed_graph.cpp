#include <algorithm>        // std::find()
#include <cstdlib>          // rand()

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

// template <typename T>
// bool DirectedGraph<T>::remove(MathVector<T> *source, MathVector<T> *destination) {
//     ERROR_EXIT(*source == *destination, "Source and destination vertices cannot be the same")
//     // Remove the edge, if present
//     return neighbors[source].erase(destination) > 0;
// }

// template <typename T>
// const std::unordered_set<MathVector<T> *>& DirectedGraph<T>::get_neighbors(MathVector<T> *v) {
//     ERROR_EXIT(neighbors.find(v) == neighbors.end(), "Vector isn't present in the graph") // Assure that 'v' is present in the map
//     return neighbors[v];
// }

// // Creates a random vector with the given dimension
// template <typename T>
// MathVector<T> *DirectedGraph<T>::random_vector(int dimension) {
//     // Get 'dimension' random values ranging from 0 to 9999
//     int *values = new int[dimension];
//     for (int i = 0 ; i < dimension ; i++) {
//         values[i] = rand() % 10000;
//     }
//     // Create the MathVector and return it
//     MathVector<T> *random = new MathVector<T>(dimension, values);
//     delete[] values;
//     return random;
// }

// // Overload '<<' operator to print the graph with ease
// friend std::ostream& operator<<(std::ostream& os, const DirectedGraph& g) {
//     for (const auto& pair : g.neighbors) { // for every MathVector-Neighbors (key-value) pair
//         os << "Vector's " << *pair.first << " KNNs: " << pair.second << "\n";
//     }
//     return os;
// }