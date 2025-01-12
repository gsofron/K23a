#define VEC_DIMENSION 100
#define K 100

#include <algorithm>    // std::sort()
#include <chrono>       // For high-resolution clock
#include <cstdlib>      // srand()
#include <ctime>        // time()
#include <iostream>     // std::cout
#include <string>       // std::string
#include <iterator>

#include "directed_graph.hpp"
#include "filtered_greedy_search.hpp"
#include "filtered_vamana.hpp"
#include "stitched_vamana.hpp"
#include "findmedoid.hpp"
#include "parameter_parser.hpp"
#include "robust_prune.hpp"
#include "utils.hpp"
#include "vamana.hpp"
#include "vectors.hpp"

// Execution examples

// time ./filtered -b dummy/dummy-data.bin -q dummy/dummy-queries.bin -g dummy/dummy-groundtruth.bin -n 10000 -m 5012 -a 1.1 -L 150 -R 12 -t 50 -i -1
// time ./filtered -b dummy/dummy-data.bin -q dummy/dummy-queries.bin -g dummy/dummy-groundtruth.bin -s vamana.bin -n 10000 -m 5012 -a 1.1 -L 150 -R 12 -t 50 -i -1
// time ./filtered -b dummy/dummy-data.bin -q dummy/dummy-queries.bin -g dummy/dummy-groundtruth.bin -v vamana.bin -s new.bin -n 10000 -m 5012 -a 1.1 -L 150 -R 12 -t 50 -i -1

// time ./stitched -b dummy/dummy-data.bin -q dummy/dummy-queries.bin -g dummy/dummy-groundtruth.bin -n 10000 -m 5012 -a 1.1 -L 150 -l 100 -r 32 -R 64 -t 50 -i -1
// time ./stitched -b dummy/dummy-data.bin -q dummy/dummy-queries.bin -g dummy/dummy-groundtruth.bin -s vamana.bin -n 10000 -m 5012 -a 1.1 -L 150 -l 100 -r 32 -R 64 -t 50 -i -1
// time ./stitched -b dummy/dummy-data.bin -q dummy/dummy-queries.bin -g dummy/dummy-groundtruth.bin -v vamana.bin -s new.bin -n 10000 -m 5012 -a 1.1 -L 150 -l 100 -r 32 -R 64 -t 50 -i -1

// Helper function to return elapsed time in seconds
float elapsed_time(std::chrono::time_point<std::chrono::steady_clock> start) {
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::milli> elapsed_time_ms = end - start;
    return elapsed_time_ms.count() / 1000;
}

// Helper function to find common elements between vectors 'vec1' and 'vec2'
int intersect(const std::vector<int>& vec1, const std::vector<int>& vec2) {
    int count = 0;
    for (auto i : vec1) {
        if (std::find(vec2.begin(), vec2.end(), i) != vec2.end()) count++;
    }
    return count;
}

// Calculate recall of current filtered query
float calculate_filtered_recall(float filter, std::unordered_map<float, int> *M, DirectedGraph *g, Vectors& vectors, int j, int base_vectors_num, int L, std::string groundtruth_file) {
    std::vector<int> L_set;
    int start = M->at(filter);
    L_set = FilteredGreedySearch(*g, vectors, start, j + base_vectors_num, K, L, std::numeric_limits<int>::max()).first;

    auto groundtruth = vectors.query_solutions(groundtruth_file, j);
    std::sort(groundtruth.begin(), groundtruth.end());
    while (!groundtruth.empty() && groundtruth[0] == -1) groundtruth.erase(groundtruth.begin());
    int groundtruth_count = groundtruth.size();

    int common_count = intersect(groundtruth, L_set);

    return float(common_count) / groundtruth_count;
}

// Calculate recall of current unfiltered query
float calculate_unfiltered_recall(std::unordered_map<float, int> *M, DirectedGraph *g, Vectors& vectors, int j, int base_vectors_num, int L, std::string groundtruth_file, int limit) {
    std::vector<int> L_set;

    std::set<std::pair<float, int>> all_medoids_knn;
    for (auto pair : *M) {
        auto set = FilteredGreedySearch(*g, vectors, pair.second, j + base_vectors_num, K, L, limit).second;

        auto start = set.begin();
        auto end = set.begin();
        std::advance(end, std::min(K, static_cast<int>(set.size()))); 
        all_medoids_knn.insert(start, end);
    }

    auto it = all_medoids_knn.begin();
    for (int i = 0; i < K && it != all_medoids_knn.end(); i++, it++) {
        L_set.push_back(it->second);
    }
    
    auto groundtruth = vectors.query_solutions(groundtruth_file, j);
    std::sort(groundtruth.begin(), groundtruth.end());
    while (!groundtruth.empty() && groundtruth[0] == -1) groundtruth.erase(groundtruth.begin());
    int groundtruth_count = groundtruth.size();

    int common_count = intersect(groundtruth, L_set);

    return float(common_count) / groundtruth_count;
}

