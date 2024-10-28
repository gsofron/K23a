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
    T **vectors;  // Holds all vectors
    int base_size;  // Base vector count
    int dimention;
    int queries;
public:
    Vectors(const std::string& file_name, int& num_read_vectors, int max_vectors, int queries_num);
    Vectors(int num_vectors, int queries_num);
    ~Vectors();

    int size() const { return base_size;}

    int dimension() const { return dimention;}

    // int dimension(int index) const { return 128; }  // Get vector dimension
    T* operator[](int index) const { return vectors[index]; }  // Access vector at index

    float euclidean_distance(int index1, int index2) const;  // Calculate distance between two vectors

    bool equal(int index1, int index2) const { 
        for (auto i = 0 ; i < dimention ; i++) {
            if (vectors[index1][i] != vectors[index2][i]) return false;
        }
        return true; }  // Check equality
    bool equal(int index, T *v) const { 
        for (auto i = 0 ; i < dimention ; i++) {
            if (vectors[index][i] != v[i]) return false;
        }
        return true; }

    std::vector<int> query_solutions(const std::string& file_name, int query_index);  // Load query solutions
    void read_queries(const std::string& file_name, int queries_num);  // Load additional queries
    void add_query(T *values);  // Add a new query vector
};

// Constructor to read vectors from file
template <typename T>
Vectors<T>::Vectors(const std::string& file_name, int& num_read_vectors, int max_vectors, int queries_num) 
    : base_size(0), queries(queries_num){
    
    std::ifstream file(file_name, std::ios::binary);
    if (!file) throw std::runtime_error("Error opening file: " + file_name);

    // Allocate memory for the array of pointers
    vectors = new T*[max_vectors + queries_num];  

    while (base_size < max_vectors && file) {
        int dimension;
        if (!file.read(reinterpret_cast<char*>(&dimension), sizeof(int))) break;

        // Allocate memory for each vector
        vectors[base_size] = new T[dimension];
        if (!file.read(reinterpret_cast<char*>(vectors[base_size]), dimension * sizeof(T))) {
            throw std::runtime_error("Error reading vector data from file");
        }

        dimention = dimension;
        ++base_size;  // Increment the number of vectors read
    }
    num_read_vectors = base_size; // Update the number of read vectors
    file.close();
}

template <typename T>
Vectors<T>::Vectors(int num_vectors, int queries_num) 
    : base_size(num_vectors), dimention(3), queries(queries_num) {
    
    // Allocate memory for the array of pointers with sufficient size
    vectors = new T*[base_size + queries];  

    for (int i = 0; i < num_vectors; ++i) {
        vectors[i] = new T[dimention]; // Allocate memory for each vector
        // Fill the vector with generated values
        for (int j = 0; j < dimention; ++j) {
            vectors[i][j] = static_cast<T>(i * 3 + (j + 1)); // Example initialization
        }
    }
}

template <typename T>
Vectors<T>::~Vectors() {
    for (int i = 0; i < base_size + queries; ++i) {
        delete[] vectors[i]; // Free each vector
    }
    delete[] vectors; // Free the array of pointers
}

// Calculate Euclidean distance between two vectors
template <typename T>
float Vectors<T>::euclidean_distance(int index1, int index2) const {
    float sum = 0.0;
    auto a = vectors[index1];
    auto b = vectors[index2];

    for (auto i = 0; i < dimention; i++) {
        float diff = static_cast<float>(a[i]) - static_cast<float>(b[i]);
        sum += diff * diff;
    }
    return std::sqrt(sum);
}

// Load query solutions from file for a specific query index
template <typename T>
std::vector<int> Vectors<T>::query_solutions(const std::string& file_name, int query_index) {
    std::ifstream file(file_name, std::ios::binary);
    if (!file) throw std::runtime_error("Error opening file: " + file_name);

    int dimension;
    if (!file.read(reinterpret_cast<char*>(&dimension), sizeof(int))) {
        throw std::runtime_error("Error reading vector dimension from file.");
    }

    // Calculate the start byte for the desired query
    long start_byte = (query_index * ((dimension + 1) * sizeof(int))) + sizeof(int);
    file.seekg(start_byte, std::ios::beg);
    if (!file) throw std::runtime_error("Error seeking to the required byte position.");

    std::vector<int> solution_indices(dimension);
    if (!file.read(reinterpret_cast<char*>(solution_indices.data()), dimension * sizeof(int))) {
        throw std::runtime_error("Error reading solution indices from file.");
    }
    file.close();
    return solution_indices;
}

// Load additional queries from a file
template <typename T>
void Vectors<T>::read_queries(const std::string& file_name, int queries_num) {
    std::ifstream file(file_name, std::ios::binary);
    if (!file) throw std::runtime_error("Error opening file: " + file_name);

    int num_read_vectors = base_size;

    while (num_read_vectors < base_size + queries_num && file) {
        int dimension;
        if (!file.read(reinterpret_cast<char*>(&dimension), sizeof(int))) break;

        vectors[num_read_vectors] = new T[dimension];
        if (!file.read(reinterpret_cast<char*>(vectors[num_read_vectors]), dimension * sizeof(T))) {
            throw std::runtime_error("Error reading vector data from file");
        }
        num_read_vectors++;
    }
    file.close();
}

template <typename T>
void Vectors<T>::add_query(T *values) {
    // Allocate memory for a new vector at base_size and copy values into it
    vectors[base_size] = new T[dimention];
    std::memcpy(vectors[base_size], values, dimention * sizeof(T));  
}