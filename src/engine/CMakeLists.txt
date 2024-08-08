find_package(Vulkan REQUIRED)

add_library(Engine
    defines.hpp

    core/logger.hpp

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

    vulkan/vulkan_debug.cpp
    vulkan/vulkan_debug.hpp
    vulkan/vulkan_utils.cpp
    vulkan/vulkan_utils.hpp
    vulkan/vulkan_shader.cpp
    vulkan/vulkan_shader.hpp
    vulkan/vulkan_sync.cpp
    vulkan/vulkan_sync.hpp
)

target_include_directories(Engine
    PUBLIC .
)

target_link_libraries(Engine
    PRIVATE Vulkan::Vulkan
)

target_compile_options(Engine PRIVATE
    $<$<CONFIG:Debug>:${CXX_DEBUG_COMPILE_FLAGS}>
    $<$<CONFIG:Release>:${CXX_RELEASE_COMPILE_FLAGS}>)
