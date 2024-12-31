#pragma once

#include <set>
#include "vectors.hpp"        
#include "directed_graph.hpp" 

DirectedGraph *stitched_vamana(Vectors& P, float a, int L_small, int R_small, int R_stitched, bool random_graph_flag, bool random_medoid_flag, bool random_subset_medoid_flag, int limit);