find_package(Vulkan REQUIRED)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)

FetchContent_Declare(
    fastgltf
    GIT_REPOSITORY https://github.com/spnda/fastgltf.git
    GIT_TAG v0.8.0
)

FetchContent_MakeAvailable(glfw)
FetchContent_MakeAvailable(fastgltf)

add_library(Engine
    defines.hpp
    Camera.hpp

    core/logger.hpp

    math/definitions.hpp
    math/geometric.hpp
    math/transformations.hpp
    math/trigonometric.cpp
    math/trigonometric.hpp

    math/Matrix3x3.hpp
    math/Matrix4x4.hpp
    math/Quaternion.hpp
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

    vulkan/DeletionQueue.cpp
    vulkan/DeletionQueue.hpp
    vulkan/DescriptorAllocator.cpp
    vulkan/DescriptorAllocator.hpp
    vulkan/DescriptorLayoutBuilder.cpp
    vulkan/DescriptorLayoutBuilder.hpp
    vulkan/DescriptorWriter.cpp
    vulkan/DescriptorWriter.hpp
    vulkan/Device.hpp
    vulkan/DeviceBuilder.cpp
    vulkan/DeviceBuilder.hpp
    vulkan/DynamicDescriptorAllocator.cpp
    vulkan/DynamicDescriptorAllocator.hpp
    vulkan/ImageBuilder.cpp
    vulkan/ImageBuilder.hpp
    vulkan/Instance.cpp
    vulkan/Instance.hpp
    vulkan/InstanceBuilder.cpp
    vulkan/InstanceBuilder.hpp
    vulkan/PhysicalDevice.hpp
    vulkan/PhysicalDeviceSelector.cpp
    vulkan/PhysicalDeviceSelector.hpp
    vulkan/PipelineBuilder.cpp
    vulkan/PipelineBuilder.hpp
    vulkan/SwapchainBuilder.cpp
    vulkan/SwapchainBuilder.hpp

    vulkan/vulkan_buffer.cpp
    vulkan/vulkan_buffer.hpp
    vulkan/vulkan_command.cpp
    vulkan/vulkan_command.hpp
    vulkan/vulkan_debug.cpp
    vulkan/vulkan_debug.hpp
    vulkan/vulkan_descriptors.cpp
    vulkan/vulkan_descriptors.hpp
    vulkan/vulkan_device.cpp
    vulkan/vulkan_device.hpp
    vulkan/vulkan_dynamic_rendering.cpp
    vulkan/vulkan_dynamic_rendering.hpp
    vulkan/vulkan_framebuffer.cpp
    vulkan/vulkan_framebuffer.hpp
    vulkan/vulkan_image.hpp
    vulkan/vulkan_image.cpp
    vulkan/vulkan_memory.cpp
    vulkan/vulkan_memory.hpp
    vulkan/vulkan_pipeline.cpp
    vulkan/vulkan_pipeline.hpp
    vulkan/vulkan_renderpass.cpp
    vulkan/vulkan_renderpass.hpp
    vulkan/vulkan_settings.hpp
    vulkan/vulkan_shader.cpp
    vulkan/vulkan_shader.hpp
    vulkan/vulkan_surface.cpp
    vulkan/vulkan_surface.hpp
    vulkan/vulkan_sync.cpp
    vulkan/vulkan_sync.hpp
    vulkan/vulkan_types.hpp

    window/glfw_utils.cpp
    window/glfw_utils.hpp
)

target_include_directories(Engine
    PUBLIC .
)

target_link_libraries(Engine
    PUBLIC Vulkan::Vulkan glfw VulkanMemoryAllocator stb fastgltf
)

target_compile_options(Engine PRIVATE
    $<$<CONFIG:Debug>:${CXX_DEBUG_COMPILE_FLAGS}>
    $<$<CONFIG:Release>:${CXX_RELEASE_COMPILE_FLAGS}>)
