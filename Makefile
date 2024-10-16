# Main makefile in the repo's root, with which anything can be compiled

MAKE += --silent

BUILD_DIR := ./build

all: main tests

# Compilation of the main executable
main:
	@mkdir -p $(BUILD_DIR)
	@$(MAKE) -C src

# Tests compilation
tests:
	@mkdir -p $(BUILD_DIR)
	@$(MAKE) -C tests

# Clean-up rule
clean:
	@$(MAKE) -C src clean
	@$(MAKE) -C tests clean
	@rm -r $(BUILD_DIR)

.PHONY: all main tests clean