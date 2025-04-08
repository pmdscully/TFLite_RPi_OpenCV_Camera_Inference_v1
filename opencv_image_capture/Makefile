# Makefile for Compiling the OpenCV Camera Processor

# Compiler
CXX = g++

# C++ Standard and Warning Flags
# -std=c++11 : Use the C++11 standard (or newer like c++14, c++17 if needed)
# -Wall      : Enable all standard warnings
# -O2        : Optimization level 2 (good balance)
CPPFLAGS = -std=c++11 -Wall -O2

# Use pkg-config to get OpenCV specific compile flags (include paths etc.)
# Replace 'opencv4' with 'opencv' if using OpenCV 3 or if 'opencv4' doesn't work
OPENCV_CFLAGS = $(shell pkg-config --cflags opencv4)

# Use pkg-config to get OpenCV specific linker flags (library paths and names)
# Replace 'opencv4' with 'opencv' if using OpenCV 3 or if 'opencv4' doesn't work
OPENCV_LIBS = $(shell pkg-config --libs opencv4)

# Combine all compiler flags
CXXFLAGS = $(CPPFLAGS) $(OPENCV_CFLAGS)

# Combine all library/linker flags
LIBS = $(OPENCV_LIBS)

# --- Project Specific ---

# Name of the final executable program
EXEC = capture_image_main

# List of C++ source files
SRCS = capture_image_main.cpp capture_image.cpp

# Automatically generate object file names from source files (e.g., camera_processor.o)
OBJS = $(SRCS:.cpp=.o)


# --- Targets ---

# Default target: Build the executable (called when you just type 'make')
all: $(EXEC)

# Rule to link the object files into the final executable
$(EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $(EXEC) $(LIBS)
	@echo "Executable '$(EXEC)' created successfully."

# Rule to compile a .cpp source file into a .o object file
# $< is the prerequisite (the .cpp file)
# $@ is the target (the .o file)
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Target to clean up generated files
clean:
	rm -f $(OBJS) $(EXEC)
	@echo "Cleaned up object files and executable."

# Declare 'all' and 'clean' as phony targets (targets that don't represent actual files)
.PHONY: all clean