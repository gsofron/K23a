#include <iostream>
#include <unistd.h>     // getopt()

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
    int k;
    float a;
    parse_parameters(argc, argv, k, a);

    // After reading fvecs/bvecs
    int total_vectors = 1000;
    if (k >= total_vectors) {
        std::cerr << "k must be smaller than the total amount of vectors" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Parameters are:" << std::endl;
    std::cout << "k = " << k << std::endl;
    std::cout << "Vector dimension = " << vector_dimension << std::endl;
    std::cout << "Vamana a = " << a << std::endl;
    std::cout << "Total vectors = " << total_vectors << std::endl << std::endl;
    
    return 0;
}