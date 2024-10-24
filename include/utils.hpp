#pragma once

#include <ostream>
#include <unordered_set>
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

// Print unordered-set's elements using << overloading
// WARNING: The unordered set contains pointers as elements
template<typename T>
std::ostream& operator<<(std::ostream& os, const std::unordered_set<T>& uset) {
    os << "{ ";
    for (auto it = uset.begin(); it != uset.end(); it++) {
        os << **it;
        if (std::next(it) != uset.end()) os << ", ";
    }
    os << " }";
    return os;
}

//Custom comparator for vectors to a query vector
template <typename T>
struct CompareDistance {
    MathVector<T>* query;  

    CompareDistance(MathVector<T> *query) : query(query) {}

    bool operator()(const MathVector<T> *a, const MathVector<T> *b) const {
        float distance_a = a->euclidean_distance(*query);
        float distance_b = b->euclidean_distance(*query);
        return (distance_a < distance_b || 
                (distance_a == distance_b && a < b));
    }
};