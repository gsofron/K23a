# Main makefile in the repo's root, with which anything can be compiled

MAKE += --silent

BUILD_DIR := ./build

all: filtered stitched groundtruth tests

# Compilation of the filtered main executable
filtered:
	@mkdir -p $(BUILD_DIR)
	@$(MAKE) -C src ../filtered

# Compilation of the stitched main executable
stitched:
	@mkdir -p $(BUILD_DIR)
	@$(MAKE) -C src ../stitched

# Compilation of the groundtruth executable
groundtruth:
	@mkdir -p $(BUILD_DIR)
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

.PHONY: all filtered stitched groundtruth tests clean