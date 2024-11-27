#define VEC_DIMENSION 100
#define K 100

#include <algorithm>
#include <cstdlib>     
#include <ctime>        
#include <fstream>      
#include <iostream>    
#include <string>      
#include <unistd.h>     

#include "directed_graph.hpp"
#include "filtered_greedy_search.hpp"
#include "filtered_vamana.hpp"
#include "findmedoid.hpp"
#include "robust_prune.hpp"
#include "utils.hpp"
#include "vamana.hpp"
#include "vectors.hpp"

// Execution examples
// time ./k23a -b dummy/dummy-data.bin -q dummy/dummy-queries.bin -g dummy/dummy-groundtruth.bin -n 10000 -m 5012 -a 1.1 -l 150 -r 100 -t 50 -i -1
// time ./k23a -b dummy/dummy-data.bin -q dummy/dummy-queries.bin -g dummy/dummy-groundtruth.bin -s vamana.bin -n 10000 -m 5012 -a 1.1 -l 150 -r 100 -t 50 -i -1
// time ./k23a -b dummy/dummy-data.bin -q dummy/dummy-queries.bin -g dummy/dummy-groundtruth.bin -v vamana.bin -s new.bin -n 10000 -m 5012 -a 1.1 -l 150 -r 100 -t 50 -i -1

void print_usage() {
    std::cerr << "Usage: " << std::endl;
    std::cerr << "---MANDATORY FLAGS---" << std::endl;
    std::cerr << "-b <base file>" << std::endl;
    std::cerr << "-q <query file>" << std::endl;
    std::cerr << "-g <groundtruth file>" << std::endl;
    std::cerr << "-n <base vectors number>" << std::endl;
    std::cerr << "-m <query vectors number>" << std::endl;
    std::cerr << "-a <alpha>" << std::endl;
    std::cerr << "-l <l>" << std::endl;
    std::cerr << "-r <r>" << std::endl;
    std::cerr << "-t <tau>" << std::endl;
    std::cerr << "-i <query index> (use -1 to calculate total recall)" << std::endl;
    std::cerr << "---OPTIONAL FLAGS---" << std::endl;
    std::cerr << "-v <vamana file>" << std::endl;
    std::cerr << "-s <save file>" << std::endl;
    exit(EXIT_FAILURE);
}

