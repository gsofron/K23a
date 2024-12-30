#include <iostream>
#include <algorithm>
#include <immintrin.h>

#include "vectors.hpp"
#include "utils.hpp"

// Load vectors from a binary file and initialize cache
Vectors::Vectors(const std::string& file_name, int vectors_dimention, int num_read_vectors, int queries_num) 
    : base_size(0), dimention(vectors_dimention), queries(queries_num) {
    
    std::ifstream file(file_name, std::ios::binary);
    if (!file) throw std::runtime_error("Error opening file: " + file_name);

    // Read the number of vectors
    u_int32_t u_max_vectors;
    if (!file.read(reinterpret_cast<char*>(&u_max_vectors), sizeof(u_max_vectors))) return;
    // Keep the smaller number so it is controllable the number of vectors that  will be read
    int max_vectors = std::min(static_cast<int>(u_max_vectors), num_read_vectors);
    
    vectors = new float*[max_vectors + queries];
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

        base_size++;
    }

    file.close();
}

// Initialize vectors with generated values and fill cache
Vectors::Vectors(int num_vectors, int queries_num) 
    : base_size(num_vectors), dimention(3), queries(queries_num) {
    
    vectors = new float*[base_size + queries];
    filters = new float[base_size + queries]();

    for (int i = 0; i < base_size; i++) {
        vectors[i] = new float[dimention];
        filters[i] = i % 2;
        filters_map[filters[i]].insert(i);
        for (int j = 0; j < dimention; j++) {
            vectors[i][j] = static_cast<float>(i * 3 + (j + 1)); 
        }
    }
}

// Destructor to free allocated memory
Vectors::~Vectors() {
    for (int i = 0; i < base_size + queries; i++)
        delete[] vectors[i];
    delete[] vectors;
    delete[] filters;
}

// Calculate Euclidean distance between two vectors
float Vectors::euclidean_distance(int index1, int index2) {
    const float *a = vectors[index1];
    const float *b = vectors[index2];

    __m256 sum_vec = _mm256_setzero_ps(); // Accumulator for the sum of squared differences
    int i;
    for (i = 0; i <= 92; i += 8) {
        __m256 vec_a = _mm256_loadu_ps(a + i);      // Load 8 floats from vector a
        __m256 vec_b = _mm256_loadu_ps(b + i);      // Load 8 floats from vector b
        __m256 diff = _mm256_sub_ps(vec_a, vec_b);  // Compute a[i]-b[i]
        __m256 sq_diff = _mm256_mul_ps(diff, diff); // Square the differences
        sum_vec = _mm256_add_ps(sum_vec, sq_diff);
    }
    // Sum the SIMD register into a single sum variable
    float sum_array[8];
    _mm256_storeu_ps(sum_array, sum_vec);
    float sum = 0.0;
    for (int j = 0; j < 8; j++)
        sum += sum_array[j];
    // Handle the remaining (possible) elements
    for (; i < 100; i++) {
        float diff = a[i] - b[i];
        sum += diff * diff;
    }

    return sum;
}

// Load multiple query vectors from a file
void Vectors::read_queries(const std::string& file_name, int read_num) {
    std::ifstream file(file_name, std::ios::binary);
    if (!file) throw std::runtime_error("Error opening file: " + file_name);

    // Read the number of queries
    u_int32_t u_queries_num;
    if (!file.read(reinterpret_cast<char*>(&u_queries_num), sizeof(u_queries_num))) return;
    int queries_num = std::min(static_cast<int>(u_queries_num), read_num);

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
        if (!file.read(reinterpret_cast<char*>(vectors[num_read_vectors]), dimention * sizeof(float))) {
            throw std::runtime_error("Error reading vector data from file");
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
    if (!file.read(reinterpret_cast<char*>(vectors[base_size]), dimention * sizeof(float))) {
        throw std::runtime_error("Error reading vector data from file");
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
}