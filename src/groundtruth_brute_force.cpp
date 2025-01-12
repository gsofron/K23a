#define K 100
#define VEC_DIMENSION 100

#include <fstream>
#include <iostream>
#include <set>

#include "vectors.hpp"

// Returns the true top k nearest neighbors for unfiltered queries
std::vector<int> get_true_knn_unfiltered(Vectors &vectors, int data_vecs_num, int query_index) {
    // Save the pair (euclidean distance, index) of all neighbors in ascending euclidean distance
    std::set<std::pair<float, int>> s;

    // Calculate euclidean distance from query vector to every single data vector
    for (int i = 0 ; i < data_vecs_num ; i++) {
        float dist = vectors.euclidean_distance(data_vecs_num + query_index, i);
        std::pair<float, int> p(dist, i);
        s.insert(p);
    }

    // Get the K nearest neighbors
    std::vector<int> knn;
    int count = 0;
    for (auto& pair : s) {
        knn.push_back(pair.second);
        if (++count == K) break;
    }
    // If there are less than K neighbors, pad with -1
    for (int i = 0 ; i < K - count ; i++) {
        knn.push_back(-1);
    }
    return knn;
}

// Returns the true top k nearest neighbors for unfiltered queries
std::vector<int> get_true_knn_filtered(Vectors &vectors, int data_vecs_num, int query_index) {
    float query_filter = vectors.filters[data_vecs_num + query_index];
    // Save the pair (euclidean distance, index) of all neighbors in ascending euclidean distance
    std::set<std::pair<float, int>> s;

    // Calculate euclidean distance from query vector to every single data vector
    for (int i = 0 ; i < data_vecs_num ; i++) {
        // Ignore vectors with different filters
        float current_filter = vectors.filters[i];
        if (query_filter != current_filter) continue;

        float dist = vectors.euclidean_distance(data_vecs_num + query_index, i);
        std::pair<float, int> p(dist, i);
        s.insert(p);
    }

    // Get the K nearest neighbors
    std::vector<int> knn;
    int count = 0;
    for (auto& pair : s) {
        knn.push_back(pair.second);
        if (++count == K) break;
    }
    // If there are less than K neighbors, pad with -1
    for (int i = 0 ; i < K - count ; i++) {
        knn.push_back(-1);
    }
    return knn;
}

// ./groundtruth ./dummy/dummy-data.bin ./dummy/dummy-queries.bin ./dummy/dummy-groundtruth.bin
int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <data vectors> <query vectors> <groundtruth filename>" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Read number of data vectors
    u_int32_t u_data_vecs_num;
    std::ifstream data_file(argv[1], std::ios::binary);
    data_file.read(reinterpret_cast<char*>(&u_data_vecs_num), sizeof(u_data_vecs_num));
    data_file.close();
    int data_vecs_num = static_cast<int>(u_data_vecs_num);

    // Read number of query vectors
    u_int32_t u_query_vecs_num;
    std::ifstream queries_file(argv[2], std::ios::binary);
    queries_file.read(reinterpret_cast<char*>(&u_query_vecs_num), sizeof(u_query_vecs_num));
    int query_vecs_num = static_cast<int>(u_query_vecs_num);


    // Use vector constructor for euclidean distances
    // NOTE: Vector constructor doesn't save queries that contain timestamp in order to maximize spatial effiency
    Vectors vectors(argv[1], VEC_DIMENSION, data_vecs_num, query_vecs_num);
    vectors.read_queries(argv[2], query_vecs_num);

    std::cout << "Reading queries..." << std::endl;

    // Read contents of query file to determine which queries are valid
    // Invalid queries have index -1
    // False are unfiltered, true are filtered
    std::pair<int, bool> index_of[query_vecs_num];
    int valid_queries_count = 0;
    for (int i = 0 ; i < query_vecs_num ; i++) {
        std::vector<float> buff(104);
        queries_file.read((char *)buff.data(), 104 * sizeof(float));
        if (buff[0] == 0) index_of[i] = {valid_queries_count++, false};
        else if (buff[0] == 1) index_of[i] = {valid_queries_count++, true};
        else index_of[i] = {-1, false};
    }
    queries_file.close();

    // Create output file
    std::ofstream groundtruth_file(argv[3], std::ios::binary);
    if (!groundtruth_file) throw std::runtime_error("Error opening file: " + (std::string)argv[3]);

    // Store all results to write them afterwards
    std::vector<std::vector<int>> results(query_vecs_num);

    std::cout << "Computing KNNs..." << std::endl;
    // Calculate K nearest neighbors for all vectors
    #pragma omp parallel for
    for (int i = 0 ; i < query_vecs_num ; i++) {
        if (index_of[i].first == -1) continue;

        // Get K nearest neighbors according to query type. Ignore timestamp queries
        std::vector<int> knn;
        if (index_of[i].second == false) knn = get_true_knn_unfiltered(vectors, data_vecs_num, index_of[i].first);
        else knn = get_true_knn_filtered(vectors, data_vecs_num, index_of[i].first);

        // Store current result
        results[i] = knn;
    }

    std::cout << "Writing KNNs..." << std::endl;
    // After all threads are done, write results in order
    for (int i = 0; i < query_vecs_num; i++) {
        auto& knn = results[i];
        for (int index : knn) {
            groundtruth_file.write(reinterpret_cast<const char*>(&index), sizeof(index));
        }
    }

    std::cout << "Groundtruth brute force finished. Counted a total of " << valid_queries_count << " valid queries" << std::endl;
    queries_file.close();
    groundtruth_file.close();
    return 0;
}