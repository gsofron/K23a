BUILD_DIR := ../build
INC_DIR := ../include
SRC_DIR := .

CXX = g++
CXXFLAGS = -g -Wall -Wextra -std=c++17 -O2 $(addprefix -I,$(INC_DIR))

EXEC := ../k23a
SRCS := $(shell find $(SRC_DIR) -maxdepth 1 -name '*.cpp')
OBJS := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SRCS))

EXEC_GROUNDTRUTH := ../groundtruth
OBJS_GROUNDTRUTH := $(BUILD_DIR)/groundtruth/groundtruth_brute_force.o $(BUILD_DIR)/vectors.o 

$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(EXEC_GROUNDTRUTH): $(OBJS_GROUNDTRUTH)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(EXEC) $(EXEC_GROUNDTRUTH)

.PHONY: clean