BUILD_DIR := ../build
INC_DIRS := ../include ../include/acutest
SRC_DIR := ../src
TEST_DIR := .

CXX = g++
CXXFLAGS = -g -Wall -Wextra -std=c++17 $(addprefix -I,$(INC_DIRS))

all: ../directed_graph_test ../vectors_test ../greedy_search_test ../robust_prune_test ../vamana_test

../directed_graph_test: $(BUILD_DIR)/directed_graph_test.o $(BUILD_DIR)/directed_graph.o
	$(CXX) $(CXXFLAGS) -o $@ $^

../vectors_test: $(BUILD_DIR)/vectors_test.o $(BUILD_DIR)/vectors.o
	$(CXX) $(CXXFLAGS) -o $@ $^

../greedy_search_test: $(BUILD_DIR)/greedy_search_test.o $(BUILD_DIR)/directed_graph.o $(BUILD_DIR)/greedy_search.o $(BUILD_DIR)/vectors.o
	$(CXX) $(CXXFLAGS) -o $@ $^

../robust_prune_test: $(BUILD_DIR)/robust_prune_test.o $(BUILD_DIR)/robust_prune.o $(BUILD_DIR)/directed_graph.o $(BUILD_DIR)/vamana.o $(BUILD_DIR)/vectors.o $(BUILD_DIR)/greedy_search.o
	$(CXX) $(CXXFLAGS) -o $@ $^

../vamana_test: $(BUILD_DIR)/vamana_test.o $(BUILD_DIR)/directed_graph.o $(BUILD_DIR)/vamana.o $(BUILD_DIR)/vectors.o $(BUILD_DIR)/robust_prune.o $(BUILD_DIR)/greedy_search.o
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(TEST_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f ../*_test

.PHONY: all clean