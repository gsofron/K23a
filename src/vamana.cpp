#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <random>
#include <string>

#include "greedy_search.hpp"
#include "robust_prune.hpp"
#include "utils.hpp"
#include "vamana.hpp"

// Creates a random R-regular out-degree directed graph
DirectedGraph *random_graph(int num_of_vertices, int R) {
    ERROR_EXIT(R <= 0, "Invalid R")

    // If there are k vertices, each vertex can have at most k-1 neighbours
    if (R > num_of_vertices - 1) R = num_of_vertices-1;

    DirectedGraph *g = new DirectedGraph(num_of_vertices);
    // In each iteration, save vertices that have already been picked
    bool *picked_indexes = new bool[num_of_vertices];
    // Repeat for all vertices
    for (int i = 0, index; i < num_of_vertices; i++) {
        // (Re-)init picked_indexes[] array
        std::fill(picked_indexes, picked_indexes + num_of_vertices, false);
        // Vertex cannot point to itself
        picked_indexes[i] = true;
        // Choose a non-repeating new neighbor randomly
        for (int j = 0; j < R; j++) {
            do { index = rand() % num_of_vertices; } while (picked_indexes[index]);
            picked_indexes[index] = true;
            g->insert(i, index);
        }
    }
    delete[] picked_indexes;

    return g;
}

// Returns the medoid vertex (vector index) of Pf
int medoid(Vectors& vectors, int *Pf, int n) {
    // Simple brute-force algorithm
    float min = std::numeric_limits<float>::max();
    int m = -1;
    for (int i = 0; i < n; i++) {
        float sum = 0.0;
        for (int j = 0; j < n && sum < min; j++) { // Add euclidean distances
            sum += vectors.euclidean_distance(Pf[i], Pf[j]);
        }
        if (sum < min) {
            min = sum;
            m = i;
        }
    }
    return m;
}

// Returns a completely random medoid
int random_medoid(int n) {
    srand(time(NULL));
    return rand() % n;
}

// Returns the medoid of a random subset of Pf
int random_subset_medoid(Vectors& vectors, int *Pf, int n) {
    // Create a copy of Pf and shuffle it
    int *Pf_shuffled = new int[n];
    for (int i = 0 ; i < n ; i++) {
        Pf_shuffled[i] = Pf[i];
    }
    auto rd = std::random_device {}; 
    auto rng = std::default_random_engine { rd() };
    std::shuffle(Pf_shuffled, Pf_shuffled + n, rng);

    // Randomly get the size of the new subset
    srand(time(NULL));
    int subset_size = rand() % n + 1;

    // Get a random subset
    int *Pf_subset = new int[subset_size];
    for (int i = 0 ; i < subset_size ; i++) {
        Pf_subset[i] = Pf_shuffled[i];
    }

    // Find medoid of random subset and return it
    int random_medoid = medoid(vectors, Pf_subset, subset_size);
    delete [] Pf_subset;
    delete [] Pf_shuffled;
    return random_medoid;
}

DirectedGraph *vamana(Vectors& P, int *Pf, int n, float a, int L, int R, bool random_medoid_flag, bool random_subset_medoid_flag, int limit) {
    // Init the R-regular (counting out-degree only) graph
    DirectedGraph *G = random_graph(n, R);
    
    // Init medoid
    int s;
    if (!random_medoid_flag && !random_subset_medoid_flag) s = medoid(P, Pf, n);
    else if (random_medoid_flag) s = random_medoid(n);
    else s = random_subset_medoid(P, Pf, n);

    // Create the random permutation sigma (σ)
    int *sigma = new int[n];
    for (int i = 0; i < n; i++) {
        sigma[i] = i;
    }
    // Shuffle to create the random permutation
    // Source for how to shuffle: https://stackoverflow.com/a/6926473
    auto rd = std::random_device {}; 
    auto rng = std::default_random_engine { rd() };
    std::shuffle(sigma, sigma + n, rng);
    
    for (int i = 0; i < n; i++) {
        auto [Lset, V] = GreedySearch(*G, P, Pf, n, s, sigma[i], 1, L, limit);
        robust_prune(G, P, Pf, sigma[i], V, a, R);

        const auto& N_out_sigma_i = G->get_neighbors(sigma[i]);
        for (auto j : N_out_sigma_i) {
            const auto& N_out_j = G->get_neighbors(j);
            if ((int)N_out_j.size() + 1 > R) {
                std::set<std::pair<float, int>> new_N_out_j;
                for (auto v : N_out_j) {
                    new_N_out_j.insert({P.euclidean_distance(Pf[j], Pf[v]), v});
                }
                new_N_out_j.insert({P.euclidean_distance(Pf[j], Pf[sigma[i]]), sigma[i]});
                robust_prune(G, P, Pf, j, new_N_out_j, a, R);
            } else {
                G->insert(j, sigma[i]);
            }
        }
    }

    delete[] sigma;

    return G;
}

// Writes (stores) a vamana graph into a (binary) file
void write_vamana_to_file(DirectedGraph& g, const std::string& file_name) {
    std::ofstream file(file_name, std::ios::binary);
    if (!file) throw std::runtime_error("Error opening file: " + file_name);

    int num_of_sets = g.get_size();
    // Write the total number of sets the graph has
    file.write(reinterpret_cast<char *>(&num_of_sets), sizeof(num_of_sets));
    for (int i = 0, set_size; i < num_of_sets; i++) {
        const auto& neighbors = g.get_neighbors(i);
        set_size = neighbors.size();
        // Write each set's size (num of indexes/neighbors)
        file.write(reinterpret_cast<char *>(&set_size), sizeof(set_size));
        for (int index : neighbors) {
            // Write the actual data (neighbors of each vertex)
            file.write(reinterpret_cast<char *>(&index), sizeof(index));
        }
    }

    file.close();
}

// Reads (loads) a vamana graph from a (binary) file
// The function acts as a constructor for a vamana graph, returning a pointer to the allocated memory
DirectedGraph *read_vamana_from_file(const std::string& file_name) {
    std::ifstream file(file_name, std::ios::binary);
    if (!file) throw std::runtime_error("Error opening file: " + file_name);

    // Firstly, read the number of sets (vertices) the graph has
    int num_of_sets;
    file.read(reinterpret_cast<char *>(&num_of_sets), sizeof(num_of_sets));

    // Init a graph object and start constructing it based on file's data
    DirectedGraph *g = new DirectedGraph(num_of_sets);
    // For every vertex
    for (int i = 0, set_size; i < num_of_sets; i++) {
        file.read(reinterpret_cast<char *>(&set_size), sizeof(set_size));
        // Load and then store the neighbors (indexes) of each vertex
        for (int j = 0, neighbor_index; j < set_size; j++) {
            file.read(reinterpret_cast<char *>(&neighbor_index), sizeof(neighbor_index));
            g->insert(i, neighbor_index);
        }
    }

    file.close();

    return g;
}