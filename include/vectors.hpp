#pragma once

#include <iostream>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <fstream>
#include <string>
#include <cstring>
#include <unordered_set>
#include <unordered_map>

class Vectors {
private:
    float **vectors;            // Stores all vectors
    u_int32_t base_size;              // Number of vectors
    int dimention;              // Dimension of each vector
    u_int32_t queries;                // Number of queries

public:
    float *filters;             // Store all filters
    std::unordered_map<float, std::unordered_set<int>> filters_map; // Stores for every filter the indeces they have it

    // Load vectors from a file
    Vectors(const std::string& file_name, int vectors_dimention, u_int32_t num_read_vectors, u_int32_t queries_num);

    // Initialize vectors with predefined values (only for testing)
    Vectors(int num_vectors, int queries_num);

    ~Vectors(); // Destructor to clean up allocated memory

    int size() const { return base_size; }
    int dimension() const { return dimention; }
    float* operator[](int index) const { return vectors[index]; }

    // Check if these two vectors has the same filter
    bool same_filter(int index1, int index2) {
        return ((filters[index1] == filters[index2]) || filters[index1] == -1 || filters[index2] == -1);
    }

    // Calculate Euclidean distance between two vectors
    float euclidean_distance(int index1, int index2);

    // Load queries from a file
    void read_queries(const std::string& file_name, u_int32_t read_num); 

    // Load the query of the given index from a file
    bool read_query(const std::string& file_name, int index); 

    // Return the indices of the k-nearest neighboors of the given index
    std::vector<int> query_solutions(const std::string& file_name, int query_index);

    // Add a new query vector (only for testing)
    void add_query(float *values); 
}; 
