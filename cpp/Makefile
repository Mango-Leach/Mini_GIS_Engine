# Mini GIS Engine — Makefile
# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -g

# Source files
SRCS = main.cpp Distance.cpp QuadTree.cpp MinHeap.cpp AVLTree.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Output executable
TARGET = gis_engine

# Default target
all: $(TARGET)

# Link object files into executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Compile .cpp to .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run the executable
run: $(TARGET)
	./$(TARGET)

# Clean build artifacts
clean:
	del /Q *.o $(TARGET).exe 2>nul || true

# Dependencies
main.o: main.cpp Point.h Distance.h QuadTree.h MinHeap.h AVLTree.h
Distance.o: Distance.cpp Distance.h Point.h
QuadTree.o: QuadTree.cpp QuadTree.h Point.h Distance.h
MinHeap.o: MinHeap.cpp MinHeap.h Point.h
AVLTree.o: AVLTree.cpp AVLTree.h Point.h

.PHONY: all clean run
