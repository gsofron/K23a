# Main makefile in the repo's root, with which anything can be compiled

MAKE += --silent

BUILD_DIR := ./build

all: main groundtruth tests

# Compilation of the main executable
main:
	@mkdir -p $(BUILD_DIR)
	@$(MAKE) -C src

# Compilation of the groundtruth executable
groundtruth:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)/groundtruth
	@$(MAKE) -C src ../groundtruth

# Tests compilation
tests:
	@mkdir -p $(BUILD_DIR)
	@$(MAKE) -C tests

# Clean-up rule
clean:
	@$(MAKE) -C src clean
	@$(MAKE) -C tests clean
	@rm -r $(BUILD_DIR)

.PHONY: all main groundtruth tests clean