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

// ./k23a -b siftsmall/siftsmall_base.fvecs -q siftsmall/siftsmall_query.fvecs -t 1 -n 5000 -d 128 -k 5 -a 1.3 -l 5 -r 3

void parse_parameters(int argc, char *argv[], std::string &base_file, std::string &query_file, int &field_type, int &total_vectors, int &vector_dimension, int &k, float &a, int &L, int &R) {
    int opt;
    std::ifstream file;

    // Make sure user gives all parameters
    if (argc != 19) {
        std::cerr << "Usage: " << argv[0] << " -b <base vectors file> -q <query vectors file> -t <field type> -n <total vectors> -d <vector dimension> -k <k neighbours> -a <a> -l <L> -r <R>" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    // Parse parameters
    while ((opt = getopt(argc, argv, "b:q:t:n:d:k:a:l:r:")) != -1) {
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
            std::cerr << "Usage: " << argv[0] << " -b <base vectors file> -q <query vectors file> -t <field type> -n <total vectors> -d <vector dimension> -k <k neighbours> -a <a> -l <L> -r <R>" << std::endl;
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
    int total_vectors, vector_dimension, k, L, R, field_type;
    float a;
    std::string base_file, query_file;
    parse_parameters(argc, argv, base_file, query_file, field_type, total_vectors, vector_dimension, k, a, L, R);

    // Read all vectors from file
    int read_vectors;
    Vectors<float> vectors("siftsmall/siftsmall_base.fvecs", read_vectors, total_vectors, 100);
    vectors.read_queries("siftsmall/siftsmall_query.fvecs", 100);

    std::cout << "-----Parameters-----" << std::endl;
    std::cout << "Base file = " << base_file << std::endl;
    std::cout << "Query file = " << query_file << std::endl;
    std::cout << "Field type = " << field_type << std::endl;
    std::cout << "Total vectors = " << total_vectors << std::endl;
    std::cout << "Vectors read = " << read_vectors << std::endl;
    std::cout << "Vector dimension = " << vector_dimension << std::endl;
    std::cout << "k = " << k << std::endl;
    std::cout << "L = " << L << std::endl;
    std::cout << "R = " << R << std::endl;
    std::cout << "a = " << a << std::endl;

    // Get a random R regular graph and convert it to R-1 regular
    DirectedGraph *g = vamana(vectors, a, L, R);

    int sum = 0;
    for ( int j = 0 ; j < 100 ; j++) {
        int index = 10000 + j;
        auto result = GreedySearch(*g, vectors, 0, index, k, L);
        // std::cout << "result   : " << result.first << std::endl;
        // for (auto i = 0 ; i < k ; i++) {
        //     std::cout << vectors.euclidean_distance_cached(index, result.first[i]) << " ";
        // }
        // std::cout << std::endl << std::endl;

        auto s = vectors.query_solutions("siftsmall/siftsmall_groundtruth.ivecs", j);
        // std::cout << "solution : " << s << std::endl;
        // for (auto i = 0 ; i < k ; i++) {
        //     std::cout << vectors.euclidean_distance_cached(index, s[i]) << " ";
        // }
        // std::cout << std::endl << std::endl;

        std::vector<int> difference = findDifference(s, result.first);
        std::cout << "differnce : " << difference.size() << std::endl;
        sum += difference.size();

    }
    std::cout << "sum : " << sum << std::endl;
    // De-allocate memory
    delete g;
    return 0;
}