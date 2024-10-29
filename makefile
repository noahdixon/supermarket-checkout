# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall

# Folders
INCLUDE_DIR = include
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Source and object files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))

# Executable
EXEC = $(BIN_DIR)/supermarket_checkout

# Conditional for Windows
ifeq ($(OS),Windows_NT)
    MKDIR = mkdir
    RM = del /Q
else
    MKDIR = @mkdir -p
    RM = @rm -rf
endif

# Default rule
all: $(EXEC)

# Link object files
$(EXEC): $(OBJECTS)
	$(MKDIR) $(BIN_DIR)
	$(CXX) $(OBJECTS) -o $(EXEC)

# Compile cpp files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(MKDIR) $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Clean up build files
clean:
	$(RM) $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean