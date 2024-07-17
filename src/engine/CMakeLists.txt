add_library(Engine
    defines.hpp

    core/logger.hpp
    core/dsa/LinkedList.hpp
    core/dsa/Stack.hpp
    core/dsa/Queue.hpp
    core/dsa/Heap.hpp
    core/dsa/DisjointSet.hpp
    core/dsa/Trie.hpp
    core/dsa/BinarySearchTree.hpp
    core/dsa/algorithms.hpp

    math/definitions.hpp
    math/transformations.hpp
    math/trigonometric.cpp
    math/trigonometric.hpp
    math/geometric.hpp
    math/Matrix3x3.hpp
    math/Matrix4x4.hpp
    math/Vector2.hpp
    math/Vector3.hpp
    math/Vector4.hpp

    memory/memory.cpp
    memory/memory.hpp
    memory/Allocator.cpp
    memory/Allocator.hpp
    memory/LinearAllocator.cpp
    memory/LinearAllocator.hpp
    memory/FreeBlock.hpp
    memory/AllocationHeader.hpp
    memory/FreeListAllocator.hpp
    memory/FreeListAllocator.cpp
)

target_include_directories(Engine
    INTERFACE .
)

target_compile_options(Engine PRIVATE
    $<$<CONFIG:Debug>:${CXX_DEBUG_COMPILE_FLAGS}>
    $<$<CONFIG:Release>:${CXX_RELEASE_COMPILE_FLAGS}>)
