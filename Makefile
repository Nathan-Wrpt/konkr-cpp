# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++17 -Wall

# Linker flags
LDFLAGS = -lSDL2 -lSDL2_image -lSDL2_gfx

# Source files
SRC = $(wildcard *.cpp core/*.cpp entities/*.cpp game/*.cpp)

# Object files
OBJ = $(SRC:.cpp=.o)

# Executable
TARGET = konkr

# Default target
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)

# Compile source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJ) $(TARGET)

# Phony targets
.PHONY: all clean