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

// Command line: ./k23a -b <base file> -q <query file> -g <groundtruth file> -t <field type> -n <num base vectors> 
//               -d <dim> -k <neighbors> -a <alpha> -l <L> -r <R>

// Parse input arguments and load necessary parameters
void parse_parameters(int argc, char *argv[], std::string &base_file, std::string &query_file, 
                      std::string &groundtruth_file, int &field_type, int &base_vectors, int &queries_vectors, 
                      int &vector_dimension, int &k, float &a, int &L, int &R) {
    int opt;
    std::ifstream file;

    // Require 23 arguments for full input specification
    if (argc != 23) {
        std::cerr << "Usage: " << argv[0] << " -b <base file> -q <query file> -g <groundtruth file> "
                     "-t <field type> -n <num base vectors> -m <num queries> -d <dimension> "
                     "-k <neighbors> -a <alpha> -l <L> -r <R>" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    // Parse arguments using getopt
    while ((opt = getopt(argc, argv, "b:q:g:t:n:m:d:k:a:l:r:")) != -1) {
        switch (opt) {
        case 'b': // Base vectors file
            base_file = optarg;
            if (!std::ifstream(base_file)) {
                std::cerr << "Base vectors file doesn't exist" << std::endl;
                exit(EXIT_FAILURE);
            }
            break;
        case 'q': // Query vectors file
            query_file = optarg;
            if (!std::ifstream(query_file)) {
                std::cerr << "Query vectors file doesn't exist" << std::endl;
                exit(EXIT_FAILURE);
            }
            break;
        case 'g': // Groundtruth vectors file
            groundtruth_file = optarg;
            if (!std::ifstream(groundtruth_file)) {
                std::cerr << "Groundtruth file doesn't exist" << std::endl;
                exit(EXIT_FAILURE);
            }
            break;
        case 't': // Field type
            field_type = std::stoi(optarg);
            if (field_type < 0 || field_type > 1) {
                std::cerr << "Invalid field type: 0 for unsigned char, 1 for float" << std::endl;
                exit(EXIT_FAILURE);
            }
            break;
        case 'n': // Base vectors count
            base_vectors = std::stoi(optarg);
            if (base_vectors <= 0) {
                std::cerr << "Number of base vectors must be positive" << std::endl;
                exit(EXIT_FAILURE);
            }
            break;
        case 'm': // Query vectors count
            queries_vectors = std::stoi(optarg);
            if (queries_vectors <= 0) {
                std::cerr << "Number of query vectors must be positive" << std::endl;
                exit(EXIT_FAILURE);
            }
            break;
        case 'd': // Vector dimension
            vector_dimension = std::stoi(optarg);
            if (vector_dimension <= 0) {
                std::cerr << "Vector dimension must be positive" << std::endl;
                exit(EXIT_FAILURE);
            }
            break;
        case 'k': // Number of neighbors
            k = std::stoi(optarg);
            if (k <= 0 || k >= base_vectors) {
                std::cerr << "k must be positive and smaller than total base vectors" << std::endl;
                exit(EXIT_FAILURE);
            }
            break;
        case 'a': // Alpha value
            a = std::stof(optarg);
            if (a < 1.0) {
                std::cerr << "Alpha value cannot be less than 1" << std::endl;
                exit(EXIT_FAILURE);
            }
            break;
        case 'l': // Greedy search limit L
            L = std::stoi(optarg);
            if (L < k) {
                std::cerr << "L cannot be smaller than k" << std::endl;
                exit(EXIT_FAILURE);
            }
            break;
        case 'r': // Graph degree limit R
            R = std::stoi(optarg);
            if (R <= 0) {
                std::cerr << "R must be positive" << std::endl;
                exit(EXIT_FAILURE);
            }
            break;
        default:
            std::cerr << "Usage: " << argv[0] << " -b <base file> -q <query file> -g <groundtruth file> "
                         "-t <field type> -n <num base vectors> -d <dimension> -k <neighbors> -a <alpha> "
                         "-l <L> -r <R>" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
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
    int base_vectors, queries_vectors, vector_dimension, k, L, R, field_type;
    float a;
    std::string base_file, query_file, groundtruth_file;
    parse_parameters(argc, argv, base_file, query_file, groundtruth_file, field_type, base_vectors, queries_vectors, vector_dimension, k, a, L, R);

    // Load vectors
    int read_vectors;
    Vectors<float> vectors("siftsmall/siftsmall_base.fvecs", read_vectors, base_vectors, queries_vectors);
    vectors.read_queries("siftsmall/siftsmall_query.fvecs", queries_vectors);

    // Output parameters
    std::cout << "-----Parameters-----" << std::endl;
    std::cout << "Base file = " << base_file << std::endl;
    std::cout << "Query file = " << query_file << std::endl;
    std::cout << "Groundtruth file = " << groundtruth_file << std::endl;
    std::cout << "Field type = " << field_type << std::endl;
    std::cout << "Base vectors = " << base_vectors << std::endl;
    std::cout << "Queries vectors = " << queries_vectors << std::endl;
    std::cout << "Vectors read = " << read_vectors << std::endl;
    std::cout << "Vector dimension = " << vector_dimension << std::endl;
    std::cout << "k = " << k << std::endl;
    std::cout << "L = " << L << std::endl;
    std::cout << "R = " << R << std::endl;
    std::cout << "a = " << a << std::endl;

    DirectedGraph *g;

    int choice;
    
    while (true) {
        std::cout << "\n--- Vamana Initialization ---\n"
                  << "1) Read Vamana structure from file\n"
                  << "2) Create a new Vamana structure\n"
                  << "Enter choice (1-3): ";
        std::cin >> choice;

        if (choice == 1) { // Option to read Vamana from file
            std::string filename;
            std::cout << "Enter filename to read Vamana structure: ";
            std::cin >> filename;

            g = read_vamana_from_file(filename);
            break;
        } else if (choice == 2) { // Option to create a new Vamana structure
            std::cout << "Creating Vamana..." << std::endl;
            g = vamana(vectors, a, L, R);
            break;
        } else {
            std::cout << "Invalid option. Please enter 1, 2." << std::endl;
        }
    }

    while (true) {
        std::cout << "\n--- Menu ---\n"
                << "1) Find k-nearest neighbors for a query\n"
                << "2) Find k-nearest neighbors for all queries\n"
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

            auto result = GreedySearch(*g, vectors, 0, index + base_vectors, k, L);
            auto groundtruth = vectors.query_solutions(groundtruth_file, index);
            std::vector<int> difference = findDifference(groundtruth, result.first);
            std::cout << "Recall: " << (static_cast<float>(k - difference.size()) / k) << std::endl;

        } else if (choice == 2) { // All queries processing
            int mismatch_count = 0;
            for (int j = 0; j < queries_vectors; j++) {
                auto result = GreedySearch(*g, vectors, 0, j + base_vectors, k, L);
                auto groundtruth = vectors.query_solutions(groundtruth_file, j);
                mismatch_count += findDifference(groundtruth, result.first).size();
            }
            std::cout << "Recall: " << (static_cast<float>((k * queries_vectors) - mismatch_count) / (k * queries_vectors)) << std::endl;

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