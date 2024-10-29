#pragma once

#include <ostream>
#include <unordered_set>
#include <vector>

#include "vectors.hpp"

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

// Print unordered-set's elements using << overloading
template<typename T>
std::ostream& operator<<(std::ostream& os, const std::unordered_set<T>& uset) {
    os << "{ ";
    for (auto it = uset.begin(); it != uset.end(); it++) {
        os << *it;
        if (std::next(it) != uset.end()) os << ", ";
    }
    os << " }";
    return os;
}

//Custom comparator for vectors to a query vector
template <typename T>
struct CompareDistance {
    int query;  
    Vectors<T>& vectors;

    CompareDistance(int query, Vectors<T>& vectors) : query(query), vectors(vectors) {}

    bool operator()(int a, int b) const {
        float distance_a = vectors.euclidean_distance_cached(query, a);
        float distance_b = vectors.euclidean_distance_cached(query, b);
        return (distance_a < distance_b || 
                (distance_a == distance_b && a < b));
    }
};