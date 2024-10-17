#pragma once

#include <ostream>
#include <vector>

// If given condition is true exit() the program, printing given messsage
#define ERROR_EXIT(cond, msg)                                                                           \
    if (cond) {                                                                                         \
        std::cerr << "Error in " << __FILE__ << " line " << __LINE__ << " --- " << msg << std::endl;    \
        exit(EXIT_FAILURE);                                                                             \
    }

// Print vector elements using << overloading
template <typename S>
std::ostream &operator<<(std::ostream &os, const std::vector<S> &vector) {
    for (auto v : vector) {
        std::cout << v << " ";
    }
    return os;
}