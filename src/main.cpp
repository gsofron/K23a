#include <iostream>
#include "vector.hpp"

int main() {
    std::cout << "Hello K23A" << std::endl;

    int num_vectors;

    auto* float_vectors = MathVector<float>::init_from_file("siftsmall/siftsmall_base.fvecs", num_vectors, 10000);
    std::cout << "Read " << num_vectors << " float vectors." << std::endl;

    if (num_vectors > 0) {
        std::cout << *(*float_vectors)[0] << std::endl;
    }

    auto* uchar_vectors = MathVector<unsigned char>::init_from_file("bigann/queries.bvecs", num_vectors, 10000);
    std::cout << "Read " << num_vectors << " unsigned char vectors." << std::endl;

    if (num_vectors > 235) {
        std::cout << *(*uchar_vectors)[235] << std::endl;
    }

    auto* solutions = MathVector<float>::query_solutions("siftsmall/siftsmall_groundtruth.ivecs", 0);

    destroy_vector(float_vectors);

    destroy_vector(uchar_vectors);

    delete solutions;

    return 0;
}
