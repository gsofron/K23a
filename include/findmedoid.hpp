#pragma once

#include "vectors.hpp"

std::unordered_map<float, int> *find_medoid(const Vectors &vectors, int threshold);