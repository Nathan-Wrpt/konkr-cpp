# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -Werror

# Linker flags
LDFLAGS = -lSDL2 -lSDL2_image -lSDL2_gfx -lSDL2_ttf

# Source files
SRC = $(wildcard *.cpp core/*.cpp entities/*.cpp game/*.cpp players/*.cpp ui/*.cpp constants/*.cpp)

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