BUILD_DIR := ../build
INC_DIR := ../include
SRC_DIR := .

CXX = g++
CXXFLAGS = -g -Wall -Wextra $(addprefix -I,$(INC_DIR)) -std=c++17

EXEC := ../k23a

SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
OBJS := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SRCS))

$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(EXEC)

.PHONY: clean