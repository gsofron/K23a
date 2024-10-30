#include <cstdlib>      // srand()
#include <ctime>        // time(NULL) for srand
#include <fstream>      // ifstream
#include <iostream>     // std:cerr etc.
#include <string>       // std::string
#include <unistd.h>     // getopt()

#include "directed_graph.hpp"
#include "robust_prune.hpp"
#include "utils.hpp"
#include "vamana.hpp"
#include "vectors.hpp"

// time ./k23a -b siftsmall/siftsmall_base.fvecs -q siftsmall/siftsmall_query.fvecs -g siftsmall/siftsmall_groundtruth.ivecs -t 1 -n 10000 -d 128 -k 100 -a 1.1 -l 150 -r 100
// ~30-40 seconds

void parse_parameters(int argc, char *argv[], std::string &base_file, std::string &query_file, std::string &groundtruth_file, \
                        int &field_type, int &base_vectors, int &queries_vectors, int &vector_dimension, int &k, float &a, int &L, int &R) {
    int opt;
    std::ifstream file;

    // Make sure user gives all parameters
    if (argc != 23) {
        std::cerr << "Usage: " << argv[0] << " -b <base vectors file> -q <query vectors file> -g <groundtruth vectors file>" 
        "-t <field type> -n <base vectors num> -m <queries num> -d <vector dimension> -k <k neighbours> -a <a> -l <L> -r <R>" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    // Parse parameters
    while ((opt = getopt(argc, argv, "b:q:g:t:n:m:d:k:a:l:r:")) != -1) {
        switch (opt) {
        case 'b':
            base_file = optarg;
            file.open(base_file);
            if (!file) {
                std::cerr << "Base vectors file doesn't exist" << std::endl;
                exit(EXIT_FAILURE);
            }
            file.close();
            break;
        case 'q':
            query_file = optarg;
            file.open(query_file);
            if (!file) {
                std::cerr << "Query vectors file doesn't exist" << std::endl;
                exit(EXIT_FAILURE);
            }
            file.close();
            break;
        case 'g':
            groundtruth_file = optarg;
            file.open(groundtruth_file);
            if (!file) {
                std::cerr << "Groundtruth vectors file doesn't exist" << std::endl;
                exit(EXIT_FAILURE);
            }
            file.close();
            break;
        case 't':
            field_type = atoi(optarg);
            if (field_type < 0 || field_type > 1) {
                std::cerr << "Invalid field type: Type 0 for unsigned char, 1 for float" << std::endl;
                exit(EXIT_FAILURE);
            }
            break;
        case 'n':
            base_vectors = atoi(optarg);
            if (base_vectors <= 0) {
                std::cerr << "Total vectors must be positive" << std::endl;
                exit(EXIT_FAILURE);
            }
            break;
        case 'm':
            queries_vectors = atoi(optarg);
            if (queries_vectors <= 0) {
                std::cerr << "Queries vectors must be positive" << std::endl;
                exit(EXIT_FAILURE);
            }
            break;
        case 'd':
            vector_dimension = atoi(optarg);
            if (vector_dimension <= 0) {
                std::cerr << "Vector dimension must be positive" << std::endl;
                exit(EXIT_FAILURE);
            }
            break;
        case 'k':
            k = atoi(optarg);
            if (k <= 0) {
                std::cerr << "k must be positive" << std::endl;
                exit(EXIT_FAILURE);
            }
            break;
        case 'a':
            a = atof(optarg);
            if (a < 1.0) {
                std::cerr << "a cannot be smaller than 1" << std::endl;
                exit(EXIT_FAILURE);
            }
            break;
        case 'l':
            L = atoi(optarg);
            if (L <= 0) {
                std::cerr << "L must be positive" << std::endl;
                exit(EXIT_FAILURE);
            }
            break;
        case 'r':
            R = atoi(optarg);
            if (R <= 0) {
                std::cerr << "R must be positive" << std::endl;
                exit(EXIT_FAILURE);
            }
            break;
        default:
            std::cerr << "Usage: " << argv[0] << " -b <base vectors file> -q <query vectors file> -g <groundtruth vectors file>" 
            "-t <field type> -n <total vectors> -d <vector dimension> -k <k neighbours> -a <a> -l <L> -r <R>" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    // k must be base_vectors-1 or less
    if (k >= base_vectors) {
        std::cerr << "k must be smaller than the total amount of vectors" << std::endl;
        exit(EXIT_FAILURE);
    }

    // In GreedySearch: L >= k
    if (L < k) {
        std::cerr << "L cannot be smaller than k" << std::endl;
        exit(EXIT_FAILURE);
    }
}


std::vector<int> findDifference(const std::vector<int>& a, const std::vector<int>& b) {
    std::unordered_set<int> setB(b.begin(), b.end()); // Load all elements of b into a set
    std::vector<int> result;

    // Add elements from a that are not in b
    for (int num : a) {
        if (setB.find(num) == setB.end()) {
            result.push_back(num);
        }
    }

    return result;
}

int main(int argc, char *argv[]) {
    // This is necessary because we use rand() inside Vamana
    srand(time(NULL));

    // Get command line arguements
    int base_vectors, queries_vectors, vector_dimension, k, L, R, field_type;
    float a;
    std::string base_file, query_file, groundtruth_file;
    parse_parameters(argc, argv, base_file, query_file, groundtruth_file, field_type, base_vectors, queries_vectors, vector_dimension, k, a, L, R);

    // Read all vectors from file
    int read_vectors;
    Vectors<float> vectors("siftsmall/siftsmall_base.fvecs", read_vectors, base_vectors, queries_vectors);
    vectors.read_queries("siftsmall/siftsmall_query.fvecs", queries_vectors);

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

    // Get a random R regular graph and convert it to R-1 regular
    DirectedGraph *g = vamana(vectors, a, L, R);


    int choice;
    while (true) {
        // Display menu
        std::cout << "\n--- Menu ---" << std::endl;
        std::cout << "1) Find the k-nearest neighbors of one query" << std::endl;
        std::cout << "2) Find the k-nearest neighbors of all the queries" << std::endl;
        std::cout << "3) Exit" << std::endl;
        std::cout << "Enter your choice (1-3): ";
        std::cin >> choice;

        // Process choice
        if (choice == 1) {
            int index;
            std::cout << "Option 1 selected: Find the k-nearest neighbors of an index" << std::endl;
            std::cout << "Enter the index (0," << queries_vectors-1 << "): ";
            std::cin >> index;

            if (index >= queries_vectors) {
                std::cout << "Invalid choice. Please select a valid index." << std::endl;
                continue;
            }
            // Here, add logic for finding k-nearest neighbors of the specified index
            std::cout << "Finding the k-nearest neighbors of index " << index << "..." << std::endl;

            auto result = GreedySearch(*g, vectors, 0, index + base_vectors, k, L);

            auto s = vectors.query_solutions(groundtruth_file, index);

            std::vector<int> difference = findDifference(s, result.first);
            std::cout << "Recall : " << (static_cast<float>(k - difference.size()) / k) << std::endl;

        } else if (choice == 2) {
            std::cout << "Option 2 selected: Find the k-nearest neighbors" << std::endl;
            int sum = 0;
            for ( int j = 0 ; j < queries_vectors ; j++) {
                int index = base_vectors + j;
                auto result = GreedySearch(*g, vectors, 0, index, k, L);

                auto s = vectors.query_solutions(groundtruth_file, j);

                std::vector<int> difference = findDifference(s, result.first);
                sum += difference.size();

            }
            std::cout << "Recall : " << (static_cast<float>((k * queries_vectors) - sum) / (k * queries_vectors)) << std::endl;
        } else if (choice == 3) {
            std::cout << "Exiting the program." << std::endl;
            break; // Exit the loop
        } else {
            std::cout << "Invalid choice. Please select an option from 1 to 3." << std::endl;
        }
    }
    
    
    delete g;
    return 0;
}