#define VEC_DIMENSION 100
#define K 100

#include <algorithm>    // std::sort()
#include <cstdlib>      // srand()
#include <ctime>        // time()
#include <fstream>      // ifstream
#include <iostream>     // std::cout
#include <string>       // std::string
#include <unistd.h>     // getopt()
#include <iterator>

#include "directed_graph.hpp"
#include "filtered_greedy_search.hpp"
#include "filtered_vamana.hpp"
#include "stitched_vamana.hpp"
#include "findmedoid.hpp"
#include "robust_prune.hpp"
#include "utils.hpp"
#include "vamana.hpp"
#include "vectors.hpp"

// Some function prototypes so program can compile

// Parse input arguments for FilteredVamana
void parse_filtered(int argc, char *argv[], std::string &base_file, std::string &query_file, std::string &groundtruth_file, \
                      std::string &vamana_file, std::string &save_file, int &base_vectors_num, int &query_vectors_num, \
                      float &a, int &L, int &t, int &index, int &R);
// Parse input arguments for StitchedVamana
void parse_stitched(int argc, char *argv[], std::string &base_file, std::string &query_file, std::string &groundtruth_file, \
                      std::string &vamana_file, std::string &save_file, int &base_vectors_num, int &query_vectors_num, \
                      float &a, int &L, int &t, int &index, int &L_small, int &R_small, int &R_stitched);

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

    int R;                              // Extra parameters for filtered Vamana
    int L_small, R_small, R_stitched;   // Extra parameters for stitched Vamana
    // We (void) these variables so we don't get unused variable warning
    (void)R; (void)L_small; (void)R_small; (void)R_stitched;

    // Parse command line arguements differently for each executable
    #ifdef FILTERED_VAMANA
    parse_filtered(argc, argv, base_file, query_file, groundtruth_file, vamana_file, save_file, \
                   base_vectors_num, query_vectors_num, a, L, t, index, R);
    #else
    parse_stitched(argc, argv, base_file, query_file, groundtruth_file, vamana_file, save_file, base_vectors_num, \
                   query_vectors_num, a, L, t, index, L_small, R_small, R_stitched);
    #endif

    // Load base and queries vectors
    Vectors vectors(base_file, VEC_DIMENSION, base_vectors_num, query_vectors_num);
    vectors.read_queries(query_file, query_vectors_num);

    DirectedGraph *g;
    // If user gave vamana file, use it to initialize the graph
    if (!vamana_file.empty()) g = read_vamana_from_file(vamana_file);
    // Else, initialize graph g with FilteredVamana or StitchedVamana accordingly for each executable
    #ifdef FILTERED_VAMANA
    else g = filtered_vamana(vectors, a, L, R, t);
    #else
    else g = stitched_vamana(vectors, a, L_small, R_small, R_stitched);
    #endif

    // User wants to calculate total recall
    auto *M = find_medoid(vectors, t);
    if (index == -1) {
        int count = 0;
        float recall_sum = 0.0;
        
        for (int j = 0; j < query_vectors_num; j++) { 
            float filter = vectors.filters[j + base_vectors_num];
            
            if (filter != -1 && M->find(filter) == M->end()) continue;
            
            count++;

            std::vector<int> L_set;
            if (filter > -1) {
                int start = M->at(filter);
                L_set = FilteredGreedySearch(*g, vectors, start, j+base_vectors_num, K, L).first;
            } else {
                std::set<std::pair<float, int>> all_medoids_knn;
                for (auto pair : *M) {
                    auto set = FilteredGreedySearch(*g, vectors, pair.second, j+base_vectors_num, K, L).second;

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
            while (groundtruth[0] == -1) groundtruth.erase(groundtruth.begin());
            int groundtruth_count = groundtruth.size();

            int common_count = intersect(groundtruth, L_set);

            float current_recall = float(common_count) / groundtruth_count;
            recall_sum += current_recall;
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

    // Check if user wants to save the graph
    if (!save_file.empty()) write_vamana_to_file(*g, save_file);

    delete g;
    return 0;
}

// Print usage in cerr
void print_usage() {
    std::cerr << "Usage: " << std::endl;
    std::cerr << "---MANDATORY FLAGS---" << std::endl;
    std::cerr << "-b <base file>" << std::endl;
    std::cerr << "-q <query file>" << std::endl;
    std::cerr << "-g <groundtruth file>" << std::endl;
    std::cerr << "-n <base vectors number>" << std::endl;
    std::cerr << "-m <query vectors number>" << std::endl;
    std::cerr << "-a <alpha>" << std::endl;
    std::cerr << "-L <L>" << std::endl;
    #ifdef FILTERED_VAMANA
    std::cerr << "-R <R>" << std::endl;
    #else
    std::cerr << "-l <Lsmall>" << std::endl;
    std::cerr << "-r <Rsmall>" << std::endl;
    std::cerr << "-R <Rstitched>" << std::endl;
    #endif
    std::cerr << "-t <tau>" << std::endl;
    std::cerr << "-i <query index> (use -1 to calculate total recall)" << std::endl;
    std::cerr << "---OPTIONAL FLAGS---" << std::endl;
    std::cerr << "-v <vamana file>" << std::endl;
    std::cerr << "-s <save file>" << std::endl;
}

// Parse input arguments for FilteredVamana
void parse_filtered(int argc, char *argv[], std::string &base_file, std::string &query_file, std::string &groundtruth_file, \
                      std::string &vamana_file, std::string &save_file, int &base_vectors_num, int &query_vectors_num, \
                      float &a, int &L, int &t, int &index, int &R) {
    // Common command line mandatory flags
    bool base_file_flag = false, query_file_flag = false, groundtruth_file_flag = false, base_vectors_num_flag = false, \
         query_vectors_num_flag = false, a_flag = false, L_flag = false, t_flag = false, index_flag = false;
    bool R_flag = false;    // Extra mandatory flag for FilteredVamana
         
    // For FilteredVamana, minimum arguements are 21 and maximum are 25. Arguements come in pairs so argc must always be odd
    if (argc < 21 || argc > 25 || argc % 2 == 0) {
        print_usage();
        exit(EXIT_FAILURE);
    }

    // Parse arguments using getopt
    int opt;
    std::ifstream file;
    while ((opt = getopt(argc, argv, "b:q:g:v:s:n:m:a:L:t:i:R:")) != -1) {
    // while ((opt = getopt(argc, argv, "b:q:g:v:s:n:m:a:i:L:Lsmall:Rsmall:Rstitched:")) != -1) {
        switch (opt) {
        case 'b': // Base vectors file
            base_file = optarg;
            if (!std::ifstream(base_file)) {
                std::cerr << "Base vectors file doesn't exist" << std::endl;
                exit(EXIT_FAILURE);
            }
            base_file_flag = true;
            break;
        case 'q': // Query vectors file
            query_file = optarg;
            if (!std::ifstream(query_file)) {
                std::cerr << "Query vectors file doesn't exist" << std::endl;
                exit(EXIT_FAILURE);
            }
            query_file_flag = true;
            break;
        case 'g': // Groundtruth vectors file
            groundtruth_file = optarg;
            if (!std::ifstream(groundtruth_file)) {
                std::cerr << "Groundtruth file doesn't exist" << std::endl;
                exit(EXIT_FAILURE);
            }
            groundtruth_file_flag = true;
            break;
        case 'v': // Vamana graph file
            vamana_file = optarg;
            if (!std::ifstream(vamana_file)) {
                std::cerr << "Vamana file doesn't exist" << std::endl;
                exit(EXIT_FAILURE);
            }
            break;
        case 's': // Save file
            save_file = optarg;
            break;
        case 'n': // Number of base vectors
            base_vectors_num = std::stoi(optarg);
            if (base_vectors_num <= 0) {
                std::cerr << "Base vectors number must be positive" << std::endl;
                exit(EXIT_FAILURE);
            }
            base_vectors_num_flag = true;
            break;
        case 'm': // Number of query vectors
            query_vectors_num = std::stoi(optarg);
            if (query_vectors_num <= 0) {
                std::cerr << "Query vectors number must be positive" << std::endl;
                exit(EXIT_FAILURE);
            }
            query_vectors_num_flag = true;
            break;
        case 'a': // Alpha value
            a = std::stof(optarg);
            if (a < 1.0) {
                std::cerr << "Alpha value cannot be less than 1" << std::endl;
                exit(EXIT_FAILURE);
            }
            a_flag = true;
            break;
        case 'L': // Greedy search limit L
            L = std::stoi(optarg);
            if (L < K) {
                std::cerr << "L cannot be smaller than K" << std::endl;
                exit(EXIT_FAILURE);
            }
            L_flag = true;
            break;
        case 't': // Tau for FindMedoid()
            t = std::stoi(optarg);
            if (t < 1) {
                std::cerr << "Tau must be greater than 0" << std::endl;
                exit(EXIT_FAILURE);
            }
            t_flag = true;
            break;
        case 'i': // Index of query vector
            index = std::stoi(optarg);
            index_flag = true;
            break;
        case 'R': // Graph degree limit R
            R = std::stoi(optarg);
            if (R <= 0) {
                std::cerr << "R must be positive" << std::endl;
                exit(EXIT_FAILURE);
            }
            R_flag = true;
            break;
        default:
            print_usage();
            exit(EXIT_FAILURE);
        }
    }

    // Check if all mandatory flags are given
    if (!base_file_flag || !query_file_flag || !groundtruth_file_flag || !base_vectors_num_flag || !query_vectors_num_flag \
        || !a_flag || !L_flag || !t_flag || !index_flag || !R_flag) {
            print_usage();
            exit(EXIT_FAILURE);
        }

    // Check this here to make sure query_vectors_num is initialized
    if (index < -1 || index >= query_vectors_num) {
        std::cerr << "Invalid query index" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Output parameters
    std::cout << "-----Parameters-----" << std::endl;
    std::cout << "Base file = " << base_file << std::endl;
    std::cout << "Query file = " << query_file << std::endl;
    std::cout << "Groundtruth file = " << groundtruth_file << std::endl;
    std::cout << "Vamana file = " << vamana_file << std::endl;
    std::cout << "Save file = " << save_file << std::endl;
    std::cout << "Vector dimension = " << VEC_DIMENSION << std::endl;
    std::cout << "k = " << K << std::endl;
    std::cout << "Base vectors number = " << base_vectors_num << std::endl;
    std::cout << "Query vectors number = " << query_vectors_num << std::endl;
    std::cout << "a = " << a << std::endl;
    std::cout << "L = " << L << std::endl;
    std::cout << "R = " << R << std::endl;
    std::cout << "t = " << t << std::endl;
    std::cout << "index = " << index << std::endl;
    std::cout << std::endl;
}

// Parse input arguments for StitchedVamana
void parse_stitched(int argc, char *argv[], std::string &base_file, std::string &query_file, std::string &groundtruth_file, \
                      std::string &vamana_file, std::string &save_file, int &base_vectors_num, int &query_vectors_num, \
                      float &a, int &L, int &t, int &index, int &L_small, int &R_small, int &R_stitched) {
    // Common command line mandatory flags
    bool base_file_flag = false, query_file_flag = false, groundtruth_file_flag = false, base_vectors_num_flag = false, \
         query_vectors_num_flag = false, a_flag = false, L_flag = false, t_flag = false, index_flag = false;
    bool L_small_flag = false, R_small_flag = false, R_stitched_flag = false;   // Extra mandatory flags for FilteredVamana
         

    // For StitchedVamana, minimum arguements are 25 and maximum are 29. Arguements come in pairs so argc must always be odd
    if (argc < 25 || argc > 29 || argc % 2 == 0) {
        print_usage();
        exit(EXIT_FAILURE);
    }

    // Parse arguments using getopt
    int opt;
    std::ifstream file;
    while ((opt = getopt(argc, argv, "b:q:g:v:s:n:m:a:L:t:i:l:r:R:")) != -1) {
    // while ((opt = getopt(argc, argv, "b:q:g:v:s:n:m:a:i:L:Lsmall:Rsmall:Rstitched:")) != -1) {
        switch (opt) {
        case 'b': // Base vectors file
            base_file = optarg;
            if (!std::ifstream(base_file)) {
                std::cerr << "Base vectors file doesn't exist" << std::endl;
                exit(EXIT_FAILURE);
            }
            base_file_flag = true;
            break;
        case 'q': // Query vectors file
            query_file = optarg;
            if (!std::ifstream(query_file)) {
                std::cerr << "Query vectors file doesn't exist" << std::endl;
                exit(EXIT_FAILURE);
            }
            query_file_flag = true;
            break;
        case 'g': // Groundtruth vectors file
            groundtruth_file = optarg;
            if (!std::ifstream(groundtruth_file)) {
                std::cerr << "Groundtruth file doesn't exist" << std::endl;
                exit(EXIT_FAILURE);
            }
            groundtruth_file_flag = true;
            break;
        case 'v': // Vamana graph file
            vamana_file = optarg;
            if (!std::ifstream(vamana_file)) {
                std::cerr << "Vamana file doesn't exist" << std::endl;
                exit(EXIT_FAILURE);
            }
            break;
        case 's': // Save file
            save_file = optarg;
            break;
        case 'n': // Number of base vectors
            base_vectors_num = std::stoi(optarg);
            if (base_vectors_num <= 0) {
                std::cerr << "Base vectors number must be positive" << std::endl;
                exit(EXIT_FAILURE);
            }
            base_vectors_num_flag = true;
            break;
        case 'm': // Number of query vectors
            query_vectors_num = std::stoi(optarg);
            if (query_vectors_num <= 0) {
                std::cerr << "Query vectors number must be positive" << std::endl;
                exit(EXIT_FAILURE);
            }
            query_vectors_num_flag = true;
            break;
        case 'a': // Alpha value
            a = std::stof(optarg);
            if (a < 1.0) {
                std::cerr << "Alpha value cannot be less than 1" << std::endl;
                exit(EXIT_FAILURE);
            }
            a_flag = true;
            break;
        case 'L': // Greedy search limit L
            L = std::stoi(optarg);
            if (L < K) {
                std::cerr << "L cannot be smaller than K" << std::endl;
                exit(EXIT_FAILURE);
            }
            L_flag = true;
            break;
        case 't': // Tau for FindMedoid()
            t = std::stoi(optarg);
            if (t < 1) {
                std::cerr << "Tau must be greater than 0" << std::endl;
                exit(EXIT_FAILURE);
            }
            t_flag = true;
            break;
        case 'i': // Index of query vector
            index = std::stoi(optarg);
            index_flag = true;
            break;
        case 'l': // Lsmall for StitchedVamana
            L_small = std::stoi(optarg);
            if (L_small <= 0) {
                std::cerr << "L_small must be positive" << std::endl;
                exit(EXIT_FAILURE);
            }
            L_small_flag = true;
            break;
        case 'r': // Rsmall for StitchedVamana
            R_small = std::stoi(optarg);
            if (R_small <= 0) {
                std::cerr << "R_small must be positive" << std::endl;
                exit(EXIT_FAILURE);
            }
            R_small_flag = true;
            break;
        case 'R': // Rstitched for StitchedVamana
            R_stitched = std::stoi(optarg);
            if (R_stitched <= 0) {
                std::cerr << "R_stitched must be positive" << std::endl;
                exit(EXIT_FAILURE);
            }
            R_stitched_flag = true;
            break;
        default:
            print_usage();
            exit(EXIT_FAILURE);
        }
    }

    // Check if all mandatory flags are given
    if (!base_file_flag || !query_file_flag || !groundtruth_file_flag || !base_vectors_num_flag || !query_vectors_num_flag || \
        !a_flag || !L_flag || !t_flag || !index_flag || !L_small_flag || !R_small_flag ||!R_stitched_flag) {
            print_usage();
            exit(EXIT_FAILURE);
        }

    // Check this here to make sure query_vectors_num is initialized
    if (index < -1 || index >= query_vectors_num) {
        std::cerr << "Invalid query index" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Output parameters
    std::cout << "-----Parameters-----" << std::endl;
    std::cout << "Base file = " << base_file << std::endl;
    std::cout << "Query file = " << query_file << std::endl;
    std::cout << "Groundtruth file = " << groundtruth_file << std::endl;
    std::cout << "Vamana file = " << vamana_file << std::endl;
    std::cout << "Save file = " << save_file << std::endl;
    std::cout << "Vector dimension = " << VEC_DIMENSION << std::endl;
    std::cout << "k = " << K << std::endl;
    std::cout << "Base vectors number = " << base_vectors_num << std::endl;
    std::cout << "Query vectors number = " << query_vectors_num << std::endl;
    std::cout << "a = " << a << std::endl;
    std::cout << "L = " << L << std::endl;
    std::cout << "Lsmall = " << L_small << std::endl;
    std::cout << "Rsmall = " << R_small << std::endl;
    std::cout << "Rstitched = " << R_stitched << std::endl;
    std::cout << "t = " << t << std::endl;
    std::cout << "index = " << index << std::endl;
    std::cout << std::endl;
}