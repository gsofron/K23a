#!/bin/bash
# ./scripts/metrics-filtered.sh 10 ./metrics/filtered/default.txt
# ./scripts/metrics-filtered.sh 10 ./metrics/filtered/random_graph.txt --random-graph
# ./scripts/metrics-filtered.sh 10 ./metrics/filtered/limit.txt --limit 40
# ./scripts/metrics-filtered.sh 10 ./metrics/filtered/random_graph_limit.txt --random-graph --limit 40

# Check if all mandatory arguments are provided
if [ $# -lt 2 ]; then
    echo "Usage: $0 <number of loops> <filename> [flags...]"
    exit 1
fi

# Validate if the first argument is a positive integer
if ! [[ "$1" =~ ^[0-9]+$ ]]; then
    echo "Error: The number of loops must be a positive integer."
    exit 1
fi

# Initialize variables
count=$1
filename=$2
>$filename

command="./filtered -b dummy/dummy-data.bin -q dummy/dummy-queries.bin -g dummy/dummy-groundtruth.bin -s vamana.bin -n 10000 -m 5012 -a 1.1 -L 150 -R 12 -t 50 -i -1"
# Loop through flags
for (( i=3; i<=$#; i++ )); do
    command+=" ${!i}"
done

build_time_sum=0
filtered_queries_time_sum=0
filtered_queries_recall_sum=0
unfiltered_queries_time_sum=0
unfiltered_queries_recall_sum=0
recall_sum=0
query_time_sum=0
file_size_sum=0

echo "Executing command: $command"

# Run as many times as user requested
for ((i = 1; i <= count; i++)); do
    # Get metrics
    rm -rf vamana.bin
    output=$($command)
    build_time=$(echo "$output" | grep "Build time:" | awk '{print $3}')
    filtered_queries_time=$(echo "$output" | grep "Filtered queries time:" | awk '{print $4}')
    filtered_queries_recall=$(echo "$output" | grep "Filtered queries recall:" | awk '{print $4}' | tr -d '%')
    unfiltered_queries_time=$(echo "$output" | grep "Unfiltered queries time:" | awk '{print $4}')
    unfiltered_queries_recall=$(echo "$output" | grep "Unfiltered queries recall:" | awk '{print $4}' | tr -d '%')
    recall=$(echo "$output" | grep "Total Recall Percent:" | awk '{print $4}' | tr -d '%')
    query_time=$(echo "$output" | grep "Total query time:" | awk '{print $4}')
    file_size=$(ls -l vamana.bin | awk '{print $5}')

    # Write metrics to file
    echo -e "Execution #$i:" >> $filename
    echo -e "\tBuild Time: $build_time seconds" >> $filename
    echo -e "\tFiltered Queries Time: $filtered_queries_time seconds" >> $filename
    echo -e "\tFiltered Queries Recall: $filtered_queries_recall %" >> $filename
    echo -e "\tUnfiltered Queries Time: $unfiltered_queries_time seconds" >> $filename
    echo -e "\tUnfiltered Queries Recall: $unfiltered_queries_recall %" >> $filename
    echo -e "\tTotal Recall: $recall %" >> $filename
    echo -e "\tTotal Query Time: $query_time seconds" >> $filename
    echo -e "\tFile size: $file_size bytes" >> $filename
    echo >> $filename

    # Update total metrics
    build_time_sum=$(echo "$build_time_sum + $build_time" | bc)
    filtered_queries_time_sum=$(echo "$filtered_queries_time_sum + $filtered_queries_time" | bc)
    filtered_queries_recall_sum=$(echo "$filtered_queries_recall_sum + $filtered_queries_recall" | bc)
    unfiltered_queries_time_sum=$(echo "$unfiltered_queries_time_sum + $unfiltered_queries_time" | bc)
    unfiltered_queries_recall_sum=$(echo "$unfiltered_queries_recall_sum + $unfiltered_queries_recall" | bc)
    recall_sum=$(echo "$recall_sum + $recall" | bc)
    query_time_sum=$(echo "$query_time_sum + $query_time" | bc)
    file_size_sum=$(echo "$file_size_sum + $file_size" | bc)

    echo "Finished execution #$i..."
done

# Write average metrics to file
average_build_time=$(echo "scale=5; $build_time_sum / $1" | bc)
average_filtered_queries_time=$(echo "scale=5; $filtered_queries_time_sum / $1" | bc)
average_filtered_queries_recall=$(echo "scale=5; $filtered_queries_recall_sum / $1" | bc)
average_unfiltered_queries_time=$(echo "scale=5; $unfiltered_queries_time_sum / $1" | bc)
average_unfiltered_queries_recall=$(echo "scale=5; $unfiltered_queries_recall_sum / $1" | bc)
average_recall=$(echo "scale=5; $recall_sum / $1" | bc)
average_query_time=$(echo "scale=5; $query_time_sum / $1" | bc)
average_file_size=$(echo "scale=5; $file_size_sum / $1" | bc)
echo -e "Average Build Time: $average_build_time seconds" >> $filename
echo -e "Average Filtered Queries Time: $average_filtered_queries_time seconds" >> $filename
echo -e "Average Filtered Queries Recall: $average_filtered_queries_recall %" >> $filename
echo -e "Average Unfiltered Queries Time: $average_unfiltered_queries_time seconds" >> $filename
echo -e "Average Unfiltered Queries Recall: $average_unfiltered_queries_recall %" >> $filename
echo -e "Average Recall: $average_recall %" >> $filename
echo -e "Average Query Time: $average_query_time seconds" >> $filename
echo -e "Average File size: $average_file_size bytes" >> $filename

rm -rf vamana.bin
