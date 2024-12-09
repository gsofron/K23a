#include <cstdlib>
#include <ctime>
#include <vector>

#include "findmedoid.hpp"
#include "utils.hpp"

// Alternative implementation of FindMedoid(), leveraging the fact that each
// vector (index) maps to only one filter
std::unordered_map<float, int> *find_medoid(const Vectors &vectors, int threshold) {
    std::srand(std::time(nullptr));
    
    // Map M mapping filters to start nodes: key=filter, value:start-index
    std::unordered_map<float, int> *M = new std::unordered_map<float, int>;

    // For each filter, Rf contains `threshold` num of elements
    int *Rf = new int[threshold];

    for (const auto& pair : vectors.filters_map) { // For every filter
        float f = pair.first; // Current filter
        const std::unordered_set<int>& Pf = pair.second; // Indexes matching the filter

        // min(threshold, Pf.size())
        int n = Pf.size();
        if (threshold < n)
            n = threshold;

        // Select `n` random samples from Pf
        // We need to conver Pf to a vector to work with array-based indexes
        int index;
        std::vector<int> Pf_vector(Pf.begin(), Pf.end());
        for (int i = 0; i < n; i++) {
            index = std::rand() % Pf_vector.size();
            Rf[i] = Pf_vector[index];
            // Remove picked element from vector to ensure that it won't be selected again
            Pf_vector.erase(Pf_vector.begin() + index);
        }

        // Out of the randomly selected vectors, choose one as medoid
        index = std::rand() % n;
        (*M)[f] = Rf[index];
    }

    delete[] Rf;

    return M;
}