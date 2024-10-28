#pragma once

#include <iostream>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <fstream>
#include <string>

template <typename T>
class Vectors {
private:
    std::vector<std::vector<T>> vectors;  // Holds all vectors
    int base_size;  // Base vector count

public:
    Vectors(const std::string& file_name, int& num_read_vectors, int max_vectors);
    Vectors(int num_vectors);

    int size() const { return base_size;}

    int dimension(int index) const { return vectors[index].size(); }  // Get vector dimension
    const std::vector<T>& operator[](int index) const { return vectors[index]; }  // Access vector at index

    float euclidean_distance(int index1, int index2) const;  // Calculate distance between two vectors

    bool equal(int index1, int index2) const { return vectors[index1] == vectors[index2]; }  // Check equality
    bool equal(int index, std::vector<T> v) const { return vectors[index] == v; }

    std::vector<int> query_solutions(const std::string& file_name, int query_index);  // Load query solutions
    void read_queries(const std::string& file_name, int queries_num);  // Load additional queries
    void add_query(std::vector<T> values);  // Add a new query vector
};

// Constructor to read vectors from file
template <typename T>
Vectors<T>::Vectors(const std::string& file_name, int& num_read_vectors, int max_vectors) {
    std::ifstream file(file_name, std::ios::binary);
    if (!file) throw std::runtime_error("Error opening file: " + file_name);

    vectors.resize(max_vectors);
    base_size = max_vectors;
    num_read_vectors = 0;

    while (num_read_vectors < max_vectors && file) {
        int dimension;
        if (!file.read(reinterpret_cast<char*>(&dimension), sizeof(int))) break;

        std::vector<T> items(dimension);
        if (!file.read(reinterpret_cast<char*>(items.data()), dimension * sizeof(T))) {
            throw std::runtime_error("Error reading vector data from file");
        }
        vectors[num_read_vectors++] = items;
    }
    file.close();
}

// Constructor to initialize with generated vectors
template <typename T>
Vectors<T>::Vectors(int num_vectors) {
    vectors.resize(num_vectors);
    base_size = num_vectors;

    for (int i = 0; i < num_vectors; ++i) {
        vectors[i] = { i * 3 + 1, i * 3 + 2, i * 3 + 3 };
    }
}

// Calculate Euclidean distance between two vectors
template <typename T>
float Vectors<T>::euclidean_distance(int index1, int index2) const {
    float sum = 0.0;
    for (size_t i = 0; i < vectors[index1].size(); i++) {
        float diff = static_cast<float>(vectors[index1][i]) - static_cast<float>(vectors[index2][i]);
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

    vectors.resize(base_size + queries_num);
    int num_read_vectors = base_size;

    while (num_read_vectors < base_size + queries_num && file) {
        int dimension;
        if (!file.read(reinterpret_cast<char*>(&dimension), sizeof(int))) break;

        std::vector<T> items(dimension);
        if (!file.read(reinterpret_cast<char*>(items.data()), dimension * sizeof(T))) {
            throw std::runtime_error("Error reading vector data from file");
        }
        vectors[num_read_vectors++] = items;
    }
    file.close();
}

// Add a new query vector to the list
template <typename T>
void Vectors<T>::add_query(std::vector<T> values) {
    vectors.resize(base_size + 1);
    vectors[base_size++] = values;
}