./scripts/metrics-filtered.sh 10 ./metrics/filtered_metrics.txt 0
./scripts/metrics-filtered.sh 10 ./metrics/filtered_random_graph_metrics.txt 1
./scripts/metrics-stitched.sh 10 ./metrics/stitched_metrics.txt 0 0
./scripts/metrics-stitched.sh 10 ./metrics/stitched_random_medoid_metrics.txt 0 1
./scripts/metrics-stitched.sh 10 ./metrics/stitched_random_subset_medoid_metrics.txt 0 2
./scripts/metrics-stitched.sh 10 ./metrics/stitched_random_graph_metrics.txt 1 0