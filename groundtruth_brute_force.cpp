#define K 100
#define VEC_DIMENSION 100

#include <fstream>
#include <iostream>
#include <set>

#include "vectors.hpp"

// Write the true top k nearest neighbors to the file
void write_true_knn(std::ofstream &groundtruth_file, Vectors &vectors, int data_vecs_num, int query_array_index, int query_bin_index) {
    // Write the index of the query in the .bin file first
    groundtruth_file.write(reinterpret_cast<const char*>(&query_bin_index), sizeof(query_bin_index));

    // Save the pair (euclidean distance, index) of all neighbors in ascending eclidean distance
    std::set<std::pair<float, int>> s;
    for (int i = 0 ; i < data_vecs_num ; i++) {
        float dist = vectors.euclidean_distance_cached(query_array_index, i);
        if (dist == 0.0) continue;  // We don't calculate the distance between vectors with a different filter
        
        std::pair<float, int> p(dist, i);
        s.insert(p);
    }

    // If there are less than K neighbors, pad with -1
    int size = s.size();
    if (size < K) {
        for (auto p : s) {
            groundtruth_file.write(reinterpret_cast<const char*>(&p.second), sizeof(p.second));
        }
        for (int i = 0 ; i < K - size ; i++) {
            int x = -1;
            groundtruth_file.write(reinterpret_cast<const char*>(&x), sizeof(x));
        }
    }
    // Else, write the K nearest
    else {
        int count = 0;
        for (auto p : s ) {
            groundtruth_file.write(reinterpret_cast<const char*>(&p.second), sizeof(p.second));
            if (++count == K) break;
        }
    }
}

// ./groundtruth ./dummy/dummy-data.bin ./dummy/dummy-queries.bin ./dummy/dummy-groundtruth.bin
int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <data vectors> <query vectors> <groundtruth filename>" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Read number of data vectors
    int data_vecs_num;
    std::ifstream data_file(argv[1], std::ios::binary);
    data_file.read(reinterpret_cast<char*>(&data_vecs_num), sizeof(data_vecs_num));
    data_file.close();

    // Read number of query vectors
    int query_vecs_num;
    std::ifstream queries_file(argv[2], std::ios::binary);
    queries_file.read(reinterpret_cast<char*>(&query_vecs_num), sizeof(query_vecs_num));

    // Use vector constructor for euclidean distances
    // NOTE: Vector constructor doesn't save queries that contain timestamp in order to maximize spatial effiency
    Vectors vectors(argv[1], VEC_DIMENSION, data_vecs_num, query_vecs_num);
    vectors.read_queries(argv[2], query_vecs_num);
    
    // Create output file and write K
    std::ofstream groundtruth_file(argv[3], std::ios::binary);
    if (!groundtruth_file) throw std::runtime_error("Error opening file: " + (std::string)argv[3]);
    int k = K;
    groundtruth_file.write(reinterpret_cast<const char*>(&k), sizeof(k));

    // Since Vectors class doesn't save queries that contain timestamp, save the number of valid queries
    // and use it as an offset for euclidean_distance_cached()
    int count = 0;    
    int query_bin_index = -1;   // Index of current query in queries.bin

    // Read all vectors. Query vectors have dimension of 104
    std::vector<float> buff(104);
    while (queries_file.read((char *)buff.data(), 104 * sizeof(float))) {
        query_bin_index++;

        // Read a whole vector and save it to 'row'
        std::vector<float> row(104);
        for (int d = 0; d < 104; d++) {
            row[d] = static_cast<float>(buff[d]);
        }

        if (row[0] > 1) continue;   // Ignore timestamp queries

        // Write the actual nearest neighbors
        write_true_knn(groundtruth_file, vectors, data_vecs_num, data_vecs_num + count, query_bin_index);
        count++;
    }

    queries_file.close();
    groundtruth_file.close();
    return 0;
}