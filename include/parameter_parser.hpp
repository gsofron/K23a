#pragma once

#include <string>

// Parse input arguments for FilteredVamana
void parse_filtered(int vec_dimension, int k, int argc, char *argv[], std::string &base_file, std::string &query_file, std::string &groundtruth_file, \
                      std::string &vamana_file, std::string &save_file, int &base_vectors_num, int &query_vectors_num, \
                      float &a, int &L, int &t, int &index, int &R, bool &random_graph_flag);
                      
// Parse input arguments for StitchedVamana
void parse_stitched(int vec_dimension, int k, int argc, char *argv[], std::string &base_file, std::string &query_file, std::string &groundtruth_file, \
                      std::string &vamana_file, std::string &save_file, int &base_vectors_num, int &query_vectors_num, \
                      float &a, int &L, int &t, int &index, int &L_small, int &R_small, int &R_stitched, \
                      bool &random_graph_flag, bool &random_medoid_flag, bool &random_subset_medoid_flag);