// Parse input arguments and load necessary parameters
void parse_parameters(int argc, char *argv[], std::string &base_file, std::string &query_file, std::string &groundtruth_file, \
                        std::string &vamana_file, std::string &save_file, int &base_vectors_num, int &query_vectors_num, \
                        float &a, int &L, int &R, int &t, int &index) {
    int opt;
    bool base_flag = false, query_flag = false, ground_flag = false, base_num_flag = false, query_num_flag = false, \
        a_flag = false, l_flag = false, r_flag = false, index_flag = false, t_flag = false;
    std::ifstream file;

    // Minimum arguements are 21 and maximum are 25. Arguements come in pairs so argc must always be odd
    if (argc < 21 || argc > 25 || argc % 2 == 0) print_usage();
    
    // Parse arguments using getopt
    while ((opt = getopt(argc, argv, "b:q:g:v:s:n:m:a:l:r:t:i:")) != -1) {
        switch (opt) {
        case 'b': // Base vectors file
            base_file = optarg;
            base_flag = true;
            if (!std::ifstream(base_file)) {
                std::cerr << "Base vectors file doesn't exist" << std::endl;
                exit(EXIT_FAILURE);
            }
            break;
        case 'q': // Query vectors file
            query_file = optarg;
            query_flag = true;
            if (!std::ifstream(query_file)) {
                std::cerr << "Query vectors file doesn't exist" << std::endl;
                exit(EXIT_FAILURE);
            }
            break;
        case 'g': // Groundtruth vectors file
            groundtruth_file = optarg;
            ground_flag = true;
            if (!std::ifstream(groundtruth_file)) {
                std::cerr << "Groundtruth file doesn't exist" << std::endl;
                exit(EXIT_FAILURE);
            }
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
            base_num_flag = true;
            if (base_vectors_num <= 0) {
                std::cerr << "Base vectors number must be positive" << std::endl;
                exit(EXIT_FAILURE);
            }
            break;
        case 'm': // Number of query vectors
            query_vectors_num = std::stoi(optarg);
            query_num_flag = true;
            if (query_vectors_num <= 0) {
                std::cerr << "Query vectors number must be positive" << std::endl;
                exit(EXIT_FAILURE);
            }
            break;
        case 'a': // Alpha value
            a = std::stof(optarg);
            a_flag = true;
            if (a < 1.0) {
                std::cerr << "Alpha value cannot be less than 1" << std::endl;
                exit(EXIT_FAILURE);
            }
            break;
        case 'l': // Greedy search limit L
            L = std::stoi(optarg);
            l_flag = true;
            if (L < K) {
                std::cerr << "L cannot be smaller than K" << std::endl;
                exit(EXIT_FAILURE);
            }
            break;
        case 'r': // Graph degree limit R
            R = std::stoi(optarg);
            r_flag = true;
            if (R <= 0) {
                std::cerr << "R must be positive" << std::endl;
                exit(EXIT_FAILURE);
            }
            break;
        case 't': // Tau for FindMedoid()
            t = std::stoi(optarg);
            t_flag = true;
            if (t < 1) {
                std::cerr << "Tau must be greater than 0" << std::endl;
                exit(EXIT_FAILURE);
            }
            break;
        case 'i': // Index of query vector
            index = std::stoi(optarg);
            index_flag = true;
            break;
        default:
            print_usage();
        }
    }

    // Check this here to make sure query_vectors_num is initialized
    if (index < -1 || index >= query_vectors_num) {
        std::cerr << "Invalid query index" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Check if all mandatory flags are given
    if (!base_flag || !query_flag || !ground_flag || !base_num_flag || !query_num_flag || !a_flag || !l_flag || !r_flag || !index_flag || !t_flag) print_usage();

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

// Helper function to find elements in `a` not in `b`
int intersect(const std::vector<int>& vec1, const std::vector<int>& vec2) {
    int count = 0;
    for (auto i : vec1) {
        if (std::find(vec2.begin(), vec2.end(), i) != vec2.end()) count++;
    }
    return count;
}

// Main execution and menu
int main(int argc, char *argv[]) {
    srand(time(NULL));  // Seed for randomization

    // Initialize parameters and parse command line input
    int base_vectors_num, query_vectors_num, L, R, t, index;
    float a;
    std::string base_file, query_file, groundtruth_file, vamana_file = "", save_file = "";
    parse_parameters(argc, argv, base_file, query_file, groundtruth_file, vamana_file, save_file, base_vectors_num, query_vectors_num, a, L, R, t, index);

    // Load base and queries vectors
    Vectors vectors(base_file, VEC_DIMENSION, base_vectors_num, query_vectors_num);
    vectors.read_queries(query_file, query_vectors_num);

    DirectedGraph *g;

    // If user gave vamana file, use it to initialize the graph
    if (!vamana_file.empty()) g = read_vamana_from_file(vamana_file);
    else g = filtered_vamana(vectors, a, L, R, t);

    // User wants to calculate total recall
    auto *M = find_medoid(vectors, t);
    if (index == -1) {
        int count = 0;
        float recall_sum = 0.0;
        
        for (int j = 0; j < query_vectors_num; j++) { 
            float filter = vectors.filters[j + base_vectors_num];
            if (filter == 144 || filter == -1) continue;
  
            count++;
            int start = M->at(filter);


            auto L_set = FilteredGreedySearch(*g, vectors, start, j+base_vectors_num, K, L).first;

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
        int start = M->at(filter);
        if (index >= query_vectors_num) {
            std::cout << "Invalid query index." << std::endl;
            return 0;
        }
        auto L_set = FilteredGreedySearch(*g, vectors, start, base_vectors_num + index, K, L).first;
        
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