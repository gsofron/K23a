#pragma once

#include <iostream>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <fstream>
#include <string>
#include <cstring>

template <typename T>
class Vectors {
private:
    T **vectors;                // Stores all vectors
    float **dist_matrix;        // Cache for Euclidean distances
    int base_size;              // Number of vectors
    int dimention;              // Dimension of each vector
    int queries;                // Number of queries

public:
    // Load vectors from a file
    Vectors(const std::string& file_name, int& num_read_vectors, int max_vectors, int queries_num);

    // Initialize vectors with predefined values
    Vectors(int num_vectors, int queries_num);

    ~Vectors(); // Destructor to clean up allocated memory

    int size() const { return base_size; }
    int dimension() const { return dimention; }
    T* operator[](int index) const { return vectors[index]; }

    // Calculate Euclidean distance between two vectors
    float euclidean_distance(int index1, int index2) const;

    // Retrieve cached Euclidean distance
    float euclidean_distance_cached(int index1, int index2) const { 
        return dist_matrix[index1][index2];
    }

    // Check equality between two vectors by index
    bool equal(int index1, int index2) const; 

    // Check equality between vector and values
    bool equal(int index, T *v) const; 

    // Get k-nearest neighbors for a query
    std::vector<int> query_solutions(const std::string& file_name, int query_index);  

    // Load queries from a file
    void read_queries(const std::string& file_name, int queries_num); 

    // Add a new query vector
    void add_query(T *values); 
};

// Load vectors from a binary file and initialize cache
template <typename T>
Vectors<T>::Vectors(const std::string& file_name, int& num_read_vectors, int max_vectors, int queries_num) 
    : base_size(0), queries(queries_num) {
    
    std::ifstream file(file_name, std::ios::binary);
    if (!file) throw std::runtime_error("Error opening file: " + file_name);

    vectors = new T*[max_vectors + queries];
    dist_matrix = new float*[max_vectors + queries];

    while (base_size < max_vectors && file) {
        int dimension;
        if (!file.read(reinterpret_cast<char*>(&dimension), sizeof(int))) break;

        vectors[base_size] = new T[dimension];
        if (!file.read(reinterpret_cast<char*>(vectors[base_size]), dimension * sizeof(T))) {
            throw std::runtime_error("Error reading vector data from file");
        }

        dist_matrix[base_size] = new float[max_vectors]();
        dimention = dimension;
        base_size++;
    }
    num_read_vectors = base_size;
    file.close();
}

// Initialize vectors with generated values and fill cache
template <typename T>
Vectors<T>::Vectors(int num_vectors, int queries_num) 
    : base_size(num_vectors), dimention(3), queries(queries_num) {
    
    vectors = new T*[base_size + queries];
    dist_matrix = new float*[base_size + queries];

    for (int i = 0; i < base_size; i++) {
        vectors[i] = new T[dimention];
        dist_matrix[i] = new float[base_size]();

        for (int j = 0; j < dimention; j++) {
            vectors[i][j] = static_cast<T>(i * 3 + (j + 1)); 
        }
    }

    for (int i = 0; i < base_size; i++) {
        for (int j = i + 1; j < base_size; j++) {
            euclidean_distance(i, j);
        }
    }
}

// Destructor to free allocated memory
template <typename T>
Vectors<T>::~Vectors() {
    for (int i = 0; i < base_size + queries; ++i) {
        delete[] vectors[i];
        delete[] dist_matrix[i];
    }
    delete[] vectors;
    delete[] dist_matrix;
}

// Calculate Euclidean distance and update cache
template <typename T>
float Vectors<T>::euclidean_distance(int index1, int index2) const {
    float sum = 0.0, diff;
    auto& a = vectors[index1];
    auto& b = vectors[index2];

    for (auto i = 0; i < dimention; i++) {
        diff = static_cast<float>(a[i]) - static_cast<float>(b[i]);
        sum += diff * diff;
    }

    if (index1 >= base_size || index2 >= base_size) return sum;
    dist_matrix[index1][index2] = dist_matrix[index2][index1] = sum;

    return dist_matrix[index1][index2];
}

// Check equality of two vectors by index
template <typename T>
bool Vectors<T>::equal(int index1, int index2) const { 
    for (auto i = 0 ; i < dimention ; i++) {
        if (vectors[index1][i] != vectors[index2][i]) return false;
    }
    return true; 
}  

// Check equality of a vector with specific values
template <typename T>
bool Vectors<T>::equal(int index, T *v) const { 
    for (auto i = 0 ; i < dimention ; i++) {
        if (vectors[index][i] != v[i]) return false;
    }
    return true; 
}

// Load k-nearest neighbors for a query from file
template <typename T>
std::vector<int> Vectors<T>::query_solutions(const std::string& file_name, int query_index) {
    std::ifstream file(file_name, std::ios::binary);
    if (!file) throw std::runtime_error("Error opening file: " + file_name);

    int dimension;
    if (!file.read(reinterpret_cast<char*>(&dimension), sizeof(int))) {
        throw std::runtime_error("Error reading vector dimension from file.");
    }

    long start_byte = (query_index * ((dimension + 1) * sizeof(int))) + sizeof(int);
    file.seekg(start_byte, std::ios::beg);
    if (!file) throw std::runtime_error("Error seeking to required byte position.");

    std::vector<int> solution_indices(dimension);
    if (!file.read(reinterpret_cast<char*>(solution_indices.data()), dimension * sizeof(int))) {
        throw std::runtime_error("Error reading solution indices from file.");
    }
    file.close();
    return solution_indices;
}

// Load multiple query vectors from a file
template <typename T>
void Vectors<T>::read_queries(const std::string& file_name, int queries_num) {
    std::ifstream file(file_name, std::ios::binary);
    if (!file) throw std::runtime_error("Error opening file: " + file_name);

    int num_read_vectors = base_size;

    while (num_read_vectors < base_size + queries_num && file) {
        int dimension;
        if (!file.read(reinterpret_cast<char*>(&dimension), sizeof(int))) break;

        vectors[num_read_vectors] = new T[dimension];
        dist_matrix[num_read_vectors] = new float[base_size]();
        if (!file.read(reinterpret_cast<char*>(vectors[num_read_vectors]), dimension * sizeof(T))) {
            throw std::runtime_error("Error reading vector data from file");
        }

        for (int i = 0 ; i < base_size ; i++) {
            dist_matrix[num_read_vectors][i] = euclidean_distance(num_read_vectors, i);
        }
        num_read_vectors++;
    }
    file.close();
}

// Add a single query vector and update distance cache
template <typename T>
void Vectors<T>::add_query(T *values) {
    vectors[base_size] = new T[dimention];
    std::memcpy(vectors[base_size], values, dimention * sizeof(T));

    dist_matrix[base_size] = new float[base_size];
    for (int i = 0 ; i < base_size ; i++) {
        dist_matrix[base_size][i] = euclidean_distance(base_size, i);
    }
} 
