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
    std::vector<std::vector<T>> vectors;
    int base_size;
public:
    Vectors(const std::string& file_name, int& num_read_vectors, int max_vectors);

    int dimension(int index) const { return vectors[index].size(); }

    const std::vector<T>& operator[](int index) const {
        return vectors[index];
    }

    float euclidean_distance(int index1, int index2) const;

    bool equal(int index1, int index2) const;

    std::vector<int> query_solutions(const std::string& file_name, int query_index);

    void read_queries(const std::string& file_name, int queries_num);
};

template <typename T>
Vectors<T>::Vectors(const std::string& file_name, int& num_read_vectors, int max_vectors) {
    std::ifstream file(file_name, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Error opening file: " + file_name);
    }

    vectors.resize(max_vectors);
    base_size = max_vectors;

    num_read_vectors = 0;
    while (num_read_vectors < max_vectors && file) {
        int dimension;
        if (!file.read(reinterpret_cast<char*>(&dimension), sizeof(int))) {
            break; 
        }

        std::vector<T> items(dimension);
        if (!file.read(reinterpret_cast<char*>(items.data()), dimension * sizeof(T))) {
            throw std::runtime_error("Error reading vector data from file");
        }

        vectors[num_read_vectors] = items;
        num_read_vectors++;
    }

    file.close();
}

template <typename T>
float Vectors<T>::euclidean_distance(int index1, int index2) const {
    float sum = 0.0;
    for (size_t i = 0 ; i < vectors[index1].size() ; i++) {
        float diff = static_cast<float>(vectors[index1][i]) - static_cast<float>(vectors[index2][i]);
        sum += diff * diff;
    }
    return std::sqrt(sum);
}


template <typename T>
bool Vectors<T>::equal(int index1, int index2) const {
    return vectors[index1] == vectors[index2];
}

template <typename T>
std::vector<int> Vectors<T>::query_solutions(const std::string& file_name, int query_index) {
    std::ifstream file(file_name, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Error opening file: " + file_name);
    }

    int dimension;
    if (!file.read(reinterpret_cast<char*>(&dimension), sizeof(int))) {
        throw std::runtime_error("Error reading vector dimension from file.");
    }

    long start_byte = (query_index * ((dimension + 1) * sizeof(int))) + sizeof(int);
    file.seekg(start_byte, std::ios::beg);
    if (!file) {
        throw std::runtime_error("Error seeking to the required byte position.");
    }

    std::vector<int> solution_indices(dimension);
    if (!file.read(reinterpret_cast<char*>(solution_indices.data()), dimension * sizeof(int))) {
        delete solution_indices; 
        throw std::runtime_error("Error reading solution indices from file.");
    }

    file.close();
    return solution_indices;
}

template <typename T>
void Vectors<T>::read_queries(const std::string& file_name, int queries_num) {
    std::ifstream file(file_name, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Error opening file: " + file_name);
    }

    vectors.resize(base_size + queries_num);

    int num_read_vectors = base_size;
    while (num_read_vectors < base_size + queries_num && file) {
        int dimension;
        if (!file.read(reinterpret_cast<char*>(&dimension), sizeof(int))) {
            break; 
        }

        std::vector<T> items(dimension);
        if (!file.read(reinterpret_cast<char*>(items.data()), dimension * sizeof(T))) {
            throw std::runtime_error("Error reading vector data from file");
        }

        vectors[num_read_vectors] = items;
        num_read_vectors++;
    }

    file.close();
}

