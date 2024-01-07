all:
	cc -Ilib/hmm -Ilib/sokol -x objective-c -o main main.c -framework QuartzCore -framework Cocoa -framework Metal -framework MetalKit -framework AudioToolbox 
shader:
	../sokol-tools-bin/bin/osx_arm64/sokol-shdc --input shader/triangle.glsl --output shader/triangle.glsl.h --slang glsl330:hlsl5:metal_macos