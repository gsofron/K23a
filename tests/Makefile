BUILD_DIR := ../build
INC_DIRS := ../include ../include/acutest
SRC_DIR := ../src
TEST_DIR := .

CXX = g++
CXXFLAGS = -g -Wall -Wextra $(addprefix -I,$(INC_DIRS))

all: ../directed_graph_test ../math_vector_test

../directed_graph_test: $(BUILD_DIR)/directed_graph_test.o
	$(CXX) $(CXXFLAGS) -o $@ $^

../math_vector_test: $(BUILD_DIR)/math_vector_test.o
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(TEST_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f ../*_test

.PHONY: all clean