#!/bin/bash

# Simple bash script that executes all unit-tests

./directed_graph_test
./robust_prune_test

# To run with `valgrind` execute:
# valgrind --leak-check=full --trace-children=yes ./run-tests.sh