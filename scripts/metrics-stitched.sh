#!/bin/bash
# ./scripts/metrics-stitched.sh 10 ./metrics/stitched_metrics.txt 0 0
# ./scripts/metrics-stitched.sh 10 ./metrics/stitched_random_medoid_metrics.txt 0 1
# ./scripts/metrics-stitched.sh 10 ./metrics/stitched_random_subset_medoid_metrics.txt 0 2
# ./scripts/metrics-stitched.sh 10 ./metrics/stitched_random_graph_metrics.txt 1 0
# ./scripts/metrics-stitched.sh 10 ./metrics/stitched_random_graph_random_medoid_metrics.txt 1 1
# ./scripts/metrics-stitched.sh 10 ./metrics/stitched_random_graph_random_subset_medoid_metrics.txt 1 2

# Check if all arguments are provided
if [ $# -ne 4 ]; then
    echo "Usage: $0 <number of loops> <filename> <random graph flag> <medoid flag>"
    exit 1
fi

# Validate if the first argument is a positive integer
if ! [[ "$1" =~ ^[0-9]+$ ]]; then
    echo "Error: The number of loops must be a positive integer."
    exit 1
fi

# Check if the third argument is strictly 0 or 1
if [ "$3" -ne 0 ] && [ "$3" -ne 1 ]; then
    echo "Error: Random graph flag must be 0 or 1."
    exit 1
fi

# Check if the fourth argument is strictly 0, 1 or 2
if [ "$3" -ne 0 ] && [ "$3" -ne 1 ] && [ "$3" -ne 2 ]; then
    echo "Error: Medoid flag must be 0, 1 or 2."
    exit 1
fi

# Initialize variables
count=$1
filename=$2
>$filename

command="./stitched -b dummy/dummy-data.bin -q dummy/dummy-queries.bin -g dummy/dummy-groundtruth.bin -s vamana.bin -n 10000 -m 5012 -a 1.1 -L 150 -l 100 -r 32 -R 64 -t 50 -i -1"
if [ "$3" -eq 1 ]; then
   command+=" --random-graph"
fi
if [ "$4" -eq 1 ]; then
   command+=" --random-medoid"
elif [ "$4" -eq 2 ]; then
    command+=" --random-subset-medoid"
fi

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
