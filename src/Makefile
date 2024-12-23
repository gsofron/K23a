BUILD_DIR := ../build
INC_DIR := ../include
SRC_DIR := .

CXX = g++
CXXFLAGS = -g -Wall -Wextra -std=c++17 -fopenmp -O2 $(addprefix -I,$(INC_DIR))

EXEC_FILTERED := ../filtered
OBJS_FILTERED := $(BUILD_DIR)/directed_graph.o $(BUILD_DIR)/filtered_greedy_search.o  $(BUILD_DIR)/filtered_robust_prune.o \
                 $(BUILD_DIR)/filtered_vamana.o $(BUILD_DIR)/findmedoid.o $(BUILD_DIR)/vectors.o \
				 $(BUILD_DIR)/vamana.o $(BUILD_DIR)/greedy_search.o $(BUILD_DIR)/robust_prune.o

EXEC_STITCHED := ../stitched 
OBJS_STITCHED := $(BUILD_DIR)/directed_graph.o $(BUILD_DIR)/filtered_greedy_search.o  $(BUILD_DIR)/filtered_robust_prune.o \
                 $(BUILD_DIR)/findmedoid.o $(BUILD_DIR)/vectors.o $(BUILD_DIR)/greedy_search.o \
                 $(BUILD_DIR)/robust_prune.o $(BUILD_DIR)/vamana.o $(BUILD_DIR)/stitched_vamana.o



EXEC_GROUNDTRUTH := ../groundtruth
OBJS_GROUNDTRUTH := $(BUILD_DIR)/groundtruth_brute_force.o $(BUILD_DIR)/vectors.o 

$(EXEC_FILTERED): $(OBJS_FILTERED)
	$(CXX) $(CXXFLAGS) -DFILTERED_VAMANA=1 -c $(SRC_DIR)/main.cpp -o $(BUILD_DIR)/main.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(BUILD_DIR)/main.o

$(EXEC_STITCHED): $(OBJS_STITCHED)
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/main.cpp -o $(BUILD_DIR)/main.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(BUILD_DIR)/main.o

$(EXEC_GROUNDTRUTH): $(OBJS_GROUNDTRUTH)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(EXEC_FILTERED) $(EXEC_STITCHED) $(EXEC_GROUNDTRUTH)

.PHONY: clean