#pragma once

#include <limits>   // float max

#include "directed_graph.hpp"
#include "vector.hpp"

//TODO: OPTIMIZE NEAREST POINT IN SET BY SORTING V AND REMOVING THE FIRST ELEMENT

// Returns a point from set 'V' which is nearest to point 'p'
// Returns nullptr if set is empty
template <typename T>
MathVector<T> *nearest_point_in_set(MathVector<T> *p, std::unordered_set<MathVector<T> *> V) {
    float min_distance = std::numeric_limits<float>::max();
    MathVector<T> *nearest_point = nullptr;

    for (MathVector<T> *point : V) {
        float distance = p->euclidean_distance(*point);
        if (distance < min_distance) {
            min_distance = distance;
            nearest_point = point;
        }
    }

    // std::cout << "Nearest point found to be " << *nearest_point << " with distance " << min_distance << std::endl;
    return nearest_point;
}

// Modifies graph 'g' by setting at most 'R' new out-neighbors for point 'p'
// 'V' is a candidate set containing vertices and 'a' is the distance threshold where a >= 1
template <typename T>
void robust_prune(DirectedGraph<T> *g, MathVector<T> *p, std::unordered_set<MathVector<T> *> V, float a, int R) {
    ERROR_EXIT(a < 1.0, "a cannot be less than 1");
    ERROR_EXIT(R < 1, "R cannot be less than 1");

    const std::unordered_set<MathVector<T> *>& n_out_p = g->get_neighbors(p);

    // V <- (V U Nout(p)) \ {p}
    for (auto vector : n_out_p) {
        V.insert(vector);
    }
    V.erase(p);

    // Nout(p) <- empty set
    for (auto it = n_out_p.begin() ; it != n_out_p.end() ; it = n_out_p.begin()) {
        MathVector<T> *vector = *it;
        g->remove(p, vector);
    }

    // while V not empty
    while (!V.empty()) {
        // std::cout << "V currently is " << V << std::endl;
        // std::cout << "Nout(p) currently is " << n_out_p << std::endl;

        // p* <- arg min d(p, p') where p' in V
        MathVector<T> *p_star = nearest_point_in_set(p, V);

        // Nout(p) <- Nout(p) U {p*}
        g->insert(p, p_star);

        // if |Nout(p)| = R then break
        // std::cout << "Checking for R..." << std::endl;
        if ((int)n_out_p.size() == R) break;

        // for p' in V do
        // std::cout << "Approaching final loop..." << std::endl;
        for (auto it = V.begin() ; it != V.end() ; ) {
            MathVector<T> *point = *it;

            // if a ⋅ d(p*, p') <= d(p, p') then remove p' from V
            if (a * p_star->euclidean_distance(*point) <= p->euclidean_distance(*point)) {
                // std::cout << "Removing " << *point << " from V" << std::endl;
                it = V.erase(it);
            }
            else it++;
        }

        // std::cout << "---loop end---" << std::endl << std::endl;
    }
}