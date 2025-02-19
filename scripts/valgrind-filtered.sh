#!/bin/bash

# Executes the filtered vamana executable with 3 different parameter versions

echo -e "\nCreate filtered vamana from scratch, without saving the graph to a file. (valgrind)"
valgrind --leak-check=full --show-leak-kinds=all ./filtered -b dummy/dummy-data.bin -q dummy/dummy-queries.bin -g dummy/dummy-groundtruth.bin -n 10000 -m 5012 -a 1.1 -L 150 -R 12 -t 50 -i -1
