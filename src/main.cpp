#include <cstdlib>      // srand()
#include <ctime>        // time(NULL) for srand
#include <fstream>      // ifstream
#include <iostream>     // std:cerr etc.
#include <string>       // std::string
#include <unistd.h>     // getopt()

#include "directed_graph.hpp"
#include "robust_prune.hpp"
#include "vamana.hpp"
#include "vector.hpp"

// ./k23a -f siftsmall/siftsmall_base.fvecs -t 1 -n 5000 -d 128 -k 5 -a 1.3 -l 5 -r 3

void parse_parameters(int argc, char *argv[], std::string &filename, int &field_type, int &total_vectors, int &vector_dimension, int &k, float &a, int &L, int &R) {
    int opt;
    std::ifstream file;

    // Make sure user gives all parameters
    if (argc != 17) {
        std::cerr << "Usage: " << argv[0] << " -f <filename> -t <field type> -n <total vectors> -d <vector dimension> -k <k neighbours> -a <a> -l <L> -r <R>" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    // Parse parameters
    while ((opt = getopt(argc, argv, "f:t:n:d:k:a:l:r:")) != -1) {
        switch (opt) {
        case 'f':
            filename = optarg;
            file.open(filename);
            if (!file) {
                std::cerr << "File doesn't exist" << std::endl;
                exit(EXIT_FAILURE);
            }
            break;
        case 't':
            field_type = atoi(optarg);
            if (field_type < 0 || field_type > 1) {
                std::cerr << "Invalid field type: Type 0 for unsigned char, 1 for float" << std::endl;
                exit(EXIT_FAILURE);
            }
            break;
        case 'n':
            total_vectors = atoi(optarg);
            if (total_vectors <= 0) {
                std::cerr << "Total vectors must be positive" << std::endl;
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
            std::cerr << "Usage: " << argv[0] << " -f <filename> -t <field type> -n <total vectors> -d <vector dimension> -k <k neighbours> -a <a> -l <L> -r <R>" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    // k must be total_vectors-1 or less
    if (k >= total_vectors) {
        std::cerr << "k must be smaller than the total amount of vectors" << std::endl;
        exit(EXIT_FAILURE);
    }

    // In GreedySearch: L >= k
    if (L < k) {
        std::cerr << "L cannot be smaller than k" << std::endl;
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    // This is necessary because we use rand() inside Vamana
    srand(time(NULL));

    // Get command line arguements
    int total_vectors, vector_dimension, k, L, R, field_type;
    float a;
    std::string filename;
    parse_parameters(argc, argv, filename, field_type, total_vectors, vector_dimension, k, a, L, R);

    // Read all vectors from file
    int read_vectors;
    auto *vectors = MathVector<float>::init_from_file(filename, read_vectors, total_vectors);

    std::cout << "-----Parameters-----" << std::endl;
    std::cout << "Filename = " << filename << std::endl;
    std::cout << "Field type = " << field_type << std::endl;
    std::cout << "Total vectors = " << total_vectors << std::endl;
    std::cout << "Vectors read = " << read_vectors << std::endl;
    std::cout << "Vector dimension = " << vector_dimension << std::endl;
    std::cout << "k = " << k << std::endl;
    std::cout << "L = " << L << std::endl;
    std::cout << "R = " << R << std::endl;
    std::cout << "a = " << a << std::endl;

    // Apply Vamana Indexing algorithm to create directed graph G with out-degree <= R
    DirectedGraph<float> *g = vamana(vectors, a, L, R);

    // De-allocate memory
    delete g;
    destroy_vector(vectors);
    return 0;
}