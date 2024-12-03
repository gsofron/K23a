#include <iostream>
#include <algorithm>

#include "vectors.hpp"
#include "utils.hpp"

// Load vectors from a binary file and initialize cache
Vectors::Vectors(const std::string& file_name, int vectors_dimention, int num_read_vectors, int queries_num) 
    : base_size(0), dimention(vectors_dimention), queries(queries_num) {
    
    std::ifstream file(file_name, std::ios::binary);
    if (!file) throw std::runtime_error("Error opening file: " + file_name);

    // Read the number of vectors
    int max_vectors;
    if (!file.read(reinterpret_cast<char*>(&max_vectors), sizeof(int))) return;
    // Keep the smaller number so it is controllable the number of vectors that  will be read
    max_vectors = std::min(max_vectors, num_read_vectors);
    
    vectors = new float*[max_vectors + queries];
    dist_matrix = new float*[max_vectors + queries];
    filters = new float[max_vectors + queries];

    while (base_size < max_vectors && file) {
        if (!file.read(reinterpret_cast<char*>(&filters[base_size]), sizeof(float))) break;
        filters_map[filters[base_size]].insert(base_size);

        // Ignore the timestamp value
        file.seekg(sizeof(float), std::ios::cur);

        // Read the vectors values
        vectors[base_size] = new float[dimention];
        if (!file.read(reinterpret_cast<char*>(vectors[base_size]), dimention * sizeof(float))) {
            throw std::runtime_error("Error reading vector data from file");
        }

        // Initialise the euclidean distances of all the vectors
        dist_matrix[base_size] = new float[max_vectors]();
        base_size++;
    }

    for (int i = 0; i < base_size; i++) {
        for (int j = i + 1; j < base_size; j++) {
            euclidean_distance(i, j);
        }
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
        filters_map[filters[i]].insert(i);
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

    // Read the number of queries
    int queries_num;
    if (!file.read(reinterpret_cast<char*>(&queries_num), sizeof(int))) return;
    queries_num = std::min(queries_num, read_num);

    int num_read_vectors = base_size;

    while (num_read_vectors < base_size + queries_num && file) {
        // Determine the type of the query
        float type;
        if (!file.read(reinterpret_cast<char*>(&type), sizeof(float))) break;
        if (type > 1) { // Ignore it if it is a timestamp query
            file.seekg(103*sizeof(float), std::ios::cur);
            continue;
        }

        if (!file.read(reinterpret_cast<char*>(&filters[num_read_vectors]), sizeof(float))) break;

        // Ignore the timestamp related values
        file.seekg(2*sizeof(float), std::ios::cur);

        // Read the queries' values
        vectors[num_read_vectors] = new float[dimention];
        dist_matrix[num_read_vectors] = new float[base_size]();
        if (!file.read(reinterpret_cast<char*>(vectors[num_read_vectors]), dimention * sizeof(float))) {
            throw std::runtime_error("Error reading vector data from file");
        }

        if (!type) { // Initialise euclidean distance from query to every other base vector 
            for (int i = 0 ; i < base_size ; i++) {
                dist_matrix[num_read_vectors][i] = euclidean_distance(num_read_vectors, i);
            }
        } else { // Initialise the euclidean distance from the query to every other vector that has the same filter
            for (int i = 0 ; i < base_size ; i++) {
                if (filters[num_read_vectors] == filters[i]) {
                    dist_matrix[num_read_vectors][i] = euclidean_distance(num_read_vectors, i);
                }
            }
        }
        num_read_vectors++;
    }
    file.close();
}

// Load the query of the given index from a file
bool Vectors::read_query(const std::string& file_name, int index) {
    std::ifstream file(file_name, std::ios::binary);
    if (!file) throw std::runtime_error("Error opening file: " + file_name);
    
    int queries_num;
    if (!file.read(reinterpret_cast<char*>(&queries_num), sizeof(int))) return false;
    if (index < queries_num) {
        file.seekg((104 * index)*sizeof(float), std::ios::cur);
    } else {
        return false;
    }

    float type;
    if (!file.read(reinterpret_cast<char*>(&type), sizeof(float))) return false;
    if (type > 1) { // Ignore it if it is a timestamp query
        return false;
    }

    if (!file.read(reinterpret_cast<char*>(&filters[base_size]), sizeof(float))) return false;

    // Ignore the timestamp related values
    file.seekg(2*sizeof(float), std::ios::cur);

    // Read the queries' values
    vectors[base_size] = new float[dimention];
    dist_matrix[base_size] = new float[base_size]();
    if (!file.read(reinterpret_cast<char*>(vectors[base_size]), dimention * sizeof(float))) {
        throw std::runtime_error("Error reading vector data from file");
    }

    if (!type) { // Initialise euclidean distance from query to every other base vector 
        for (int i = 0 ; i < base_size ; i++) {
            dist_matrix[base_size][i] = euclidean_distance(base_size, i);
        }
    } else { // Initialise the euclidean distance from the query to every other vector that has the same filter
        for (int i = 0 ; i < base_size ; i++) {
            if (filters[base_size] == filters[i]) {
                dist_matrix[base_size][i] = euclidean_distance(base_size, i);
            }
        }
    }
    
    file.close();
    return true;
}

// Return the indices of the k-nearest neighboors of the given index 
std::vector<int> Vectors::query_solutions(const std::string& file_name, int query_index) {
    std::ifstream file(file_name, std::ios::binary);
    if (!file) throw std::runtime_error("Error opening file: " + file_name);

    int dimension = 100;
    long start_byte = query_index * (100 * sizeof(int));
    file.seekg(start_byte, std::ios::beg);
    if (!file) throw std::runtime_error("Error seeking to required byte position.");

    std::vector<int> solution_indices(dimension);
    if (!file.read(reinterpret_cast<char*>(solution_indices.data()), dimension * sizeof(int))) {
        throw std::runtime_error("Error reading solution indices from file.");
    }
    file.close();
    return solution_indices;
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