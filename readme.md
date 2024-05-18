../sokol-tools-bin/bin/osx_arm64/sokol-shdc --input shader/phong.glsl --output shader/phong.glsl.h --slang glsl330:hlsl5:metal_macos

after breaking change:
../sokol-tools-bin/bin/osx_arm64/sokol-shdc --input shader/cube.glsl --output shader/cube.glsl.h --slang glsl430:hlsl5:metal_macos
../sokol-tools-bin/bin/osx_arm64/sokol-shdc --input shader/volume.glsl --output shader/volume.glsl.h --slang glsl430:hlsl5:metal_macos