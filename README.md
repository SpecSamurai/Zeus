# Zeus - Game Engine

```
COMMAND dxc.exe -Zpr -spirv -O0 -T vs_6_8 -E main shader.vert.hlsl -Fo shader.vert.spv
COMMAND dxc.exe -Zpr -spirv -O0 -T ps_6_8 -E main shader.frag.hlsl -Fo shader.frag.spv
```

## Math
Math implementation follows column-major memory layout to keep it consistent with both GLSL and SPIR-V.
TODO: Add flag to change to row-major memory layout.

## Git Submodules
While cloning the repo you need to pull all the submodules:
`git clone --recurse-submodules`

If you are missing the submodules, use the following commands:
```
git submodule init
git submodule update
```

## Tests
```
cmake --build --preset debug
ctest --preset test
```

## Environment variables
- `VULKAN_SDK` specifies the location of the Vulkan SDK root directory
