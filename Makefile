# Root directory
ROOT_DIR := $(dir $(lastword $(MAKEFILE_LIST)))

# Compiler
CC := cc
CXX := g++ # g++ is used to compile Imgui, which is a c++ library

# Include directories
INCLUDES := -I$(ROOT_DIR) \
			-I$(ROOT_DIR)lib/hmm \
			-I$(ROOT_DIR)lib/sokol \
			-I$(ROOT_DIR)lib/sokol/util \
			-I$(ROOT_DIR)lib/cimgui/imgui \
			-I$(ROOT_DIR)lib/cimgui -I$(ROOT_DIR)lib/fast_obj \
			-I$(ROOT_DIR)

# Compiler flags
CFLAGS := -g $(INCLUDES)
CXXFLAGS := $(CFLAGS) -std=c++11 # Use C++11 standard
OBJCFLAGS := $(CFLAGS) -x objective-c
OBJCXXFLAGS := $(CXXFLAGS) -x objective-c++ # Use CXXFLAGS for Objective-C++

# Frameworks
FRAMEWORKS := -framework Metal -framework MetalKit -framework Cocoa -framework IOKit -framework CoreVideo -framework QuartzCore -framework CoreAudio

# Default source files if not provided
DEFAULT_OBJC_SOURCES := $(ROOT_DIR)main.c
IMGUI_SOURCES := $(ROOT_DIR)lib/cimgui/cimgui.cpp \
	 			 $(ROOT_DIR)lib/cimgui/imgui/imgui.cpp \
				 $(ROOT_DIR)lib/cimgui/imgui/imgui_widgets.cpp \
				 $(ROOT_DIR)lib/cimgui/imgui/imgui_draw.cpp \
				 $(ROOT_DIR)lib/cimgui/imgui/imgui_tables.cpp \
				 $(ROOT_DIR)lib/cimgui/imgui/imgui_demo.cpp

# Allow overriding source files
OBJC_SOURCES ?= $(DEFAULT_OBJC_SOURCES)

# Object files
OBJ := $(OBJC_SOURCES:.c=.o) $(IMGUI_SOURCES:.cpp=.o)

# Output
EXECUTABLE := main

all: $(EXECUTABLE)

# Linking
$(EXECUTABLE): $(OBJ)
	$(CXX) -o $@ $^ $(FRAMEWORKS)

%.o: %.c
	$(CC) $(OBJCFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(OBJCXXFLAGS) -c $< -o $@

clean:
	rm -f $(EXECUTABLE) $(OBJ)

.PHONY: all clean