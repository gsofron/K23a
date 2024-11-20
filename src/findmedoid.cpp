#include <cstdlib>
#include <ctime>
#include <map>
#include <vector>

#include "findmedoid.hpp"
#include "utils.hpp"

std::unordered_map<float, int> *find_medoid(const Vectors &vectors, int threshold) {
    std::srand(std::time(nullptr));
    
    // Map M mapping filters to start nodes: key=filter, value:start-index
    std::unordered_map<float, int> *M = new std::unordered_map<float, int>;

    // Counter map T: key=index, value=counter
    std::unordered_map<int, int> T;

    // For each filter, Rf contains `threshold` num of elements
    int *Rf = new int[threshold];

    for (const auto& pair : vectors.filters_map) { // For every filter
        float f = pair.first; // Current filter
        const std::unordered_set<int>& Pf = pair.second; // Indexes matching the filter

        // Select `threshold` random samples from Pf
        // We need to conver Pf to a vector to work with array-based indexes
        std::vector<int> Pf_vector(Pf.begin(), Pf.end());
        for (int i = 0, index; i < threshold; i++) {
            index = std::rand() % Pf_vector.size();
            Rf[i] = Pf_vector[index];
            // Remove picked element from vector to ensure that it won't be selected again
            Pf_vector.erase(Pf_vector.begin() + index);
        }

        int p_star = Rf[0];
        for (int i = 1; i < threshold; i++)
            if (T[Rf[i]] < T[p_star])
                p_star = Rf[i];

        (*M)[f] = p_star;
        T[p_star]++;
    }

    delete[] Rf;

    return M;
}