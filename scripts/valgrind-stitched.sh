#!/bin/bash

# Executes the stitched vamana executable with 3 different parameter versions, using valgrind

echo -e "\nCreate stitched vamana from scratch, without saving the graph to a file. (valgrind)"
valgrind --leak-check=full --show-leak-kinds=all ./stitched -b dummy/dummy-data.bin -q dummy/dummy-queries.bin -g dummy/dummy-groundtruth.bin -n 10000 -m 5012 -a 1.1 -L 150 -l 100 -r 32 -R 64 -t 50 -i -1
