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

// Helper function to find common elements between vectors 'vec1' and 'vec2'
int intersect(const std::vector<int>& vec1, const std::vector<int>& vec2) {
    int count = 0;
    for (auto i : vec1) {
        if (std::find(vec2.begin(), vec2.end(), i) != vec2.end()) count++;
    }
    return count;
}

int main(int argc, char *argv[]) {
    srand(time(NULL));  // Seed for randomization

    // Common command line parameters
    std::string base_file, query_file, groundtruth_file, vamana_file = "", save_file = "";
    int base_vectors_num, query_vectors_num, L, t, index;
    float a;
    bool random_graph_flag = false;

    int R;                              // Extra parameters for filtered Vamana
    int L_small, R_small, R_stitched;   // Extra parameters for stitched Vamana
    bool random_medoid_flag = false, random_subset_medoid_flag = false;
    // We (void) these variables so we don't get unused variable warning
    (void)R; (void)L_small; (void)R_small; (void)R_stitched; (void)random_medoid_flag; (void)random_subset_medoid_flag;

    // Parse command line arguements differently for each executable
    #ifdef FILTERED_VAMANA
    parse_filtered(VEC_DIMENSION, K, argc, argv, base_file, query_file, groundtruth_file, vamana_file, save_file, \
                   base_vectors_num, query_vectors_num, a, L, t, index, R, random_graph_flag);
    #else
    parse_stitched(VEC_DIMENSION, K, argc, argv, base_file, query_file, groundtruth_file, vamana_file, save_file, base_vectors_num, \
                   query_vectors_num, a, L, t, index, L_small, R_small, R_stitched, \
                   random_graph_flag, random_medoid_flag, random_subset_medoid_flag);
    #endif

    // Load base and queries vectors
    Vectors vectors(base_file, VEC_DIMENSION, base_vectors_num, query_vectors_num);
    vectors.read_queries(query_file, query_vectors_num);

    // Start timer for build time
    auto build_time_start = std::chrono::high_resolution_clock::now();

    DirectedGraph *g;
    std::unordered_map<float, int> *M = nullptr;
    // If user gave vamana file, use it to initialize the graph
    if (!vamana_file.empty()) g = read_vamana_from_file(vamana_file);
    // Else, initialize graph g with FilteredVamana or StitchedVamana accordingly for each executable
    #ifdef FILTERED_VAMANA
    else g = filtered_vamana(vectors, a, L, R, t, M, random_graph_flag);
    #else
    else g = stitched_vamana(vectors, a, L_small, R_small, R_stitched, random_graph_flag, random_medoid_flag, random_subset_medoid_flag);
    #endif

    // End timer for build time
    auto build_time_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> build_time_ms = build_time_end - build_time_start;
    std::cout << "Build time: " << build_time_ms.count() / 1000 << " seconds" << std::endl << std::endl;

    // Start timer for query time
    double filtered_time_ms = 0.0;
    auto query_time_start = std::chrono::high_resolution_clock::now();

    // User wants to calculate total recall
    if (M == nullptr) M = find_medoid(vectors, t);
    if (index == -1) {
        int count = 0;
        float recall_sum = 0.0;
        
        #pragma omp parallel for reduction(+: recall_sum, filtered_time_ms) num_threads(4)
        for (int j = 0; j < query_vectors_num; j++) {
            float filter = vectors.filters[j + base_vectors_num];

            if (filter != -1 && M->find(filter) == M->end()) continue;
            
            // Start timer for a single query
            auto single_query_start = std::chrono::high_resolution_clock::now();

            #pragma omp atomic
            count++;  // Ensuring thread-safe update of shared variable

            std::vector<int> L_set;
            if (filter > -1) {
                int start = M->at(filter);
                L_set = FilteredGreedySearch(*g, vectors, start, j + base_vectors_num, K, L).first;
            } else {
                std::set<std::pair<float, int>> all_medoids_knn;
                for (auto pair : *M) {
                    auto set = FilteredGreedySearch(*g, vectors, pair.second, j + base_vectors_num, K, L).second;

                    auto start = set.begin();
                    auto end = set.begin();
                    std::advance(end, std::min(K, static_cast<int>(set.size()))); 
                    all_medoids_knn.insert(start, end);
                }

                auto it = all_medoids_knn.begin();
                for (int i = 0; i < K && it != all_medoids_knn.end(); i++, it++) {
                    L_set.push_back(it->second);
                }
            }

            auto groundtruth = vectors.query_solutions(groundtruth_file, j);
            std::sort(groundtruth.begin(), groundtruth.end());
            while (!groundtruth.empty() && groundtruth[0] == -1) groundtruth.erase(groundtruth.begin());
            int groundtruth_count = groundtruth.size();

            int common_count = intersect(groundtruth, L_set);

            float current_recall = float(common_count) / groundtruth_count;
            recall_sum += current_recall;  // This update is part of the reduction

            // End timer for a single query
            auto single_query_end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> single_query_time_ms = single_query_end - single_query_start;

            // Increase total query time of filtered queries
            if (filter != -1) filtered_time_ms += single_query_time_ms.count();
        }

        std::cout << "Calculated recall from " << count << " queries" << std::endl;
        std::cout << "Total Recall Percent: " << 100*recall_sum/count << "%" << std::endl;
    } else {
        float filter = vectors.filters[index + base_vectors_num];
            
        if (filter != -1 && M->find(filter) == M->end()) std::cout << "This query's filter does not match with any filter of the base vectors" << std::endl;;
        

        std::vector<int> L_set;
        if (filter > -1) {
            int start = M->at(filter);
            L_set = FilteredGreedySearch(*g, vectors, start, index+base_vectors_num, K, L).first;
        } else {
            std::set<std::pair<float, int>> all_medoids_knn;
            for (auto pair : *M) {
                auto set = FilteredGreedySearch(*g, vectors, pair.second, index+base_vectors_num, K, L).second;

                auto start = set.begin();
                auto end = set.begin();
                std::advance(end, std::min(K, static_cast<int>(set.size()))); 
                all_medoids_knn.insert(start, end);
            }

            auto it = all_medoids_knn.begin();
            for (int i = 0; i < K && it != all_medoids_knn.end(); i++, it++) {
                L_set.push_back(it->second);
            }
        }

        auto groundtruth = vectors.query_solutions(groundtruth_file, index);
        std::sort(groundtruth.begin(), groundtruth.end());
        while (groundtruth[0] == -1) groundtruth.erase(groundtruth.begin());
        int groundtruth_count = groundtruth.size();

        int common_count = intersect(groundtruth, L_set);

        float current_recall = float(common_count) / groundtruth_count;
        std::cout << "Current recall is: " << 100*current_recall << "%" << std::endl;
    }

    // End timer for query time
    auto query_time_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> query_time_ms = query_time_end - query_time_start;
    std::cout << std::endl << "Query time: " << query_time_ms.count() / 1000 << " seconds" << std::endl;
    if (index == -1) {
        std::cout << "Filtered queries time: " << filtered_time_ms / 1000 << " seconds" << std::endl;
        std::cout << "Unfiltered queries time: " << (query_time_ms.count() - filtered_time_ms) / 1000 << " seconds" << std::endl;
    }

    // Check if user wants to save the graph
    if (!save_file.empty()) write_vamana_to_file(*g, save_file);

    delete M;
    delete g;
    return 0;
}
