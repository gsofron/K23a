#!/bin/bash

# Simple bash script that executes all unit-tests with valgrind

echo -e "\nExecuting all unit tests (with valgrind)"
valgrind --leak-check=full --show-leak-kinds=all --trace-children=yes ./directed_graph_test
valgrind --leak-check=full --show-leak-kinds=all --trace-children=yes ./vectors_test
valgrind --leak-check=full --show-leak-kinds=all --trace-children=yes ./greedy_search_test
valgrind --leak-check=full --show-leak-kinds=all --trace-children=yes ./filtered_greedy_search_test
valgrind --leak-check=full --show-leak-kinds=all --trace-children=yes ./robust_prune_test
valgrind --leak-check=full --show-leak-kinds=all --trace-children=yes ./filtered_robust_prune_test
valgrind --leak-check=full --show-leak-kinds=all --trace-children=yes ./vamana_test
valgrind --leak-check=full --show-leak-kinds=all --trace-children=yes ./findmedoid_test
valgrind --leak-check=full --show-leak-kinds=all --trace-children=yes ./filtered_vamana_test
