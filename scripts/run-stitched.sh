#!/bin/bash

# Executes the stitched vamana executable with 3 different parameter versions

echo -e "\nVersion 1: Create stitched vamana from scratch, without saving the graph to a file."
time ./stitched -b dummy/dummy-data.bin -q dummy/dummy-queries.bin -g dummy/dummy-groundtruth.bin -n 10000 -m 5012 -a 1.1 -L 150 -l 100 -r 32 -R 64 -t 50 -i -1

echo -e "\nVersion 2: Create stitched vamana from scratch and save it to a the vamana.bin file."
time ./stitched -b dummy/dummy-data.bin -q dummy/dummy-queries.bin -g dummy/dummy-groundtruth.bin -s vamana.bin -n 10000 -m 5012 -a 1.1 -L 150 -l 100 -r 32 -R 64 -t 50 -i -1

echo -e "\nVersion 3: Reads stitched vamana from vamana.bin file."
time ./stitched -b dummy/dummy-data.bin -q dummy/dummy-queries.bin -g dummy/dummy-groundtruth.bin -v vamana.bin -n 10000 -m 5012 -a 1.1 -L 150 -l 100 -r 32 -R 64 -t 50 -i -1