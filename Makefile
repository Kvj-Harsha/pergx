# Compiler and flags
CXX := g++
CXXFLAGS := -O2 -std=c++17 -fopenmp -Wall -Wextra

# Source and target
SRC := main.cpp
TARGET := perg

# Default target: build the binary
all: $(TARGET)

# Build rule
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Run with args: e.g., make run ARGS="-e term -f path"
run: $(TARGET)
	./$(TARGET) $(ARGS)

# Clean up build artifacts
clean:
	rm -f $(TARGET)

.PHONY: all clean run
