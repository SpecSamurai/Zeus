# Zeus - Game Engine

## Todo
- Reverse depth
- Runtime shaders compilation | libshaderc
- Transfer queue
- SIMD
- Memory Allocator
    - Multithreading
    - Strategies/Algorithms - Linear/TLSF
    - Statistics
    - Debug Mode
    - Dedicated Allocations
    - Defragmentation
    - Memory Corruption Detection
    - Priority

## Math
- Column-Major matrices to maintain consistency with both GLSL and SPIR-V.
- All spaces from local to NDC use right-handed coordinate system.
- Vulkan's NDC requires flipping Y axis downwards.

Local -> World -> View spaces use right-handed system with Y pointing upwards.

## Git Submodules
While cloning the repo you need to pull all the submodules:
`git clone --recurse-submodules`

If you are missing the submodules, use the following commands:
```
git submodule init
git submodule update
```

## Commands
### HLSL DXC
```
COMMAND dxc.exe -Zpr -spirv -O0 -T vs_6_8 -E main shader.vert.hlsl -Fo shader.vert.spv
COMMAND dxc.exe -Zpr -spirv -O0 -T ps_6_8 -E main shader.frag.hlsl -Fo shader.frag.spv
```

### CMake
```
cmake --preset debug|release|test
cmake --build --preset debug|release
ctest --preset test
```

## Environment variables
- `VULKAN_SDK` specifies the location of the Vulkan SDK root directory

## Libraries
- [Dear ImGui](https://github.com/ocornut/imgui)
- [Vulkan Memory Allocator](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator)
- [GLFW](https://github.com/glfw/glfw)
- [GoogleTest](https://github.com/google/googletest)
