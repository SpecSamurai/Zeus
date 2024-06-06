# Zeus - Game Engine

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
