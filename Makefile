# Zap Benchmarking Library - Makefile
#
# Usage:
#   make                    # Build all examples
#   make test               # Build and run tests
#   make run                # Build and run all examples
#   make run E=quick        # Build and run specific example
#   make run E=micro ARGS="--env --histogram"
#   make clean              # Remove built binaries
#
# Examples: quick, verbose, ci, micro, example, example_advanced

CC ?= gcc
CFLAGS ?= -Wall -Wextra -O2
LDFLAGS ?= -lm

BUILD_DIR := build
EXAMPLES_DIR := examples
TESTS_DIR := tests

# All example sources
EXAMPLES := $(wildcard $(EXAMPLES_DIR)/*.c)
BINARIES := $(patsubst $(EXAMPLES_DIR)/%.c,$(BUILD_DIR)/%,$(EXAMPLES))

# Test sources
TEST_SRCS := $(wildcard $(TESTS_DIR)/*.c)
TEST_BIN := $(BUILD_DIR)/test_zap

# Default target
all: $(BINARIES)

# Build directory
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Compile examples
$(BUILD_DIR)/%: $(EXAMPLES_DIR)/%.c zap.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I. -o $@ $< $(LDFLAGS)

# Build tests
$(TEST_BIN): $(TEST_SRCS) zap.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I. -o $@ $(TEST_SRCS) $(LDFLAGS)

# Run tests
.PHONY: test
test: $(TEST_BIN)
	@./$(TEST_BIN)

# Run examples
.PHONY: run
run: all
ifdef E
	@if [ -f "$(BUILD_DIR)/$(E)" ]; then \
		echo "Running $(E)..."; \
		./$(BUILD_DIR)/$(E) $(ARGS); \
	elif [ -f "$(BUILD_DIR)/example_$(E)" ]; then \
		echo "Running example_$(E)..."; \
		./$(BUILD_DIR)/example_$(E) $(ARGS); \
	else \
		echo "Example '$(E)' not found. Available:"; \
		ls -1 $(BUILD_DIR) | sed 's/^/  /'; \
		exit 1; \
	fi
else
	@echo "Running all examples..."
	@for bin in $(BINARIES); do \
		echo "\n=== $$(basename $$bin) ==="; \
		$$bin --no-save $(ARGS) || true; \
	done
endif

# List available examples
.PHONY: list
list:
	@echo "Available examples:"
	@for src in $(EXAMPLES); do \
		name=$$(basename $$src .c); \
		echo "  $$name"; \
	done
	@echo ""
	@echo "Usage: make run E=<name> [ARGS=\"...\"]"

# Clean build artifacts
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

# Help
.PHONY: help
help:
	@echo "Zap Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all          Build all examples (default)"
	@echo "  test         Build and run tests"
	@echo "  run          Build and run examples"
	@echo "  list         List available examples"
	@echo "  clean        Remove build artifacts"
	@echo "  help         Show this help"
	@echo ""
	@echo "Variables:"
	@echo "  E=<name>     Select example to run (e.g., E=quick)"
	@echo "  ARGS=\"...\"   Pass arguments to benchmark (e.g., ARGS=\"--env\")"
	@echo ""
	@echo "Examples:"
	@echo "  make                          # Build all"
	@echo "  make run                      # Run all examples"
	@echo "  make run E=quick              # Run example_quick"
	@echo "  make run E=micro ARGS=\"--env\" # Run with flags"
	@echo "  make clean && make run E=ci   # Rebuild and run"
