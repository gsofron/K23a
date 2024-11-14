#include <iostream>
#include <algorithm>

#include "vectors.hpp"
#include "utils.hpp"

// Load vectors from a binary file and initialize cache
Vectors::Vectors(const std::string& file_name, int vectors_dimention, int num_read_vectors, int queries_num) 
    : base_size(0), dimention(vectors_dimention), queries(queries_num) {
    
    std::ifstream file(file_name, std::ios::binary);
    if (!file) throw std::runtime_error("Error opening file: " + file_name);

    int max_vectors;
    if (!file.read(reinterpret_cast<char*>(&max_vectors), sizeof(int))) return;
    max_vectors = std::min(max_vectors, num_read_vectors);
    
    vectors = new float*[max_vectors + queries];
    dist_matrix = new float*[max_vectors + queries];
    filters = new float[max_vectors + queries];

    while (base_size < max_vectors && file) {
        if (!file.read(reinterpret_cast<char*>(&filters[base_size]), sizeof(float))) break;

        float x;
        if (!file.read(reinterpret_cast<char*>(&x), sizeof(float))) break;

        vectors[base_size] = new float[dimention];
        if (!file.read(reinterpret_cast<char*>(vectors[base_size]), dimention * sizeof(float))) {
            throw std::runtime_error("Error reading vector data from file");
        }

        dist_matrix[base_size] = new float[max_vectors]();
        for (int i = 0 ; i < base_size ; i++) {
            euclidean_distance(base_size, i);
        }
        base_size++;
    }

    file.close();
}

// Initialize vectors with generated values and fill cache
Vectors::Vectors(int num_vectors, int queries_num) 
    : base_size(num_vectors), dimention(3), queries(queries_num) {
    
    vectors = new float*[base_size + queries];
    dist_matrix = new float*[base_size + queries];
    filters = new float[base_size + queries];

    for (int i = 0; i < base_size; i++) {
        vectors[i] = new float[dimention];
        dist_matrix[i] = new float[base_size]();
        filters[i] = i % 2;
        for (int j = 0; j < dimention; j++) {
            vectors[i][j] = static_cast<float>(i * 3 + (j + 1)); 
        }
    }

    for (int i = 0; i < base_size; i++) {
        for (int j = i + 1; j < base_size; j++) {
            euclidean_distance(i, j);
        }
    }
}

// Destructor to free allocated memory
Vectors::~Vectors() {
    for (int i = 0; i < base_size + queries; ++i) {
        delete[] vectors[i];
        delete[] dist_matrix[i];
    }
    delete[] vectors;
    delete[] dist_matrix;
    delete[] filters;
}

bool Vectors::same_filter(int index1, int index2) {
    return ((filters[index1] == filters[index2]) || filters[index1] == -1);
}

// Calculate Euclidean distance and update cache
float Vectors::euclidean_distance(int index1, int index2) const {
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

// Load multiple query vectors from a file
void Vectors::read_queries(const std::string& file_name, int read_num) {
    std::ifstream file(file_name, std::ios::binary);
    if (!file) throw std::runtime_error("Error opening file: " + file_name);

    int queries_num;
    if (!file.read(reinterpret_cast<char*>(&queries_num), sizeof(int))) return;
    queries_num = std::min(queries_num, read_num);

    int num_read_vectors = base_size;

    while (num_read_vectors < base_size + queries_num && file) {
        
        float x;
        if (!file.read(reinterpret_cast<char*>(&x), sizeof(float))) break;

        if (!file.read(reinterpret_cast<char*>(&filters[num_read_vectors]), sizeof(float))) break;

        if (!file.read(reinterpret_cast<char*>(&x), sizeof(float))) break;
        if (!file.read(reinterpret_cast<char*>(&x), sizeof(float))) break;

        vectors[num_read_vectors] = new float[dimention];
        dist_matrix[num_read_vectors] = new float[base_size]();
        if (!file.read(reinterpret_cast<char*>(vectors[num_read_vectors]), dimention * sizeof(float))) {
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
void Vectors::add_query(float *values) {
    vectors[base_size] = new float[dimention];
    std::memcpy(vectors[base_size], values, dimention * sizeof(float));

    dist_matrix[base_size] = new float[base_size];
    for (int i = 0 ; i < base_size ; i++) {
        dist_matrix[base_size][i] = euclidean_distance(base_size, i);
    }
}