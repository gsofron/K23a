#include <iostream>
#include "vector.hpp"

int main() {
    std::cout << "Hello K23A" << std::endl;
    int num_vectors;

    auto float_vectors = MathVector<float>::init_from_file("siftsmall/siftsmall_base.fvecs", num_vectors, 10000);

    std::cout << "Read " << num_vectors << " float vectors." << std::endl;
    std::cout << *(*float_vectors)[0] << std::endl;


    auto uchar_vectors = MathVector<unsigned char>::init_from_file("bigann/queries.bvecs", num_vectors, 10000);
    
    std::cout << "Read " << num_vectors << " unsigned char vectors." << std::endl;
    std::cout << *(*uchar_vectors)[235] << std::endl;

    auto solutions = MathVector<float>::query_solutions("siftsmall/siftsmall_groundtruth.ivecs", 0);

    return 0;
}