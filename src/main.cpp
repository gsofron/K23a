#define VEC_DIMENSION 128
#define K 100

#include <cstdlib>     
#include <ctime>        
#include <fstream>      
#include <iostream>    
#include <string>      
#include <unistd.h>     

#include "directed_graph.hpp"
#include "robust_prune.hpp"
#include "utils.hpp"
#include "vamana.hpp"
#include "vectors.hpp"

// Execution examples
// time ./k23a -b siftsmall/siftsmall_base.fvecs -q siftsmall/siftsmall_query.fvecs -g siftsmall/siftsmall_groundtruth.ivecs -a 1.1 -l 150 -r 100
// time ./k23a -b siftsmall/siftsmall_base.fvecs -q siftsmall/siftsmall_query.fvecs -g siftsmall/siftsmall_groundtruth.ivecs -v vamana.bin -a 1.1 -l 150 -r 100

// Parse input arguments and load necessary parameters
void parse_parameters(int argc, char *argv[], std::string &base_file, std::string &query_file, std::string &groundtruth_file, std::string &vamana_file, float &a, int &L, int &R) {
    int opt;
    bool base_flag = false, query_flag = false, ground_flag = false, a_flag = false, l_flag = false, r_flag = false;
    std::ifstream file;

    // Require 13 arguments without vamana file, 15 with vamana file
    if (argc != 13 && argc != 15) {
        std::cerr << "Usages: " << std::endl \
        << "- " << argv[0] << " -b <base file> -q <query file> -g <groundtruth file> -a <alpha> -l <L> -r <R>" << std::endl \
        << "- " << argv[0] << " -b <base file> -q <query file> -g <groundtruth file> -v <vamana file> -a <alpha> -l <L> -r <R>";
        exit(EXIT_FAILURE);
    }
    
    // Parse arguments using getopt
    while ((opt = getopt(argc, argv, "b:q:g:v:a:l:r:")) != -1) {
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
        case 'v': // Base vectors file
            vamana_file = optarg;
            if (!std::ifstream(vamana_file)) {
                std::cerr << "Vamana file doesn't exist" << std::endl;
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
        default:
            std::cerr << "Usages: " << std::endl \
            << "- " << argv[0] << " -b <base file> -q <query file> -g <groundtruth file> -a <alpha> -l <L> -r <R>" << std::endl \
            << "- " << argv[0] << " -b <base file> -q <query file> -g <groundtruth file> -v <vamana file> -a <alpha> -l <L> -r <R>";
            exit(EXIT_FAILURE);
        }
    }

    // Check if all mandatory flags are given
    if (!base_flag || !query_flag || !ground_flag || !a_flag || !l_flag || !r_flag) {
        std::cerr << "Usages: " << std::endl \
        << "- " << argv[0] << " -b <base file> -q <query file> -g <groundtruth file> -a <alpha> -l <L> -r <R>" << std::endl \
        << "- " << argv[0] << " -b <base file> -q <query file> -g <groundtruth file> -v <vamana file> -a <alpha> -l <L> -r <R>";
        exit(EXIT_FAILURE);
    }

    // Output parameters
    std::cout << "-----Parameters-----" << std::endl;
    std::cout << "Base file = " << base_file << std::endl;
    std::cout << "Query file = " << query_file << std::endl;
    std::cout << "Groundtruth file = " << groundtruth_file << std::endl;
    std::cout << "Vamana file = " << vamana_file << std::endl;
    std::cout << "Vector dimension = " << VEC_DIMENSION << std::endl;
    std::cout << "L = " << L << std::endl;
    std::cout << "R = " << R << std::endl;
    std::cout << "a = " << a << std::endl;
}

// Helper function to find elements in `a` not in `b`
std::vector<int> findDifference(const std::vector<int>& a, const std::vector<int>& b) {
    std::unordered_set<int> setB(b.begin(), b.end());
    std::vector<int> result;

    for (int num : a) {
        if (!setB.count(num)) result.push_back(num);
    }
    return result;
}

// Main execution and menu
int main(int argc, char *argv[]) {
    srand(time(NULL));  // Seed for randomization

    // Initialize parameters and parse command line input
    int base_vectors = 10000, queries_vectors = 100, L, R;
    float a;
    std::string base_file, query_file, groundtruth_file, vamana_file = "";
    parse_parameters(argc, argv, base_file, query_file, groundtruth_file, vamana_file, a, L, R);

    // Load vectors
    int read_vectors;
    Vectors<float> vectors(base_file, read_vectors, base_vectors, queries_vectors);
    vectors.read_queries(query_file, queries_vectors);

    DirectedGraph *g;

    // If user gave vamana file, use it to initialize the graph
    if (!vamana_file.empty()) g = read_vamana_from_file(vamana_file);
    else g = vamana(vectors, a, L, R);

    int choice;

    while (true) {
        std::cout << "\n--- Menu ---\n"
                << "1) Find K-nearest neighbors for a query\n"
                << "2) Find K-nearest neighbors for all queries\n"
                << "3) Save Vamana structure to file\n"
                << "4) Exit\n"
                << "Enter choice (1-4): ";
        std::cin >> choice;

        if (choice == 1) { // Single query processing
            int index;
            std::cout << "Enter query index (0," << queries_vectors - 1 << "): ";
            std::cin >> index;

            if (index >= queries_vectors) {
                std::cout << "Invalid index." << std::endl;
                continue;
            }

            auto result = GreedySearch(*g, vectors, 0, index + base_vectors, K, L);
            auto groundtruth = vectors.query_solutions(groundtruth_file, index);
            std::vector<int> difference = findDifference(groundtruth, result.first);
            std::cout << "Recall: " << (static_cast<float>(K - difference.size()) / K) << std::endl;

        } else if (choice == 2) { // All queries processing
            int mismatch_count = 0;
            for (int j = 0; j < queries_vectors; j++) {
                auto result = GreedySearch(*g, vectors, 0, j + base_vectors, K, L);
                auto groundtruth = vectors.query_solutions(groundtruth_file, j);
                mismatch_count += findDifference(groundtruth, result.first).size();
            }
            std::cout << "Recall: " << (static_cast<float>((K * queries_vectors) - mismatch_count) / (K * queries_vectors)) << std::endl;

        } else if (choice == 3) { // Save Vamana structure to file
            std::string filename;
            std::cout << "Enter filename to save Vamana structure: ";
            std::cin >> filename;

            try {
                write_vamana_to_file(*g, filename);;  
                std::cout << "Vamana structure successfully saved to " << filename << std::endl;
            } catch (const std::exception& e) {
                std::cout << "Error saving Vamana structure: " << e.what() << std::endl;
            }

        } else if (choice == 4) { // Exit
            std::cout << "Exiting the program." << std::endl;
            break;

        } else {
            std::cout << "Invalid option." << std::endl;
        }
    }


    delete g;
    return 0;
}