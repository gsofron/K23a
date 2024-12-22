#pragma once

#include "directed_graph.hpp"
#include "vectors.hpp"

// Creates a random R-regular out-degree directed graph
DirectedGraph *random_graph(int num_of_vertices, int R);

// Returns the medoid vertex (vector index) of Pf
int medoid(Vectors& vectors, int *Pf, int n);

// Vamana Indexing Algorithm implementation using Pf as the database
DirectedGraph *vamana(Vectors& P, int *Pf, int n, float a, int L, int R);

// Writes (stores) a vamana graph into a (binary) file
void write_vamana_to_file(DirectedGraph& g, const std::string& file_name);

// Reads (loads) a vamana graph from a (binary) file
// The function acts as a constructor for a vamana graph, returning a pointer to the allocated memory
DirectedGraph *read_vamana_from_file(const std::string& file_name);