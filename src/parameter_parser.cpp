#include <fstream>      // ifstream
#include <getopt.h>     // getopt_long
#include <iostream>
#include <limits>

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
    std::cerr << "--random-graph" << std::endl;
    std::cerr << "--limit <unfiltered queries search limit>" << std::endl;
    #ifndef FILTERED_VAMANA
    std::cerr << "--random-medoid" << std::endl;
    std::cerr << "--random-subset-medoid" << std::endl;
    #endif
}

// Parse input arguments for FilteredVamana
void parse_filtered(int vec_dimension, int k, int argc, char *argv[], std::string &base_file, std::string &query_file, std::string &groundtruth_file, \
                      std::string &vamana_file, std::string &save_file, int &base_vectors_num, int &query_vectors_num, \
                      float &a, int &L, int &t, int &index, int &R, bool &random_graph_flag, int &limit) {
    // Common command line mandatory flags
    bool base_file_flag = false, query_file_flag = false, groundtruth_file_flag = false, base_vectors_num_flag = false, \
         query_vectors_num_flag = false, a_flag = false, L_flag = false, t_flag = false, index_flag = false;
    bool R_flag = false;    // Extra mandatory flag for FilteredVamana
         
    // For FilteredVamana, minimum arguements are 21 and maximum are 28
    if (argc < 21 || argc > 28) {
        print_usage();
        exit(EXIT_FAILURE);
    }

    // Define long options
    struct option long_options[] = {
        {"random-graph", no_argument, nullptr, 1},
        {"limit", required_argument, nullptr, 2},
        {nullptr, 0, nullptr, 0}    // Terminating null entry
    };

    // Parse arguments using getopt
    int opt;
    std::ifstream file;
    while ((opt = getopt_long(argc, argv, "b:q:g:v:s:n:m:a:L:t:i:R:", long_options, nullptr)) != -1) {
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
            if (L < k) {
                std::cerr << "L cannot be smaller than k" << std::endl;
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
        case 1: // Init graph with random edges
            random_graph_flag = true;
            break;
        case 2: // Set search limit for unfiltered queries
            limit = std::stoi(optarg);
            if (limit <= 0) {
                std::cerr << "limit must be positive" << std::endl;
                exit(EXIT_FAILURE);
            }
            break;
        default:
            print_usage();
            exit(EXIT_FAILURE);
        }
    }

    // Check for unexpected arguments
    if (optind < argc) {
        std::cerr << "Error: Unexpected argument \"" << argv[optind] << "\" provided." << std::endl;
        exit(EXIT_FAILURE);
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
    std::cout << "Vector dimension = " << vec_dimension << std::endl;
    std::cout << "k = " << k << std::endl;
    std::cout << "Base vectors number = " << base_vectors_num << std::endl;
    std::cout << "Query vectors number = " << query_vectors_num << std::endl;
    std::cout << "a = " << a << std::endl;
    std::cout << "L = " << L << std::endl;
    std::cout << "R = " << R << std::endl;
    std::cout << "t = " << t << std::endl;
    std::cout << "index = " << index << std::endl;
    if (random_graph_flag) std::cout << "Using random graph for FilteredVamana initialization" << std::endl;
    if (limit != std::numeric_limits<int>::max()) std::cout << "Using unfiltered query limit: " << limit << std::endl;
    std::cout << std::endl;
}

// Parse input arguments for StitchedVamana
void parse_stitched(int vec_dimension, int k, int argc, char *argv[], std::string &base_file, std::string &query_file, std::string &groundtruth_file, \
                      std::string &vamana_file, std::string &save_file, int &base_vectors_num, int &query_vectors_num, \
                      float &a, int &L, int &t, int &index, int &L_small, int &R_small, int &R_stitched, \
                      bool &random_graph_flag, bool &random_medoid_flag, bool &random_subset_medoid_flag, int &limit) {
    // Common command line mandatory flags
    bool base_file_flag = false, query_file_flag = false, groundtruth_file_flag = false, base_vectors_num_flag = false, \
         query_vectors_num_flag = false, a_flag = false, L_flag = false, t_flag = false, index_flag = false;
    bool L_small_flag = false, R_small_flag = false, R_stitched_flag = false;   // Extra mandatory flags for FilteredVamana
         

    // For StitchedVamana, minimum arguements are 25 and maximum are 33
    if (argc < 25 || argc > 33) {
        print_usage();
        exit(EXIT_FAILURE);
    }

    // Define long options
    struct option long_options[] = {
        {"random-graph", no_argument, nullptr, 1},
        {"random-medoid", no_argument, nullptr, 2},
        {"random-subset-medoid", no_argument, nullptr, 3},
        {"limit", required_argument, nullptr, 4},
        {nullptr, 0, nullptr, 0}    // Terminating null entry
    };

    // Parse arguments using getopt
    int opt;
    std::ifstream file;
    while ((opt = getopt_long(argc, argv, "b:q:g:v:s:n:m:a:L:t:i:l:r:R:", long_options, nullptr)) != -1) {
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
            if (L < k) {
                std::cerr << "L cannot be smaller than k" << std::endl;
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
        case 1: // Use a graph with random edges for StitchedVamana
            random_graph_flag = true;
            break;
        case 2: // Use a random medoid for Vamana
            random_medoid_flag = true;
            break;
        case 3: // Use a medoid of a random subset for Vamana
            random_subset_medoid_flag = true;
            break;
        case 4: // Set search limit for unfiltered queries
            limit = std::stoi(optarg);
            if (limit <= 0) {
                std::cerr << "limit must be positive" << std::endl;
                exit(EXIT_FAILURE);
            }
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

    // User cannot give both flags
    if (random_medoid_flag && random_subset_medoid_flag) {
        std::cerr << "Flags --random-medoid and --random-subset-medoid cannot be used together" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Output parameters
    std::cout << "-----Parameters-----" << std::endl;
    std::cout << "Base file = " << base_file << std::endl;
    std::cout << "Query file = " << query_file << std::endl;
    std::cout << "Groundtruth file = " << groundtruth_file << std::endl;
    std::cout << "Vamana file = " << vamana_file << std::endl;
    std::cout << "Save file = " << save_file << std::endl;
    std::cout << "Vector dimension = " << vec_dimension << std::endl;
    std::cout << "k = " << k << std::endl;
    std::cout << "Base vectors number = " << base_vectors_num << std::endl;
    std::cout << "Query vectors number = " << query_vectors_num << std::endl;
    std::cout << "a = " << a << std::endl;
    std::cout << "L = " << L << std::endl;
    std::cout << "Lsmall = " << L_small << std::endl;
    std::cout << "Rsmall = " << R_small << std::endl;
    std::cout << "Rstitched = " << R_stitched << std::endl;
    std::cout << "t = " << t << std::endl;
    std::cout << "index = " << index << std::endl;
    if (random_graph_flag) std::cout << "Using random graph for StitchedVamana initialization" << std::endl;
    if (random_medoid_flag) std::cout << "Using random medoid for FindMedoid initialization" << std::endl;
    if (random_subset_medoid_flag) std::cout << "Using a random subset of medoids for FindMedoid initialization" << std::endl;
    if (limit != std::numeric_limits<int>::max()) std::cout << "Using unfiltered query limit: " << limit << std::endl;
    std::cout << std::endl;
}