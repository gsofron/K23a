#include <cstdlib>      // srand()
#include <ctime>        // time(NULL) for srand
#include <iostream>     // std:cerr etc.
#include <unistd.h>     // getopt()

#include "directed_graph.hpp"
#include "robust_prune.hpp"
#include "vector.hpp"

int vector_dimension;

void parse_parameters(int argc, char *argv[], int &k, float &a) {
    int opt;
    // Make sure user gives all parameters
    if (argc != 7) {
        std::cerr << "Usage: " << argv[0] << " -k <k neighbours> -d <vector dimension> -a <Vamana a>" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    // Parse parameters
    while ((opt = getopt(argc, argv, "k:d:a:")) != -1) {
        switch (opt) {
        case 'k':
            k = atoi(optarg);
            if (k <= 0) {
                std::cerr << "k must be positive" << std::endl;
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
        case 'a':
            a = atof(optarg);
            if (a <= 1.0) {
                std::cerr << "Vamana a must be greater than 1" << std::endl;
                exit(EXIT_FAILURE);
            }
            break;
        default:
            std::cerr << "Usage: " << argv[0] << " -k <k neighbours> -d <vector dimension> -a <Vamana a>" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char *argv[]) {
    srand(time(NULL));

    int k;
    float a;
    parse_parameters(argc, argv, k, a);

    // After reading fvecs/bvecs
    int total_vectors = 10;
    if (k >= total_vectors) {
        std::cerr << "k must be smaller than the total amount of vectors" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Parameters are:" << std::endl;
    std::cout << "k = " << k << std::endl;
    std::cout << "Vector dimension = " << vector_dimension << std::endl;
    std::cout << "Vamana a = " << a << std::endl;
    std::cout << "Total vectors = " << total_vectors << std::endl << std::endl;

    // Create 'total_vectors' random MathVectors (create this here to deallocate it later)
    std::cout << "-----Creating " << total_vectors << " random vectors-----" << std::endl;
    std::vector<MathVector <int> *> vectors;
    for (int i = 0 ; i < total_vectors ; i++) {
        MathVector<int> *random_vector = DirectedGraph<int>::random_vector(vector_dimension);
        vectors.push_back(random_vector);
        std::cout << *vectors[i] << std::endl;
    }
    std::cout << std::endl;

    // Create a random graph
    std::cout << "-----Creating a random graph where each vertex has 5 neighbors-----" << std::endl;
    DirectedGraph<int> *g = DirectedGraph<int>::random_graph(vectors, 5);
    std::cout << *g << std::endl;

    // Prune it with R = 3
    for (int i = 0 ; i < total_vectors ; i++) {
        robust_prune<int>(g, vectors[i], {}, a, 3);
    } 

    std::cout << std::endl << "-----New Graph-----" << std::endl;
    std::cout << *g << std::endl;

    // Deallocate memory
    for (int i = 0 ; i < total_vectors ; i++) {
        delete vectors[i];
    }
    delete g;
    return 0;
}