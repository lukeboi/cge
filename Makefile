# Compiler
CC := cc
CXX := g++ # g++ is used to compile Imgui, which is a c++ library

# Include directories
INCLUDES := -Ilib/hmm -Ilib/sokol -Ilib/sokol/util -Ilib/cimgui/imgui -Ilib/cimgui -Ilib/fast_obj

# Compiler flags
CFLAGS := -g $(INCLUDES)
CXXFLAGS := $(CFLAGS) -std=c++11 # Use C++11 standard
OBJCFLAGS := $(CFLAGS) -x objective-c
OBJCXXFLAGS := $(CXXFLAGS) -x objective-c++ # Use CXXFLAGS for Objective-C++

# Frameworks
FRAMEWORKS := -framework Metal -framework MetalKit -framework Cocoa -framework IOKit -framework CoreVideo -framework QuartzCore -framework CoreAudio

# Source files
OBJC_SOURCES := main.c
IMGUI_SOURCES := lib/cimgui/cimgui.cpp \
	 			 lib/cimgui/imgui/imgui.cpp \
				 lib/cimgui/imgui/imgui_widgets.cpp \
				 lib/cimgui/imgui/imgui_draw.cpp \
				 lib/cimgui/imgui/imgui_tables.cpp \
				 lib/cimgui/imgui/imgui_demo.cpp \
	
SOURCES := $(OBJC_SOURCES) $(IMGUI_SOURCES)

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

# clean:
# 	rm -f $(EXECUTABLE) $(OBJ)

.PHONY: all clean
