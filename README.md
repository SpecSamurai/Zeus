# Zeus - Game Engine

## Todo
- Cleanup
    - DONE:
        - VkContext
        - Device
        - Semaphore
        - Fence
        - CommandPool
        - Queue
        - DescriptorSetLayout
        - DescriptorPool
        - Sampler
- Grid
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
- UI Render with orthographic projection
- Middlewares
- Shaders
    - Async compilation, ThreadPool
    - Runtime shaders compilation | libshaderc
    - Atomic
- ECS
    - Versions
    - Systems
    - Archetype
    - Query - iterator
    - Entity LifeCycle
    - Grouping and filtering
    - Observe changes
    - A tombstone is a marker or reserved value used to represent an entity that
    - has been destroyed or is no longer valid in an entity-component system (ECS).

## Math
- Column-Major matrices to maintain consistency with both GLSL and SPIR-V.
- All spaces from local to NDC use right-handed coordinate system.
- Vulkan's NDC requires flipping Y axis downwards.

Local -> World -> View spaces use right-handed system with Y pointing upwards.

## Features
- GLTF
- Reverse depth
    - `Clear Value: 0, depthCompareOp: VK_COMPARE_OP_GREATER_OR_EQUAL, Perspective: near > far`
- Circular command stack
- Event Queue/Immediate dispatcher
- Console Logging
- Custom memory allocators
- 3D Math
    - Assumes only uniform transformations.
    - Normal matrix required to support non-uniform transformations(transpose_inverse)

## Git Submodules
While cloning the repo you need to pull all the submodules:
`git clone --recurse-submodules`

If you are missing the submodules, use the following commands:
```
git submodule init
git submodule update
```

Add a new submodule:
1. Go to submodule directory `cd libraries`
2. Add submodule: `git submodule add <url>`
3. Checkout desired branch/tag `git checkout master`
4. Update `git pull`

Updating submodules:
1. Go to submodule directory `cd libraries`
2. Checkout desired branch `git checkout master`
3. Update `git pull`
4. Go back to project root
5. Commit

...or `git submodule foreach git pull origin master`

## Commands
### HLSL DXC
```
COMMAND dxc.exe -Zpr -spirv -O0 -T vs_6_8 -E main shader.vert.hlsl -Fo shader.vert.spv
COMMAND dxc.exe -Zpr -spirv -O0 -T ps_6_8 -E main shader.frag.hlsl -Fo shader.frag.spv
```

### CMake
```
cmake --preset debug|release|test
cmake --build --preset debug|release --clean-first
cmake --build --verbose --preset debug
ctest --preset test
ctest --preset test -R TestName
```

### LLDB
```
target symbols add my.pdb

breakpoint list
breakpoint delete 1
breakpoint set --method method
```

## Environment variables
- `VULKAN_SDK` specifies the location of the Vulkan SDK root directory
- `LLDB_USE_NATIVE_PDB_READER` set to `yes` or `1` workaround for console LLDB on windows not loading symbols.
