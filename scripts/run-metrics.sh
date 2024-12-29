#!/bin/bash

# Check if all arguments are provided
if [ $# -ne 1 ]; then
    echo "Usage: $0 <number of loops>"
    exit 1
fi

# ./scripts/metrics-filtered.sh $1 ./metrics/filtered/default.txt
# ./scripts/metrics-filtered.sh $1 ./metrics/filtered/random_graph.txt --random-graph
# ./scripts/metrics-filtered.sh $1 ./metrics/filtered/limit.txt --limit 40
# ./scripts/metrics-filtered.sh $1 ./metrics/filtered/random_graph_limit.txt --random-graph --limit 40

./scripts/metrics-stitched.sh $1 ./metrics/stitched/default.txt
./scripts/metrics-stitched.sh $1 ./metrics/stitched/random_graph.txt --random-graph
./scripts/metrics-stitched.sh $1 ./metrics/stitched/random_medoid.txt --random-medoid
./scripts/metrics-stitched.sh $1 ./metrics/stitched/random_subset_medoid.txt --random-subset-medoid
./scripts/metrics-stitched.sh $1 ./metrics/stitched/limit.txt --limit 40
./scripts/metrics-stitched.sh $1 ./metrics/stitched/random_graph_limit.txt --random-graph --limit 40