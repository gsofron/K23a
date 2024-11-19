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
    float **vectors;             // Stores all vectors
    float **dist_matrix;        // Cache for Euclidean distances
    float *filters;             // Store all filters
    std::unordered_map<float, std::unordered_set<int>> filters_map; // Stores for every filter the indeces they have it
    int base_size;              // Number of vectors
    int dimention;              // Dimension of each vector
    int queries;                // Number of queries

public:
    // Load vectors from a file
    Vectors(const std::string& file_name, int vectors_dimention, int num_read_vectors, int queries_num);

    // Initialize vectors with predefined values (only for testing)
    Vectors(int num_vectors, int queries_num);

    ~Vectors(); // Destructor to clean up allocated memory

    int size() const { return base_size; }
    int dimension() const { return dimention; }
    float* operator[](int index) const { return vectors[index]; }

    // Return all the indeces that has the given filter
    std::unordered_set<int> filter_indeces(float filter);

    // Check if these two vectors has the same filter
    bool same_filter(int index1, int index2); 

    // Calculate Euclidean distance between two vectors
    float euclidean_distance(int index1, int index2) const;

    // Retrieve cached Euclidean distance
    float euclidean_distance_cached(int index1, int index2) const { 
        return dist_matrix[index1][index2];
    }

    // Load queries from a file
    void read_queries(const std::string& file_name, int read_num); 

    // Add a new query vector (only for testing)
    void add_query(float *values); 
}; 