int main(int argc, char *argv[]) {
    srand(time(NULL));  // Seed for randomization

    // Common command line parameters
    std::string base_file, query_file, groundtruth_file, vamana_file = "", save_file = "";
    int base_vectors_num, query_vectors_num, L, t, index, limit = std::numeric_limits<int>::max();
    float a;
    bool random_graph_flag = false;

    // Extra parameter for filtered Vamana
    int R;
    // Extra parameters for stitched Vamana
    int L_small, R_small, R_stitched;
    bool random_medoid_flag = false, random_subset_medoid_flag = false;
    // We (void) these variables so we don't get unused variable warning
    (void)R; (void)L_small; (void)R_small; (void)R_stitched; (void)random_medoid_flag; (void)random_subset_medoid_flag;

    // Parse command line arguements differently for each executable
    #ifdef FILTERED_VAMANA
    parse_filtered(VEC_DIMENSION, K, argc, argv, base_file, query_file, groundtruth_file, vamana_file, save_file, \
                   base_vectors_num, query_vectors_num, a, L, t, index, R, random_graph_flag, limit);
    #else
    parse_stitched(VEC_DIMENSION, K, argc, argv, base_file, query_file, groundtruth_file, vamana_file, save_file, base_vectors_num, \
                   query_vectors_num, a, L, t, index, L_small, R_small, R_stitched, \
                   random_graph_flag, random_medoid_flag, random_subset_medoid_flag, limit);
    #endif

    // Load base and queries vectors
    Vectors vectors(base_file, VEC_DIMENSION, base_vectors_num, query_vectors_num);
    vectors.read_queries(query_file, query_vectors_num);

    // Start timer for build time
    std::cout << "Building..." << std::endl;
    auto build_start = std::chrono::steady_clock::now();

    DirectedGraph *g;
    std::unordered_map<float, int> *M = find_medoid(vectors, t);
    // If user gave vamana file, use it to initialize the graph
    if (!vamana_file.empty()) g = read_vamana_from_file(vamana_file);
    // Else, initialize graph g with FilteredVamana or StitchedVamana accordingly for each executable
    #ifdef FILTERED_VAMANA
    else g = filtered_vamana(vectors, a, L, R, M, random_graph_flag, limit);
    #else
    else g = stitched_vamana(vectors, a, L_small, R_small, R_stitched, random_graph_flag, random_medoid_flag, random_subset_medoid_flag, limit);
    #endif

    // End timer for build time
    std::cout << "Build time: " << elapsed_time(build_start) << " seconds" << std::endl << std::endl;

    // Start timer for total query time
    std::cout << "Querying..." << std::endl;
    auto total_query_start = std::chrono::steady_clock::now();

    // User wants to calculate total recall
    if (index == -1) {
        int count = 0, filtered_count = 0, unfiltered_count = 0;
        float recall_sum = 0.0, filtered_recall_sum = 0.0, unfiltered_recall_sum = 0.0;
        
        // Filtered Queries
        auto filtered_queries_start = std::chrono::steady_clock::now();
        #pragma omp parallel for reduction(+: recall_sum, count, filtered_recall_sum, filtered_count)
        for (int j = 0; j < query_vectors_num; j++) {
            float filter = vectors.filters[j + base_vectors_num];
            if (filter == -1 || M->find(filter) == M->end()) continue;
            
            float current_recall = calculate_filtered_recall(filter, M, g, vectors, j, base_vectors_num, L, groundtruth_file);
            
            // These updates are part of the reduction
            count++;
            recall_sum += current_recall;
            filtered_count++;
            filtered_recall_sum += current_recall;
        }
        std::cout << "Filtered queries time: " << elapsed_time(filtered_queries_start) << std::endl;
        std::cout << "Filtered queries recall: " << 100*filtered_recall_sum/filtered_count << "%" << std::endl << std::endl;

        // Unfiltered Queries
        auto unfiltered_queries_start = std::chrono::steady_clock::now();
        #pragma omp parallel for reduction(+: recall_sum, count, unfiltered_recall_sum, unfiltered_count)
        for (int j = 0; j < query_vectors_num; j++) {
            float filter = vectors.filters[j + base_vectors_num];
            if (filter != -1) continue;
            
            float current_recall = calculate_unfiltered_recall(M, g, vectors, j, base_vectors_num, L, groundtruth_file, limit);
            
            // These updates are part of the reduction
            count++;
            recall_sum += current_recall;
            unfiltered_count++;
            unfiltered_recall_sum += current_recall;
        }
        std::cout << "Unfiltered queries time: " << elapsed_time(unfiltered_queries_start) << std::endl;
        std::cout << "Unfiltered queries recall: " << 100*unfiltered_recall_sum/unfiltered_count << "%" << std::endl << std::endl;

        std::cout << "Calculated recall from " << count << " queries" << std::endl;
        std::cout << "Total Recall Percent: " << 100*recall_sum/count << "%" << std::endl << std::endl;
    } else {
        float filter = vectors.filters[index + base_vectors_num];
        if (filter != -1 && M->find(filter) == M->end()) std::cout << "This query's filter does not match with any filter of the base vectors" << std::endl;;
        
        float current_recall;
        if (filter != -1) current_recall = calculate_filtered_recall(filter, M, g, vectors, index, base_vectors_num, L, groundtruth_file);
        else current_recall = calculate_unfiltered_recall(M, g, vectors, index, base_vectors_num, L, groundtruth_file, limit);
        std::cout << "Current recall is: " << 100*current_recall << "%" << std::endl;
    }

    // End timer for total query time
    std::cout << "Total query time: " << elapsed_time(total_query_start) << " seconds" << std::endl << std::endl;

    // Check if user wants to save the graph
    if (!save_file.empty()) write_vamana_to_file(*g, save_file);

    delete M;
    delete g;
    return 0;
}
