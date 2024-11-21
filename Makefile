# Main makefile in the repo's root, with which anything can be compiled

MAKE += --silent

BUILD_DIR := ./build
SRC_DIR := ./src
INC_DIR := ./include

CXX = g++
CXXFLAGS = -g -Wall -Wextra -std=c++17 -O2 $(addprefix -I,$(INC_DIR))

all: main tests

# Compilation of the main executable
main:
	@mkdir -p $(BUILD_DIR)
	@$(MAKE) -C src

# Tests compilation
tests:
	@mkdir -p $(BUILD_DIR)
	@$(MAKE) -C tests

# Groundtruth brute force compilation
groundtruth:
	@mkdir -p $(BUILD_DIR)
	@$(CXX) $(CXXFLAGS) -c groundtruth_brute_force.cpp -o $(BUILD_DIR)/groundtruth_brute_force.o
	@$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/vectors.cpp -o $(BUILD_DIR)/vectors.o
	@$(CXX) $(CXXFLAGS) -o groundtruth $(BUILD_DIR)/groundtruth_brute_force.o $(BUILD_DIR)/vectors.o

# Clean-up rule
clean:
	@$(MAKE) -C src clean
	@$(MAKE) -C tests clean
	@rm -rf $(BUILD_DIR)
	@rm -f groundtruth

.PHONY: all main tests clean