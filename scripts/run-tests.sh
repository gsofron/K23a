#!/bin/bash

# Simple bash script that executes all unit-tests

echo -e "\nExecuting all unit tests"
./directed_graph_test
./vectors_test
./greedy_search_test
./filtered_greedy_search_test
./robust_prune_test
./filtered_robust_prune_test
./vamana_test
./findmedoid_test
./filtered_vamana_test
