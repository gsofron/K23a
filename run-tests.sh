#!/bin/bash

# Simple bash script that executes all unit-tests

./directed_graph_test
./math_vector_test
./greedy_search_test
./robust_prune_test
./vamana_test

# To run with `valgrind` execute:
# valgrind --leak-check=full --trace-children=yes ./run-tests.sh