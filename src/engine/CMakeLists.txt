find_package(Vulkan REQUIRED)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt
    GIT_TAG 11.0.2
)
FetchContent_MakeAvailable(fmt)

set_target_properties(fmt PROPERTIES FOLDER "Libraries")

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
    fastgltf
    GIT_REPOSITORY https://github.com/spnda/fastgltf.git
    GIT_TAG v0.8.0
)
FetchContent_MakeAvailable(fastgltf)

set_target_properties(fastgltf PROPERTIES FOLDER "Libraries")

FetchContent_Declare(
    xxhash
    GIT_REPOSITORY https://github.com/Cyan4973/xxHash.git
    GIT_TAG v0.8.3
    SOURCE_SUBDIR cmake_unofficial
)
FetchContent_MakeAvailable(xxhash)

set_target_properties(
    xxhash
    xxhsum
    PROPERTIES FOLDER "Libraries")

add_library(Engine STATIC
    application/Application.cpp
    application/Application.hpp
    application/CommandLineArgs.cpp
    application/CommandLineArgs.hpp
    application/entry_point.cpp
    application/entry_point.hpp

    assets/AssetsManager.hpp
    assets/ObjLoader.cpp
    assets/ObjLoader.hpp
    assets/ObjModel.hpp
    assets/utility.cpp
    assets/utility.hpp

    camera/ArcballCamera.cpp
    camera/ArcballCamera.hpp
    camera/EditorCamera.hpp
    camera/FreeflyCamera.cpp
    camera/FreeflyCamera.hpp

    commands/CommandStack.hpp
    commands/ICommand.hpp

    components/Light.hpp
    components/Renderable.hpp
    components/Transform.hpp

    core/defines.hpp
    core/Engine.cpp
    core/Engine.hpp
    core/FileSystem.hpp
    core/Hasher.hpp
    core/World.cpp
    core/World.hpp

    ecs/ComponentSparseSet.hpp
    ecs/ComponentSparseSetIterator.hpp
    ecs/Entity.hpp
    ecs/FamilyId.hpp
    ecs/Query.hpp
    ecs/QueryIterator.hpp
    ecs/Registry.hpp
    ecs/SparseSet.cpp
    ecs/SparseSet.hpp
    ecs/SparseSetIterator.hpp

    events/Event.hpp
    events/EventDispatcher.hpp
    events/EventHandler.hpp
    events/EventQueue.hpp
    events/KeyEvent.hpp
    events/MouseEvent.hpp
    events/WindowEvent.hpp

    input/Input.cpp
    input/Input.hpp
    input/KeyCode.hpp
    input/MouseButtonCode.hpp

    logging/logger.hpp

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

    profiling/Profiler.cpp
    profiling/Profiler.hpp
    profiling/Stopwatch.cpp
    profiling/Stopwatch.hpp

    rendering/Material.cpp
    rendering/Material.hpp
    rendering/Mesh.cpp
    rendering/Mesh.hpp
    rendering/Renderer.cpp
    rendering/Renderer.hpp
    rendering/Renderer_resources.cpp
    rendering/Renderer_resources.hpp
    rendering/Renderer_types.hpp
    rendering/Vertex.hpp

    rhi/BlendState.cpp
    rhi/BlendState.hpp
    rhi/Buffer.cpp
    rhi/Buffer.hpp
    rhi/CommandBuffer.cpp
    rhi/CommandBuffer.hpp
    rhi/CommandPool.cpp
    rhi/CommandPool.hpp
    rhi/Definitions.hpp
    rhi/DeletionQueue.cpp
    rhi/DeletionQueue.hpp
    rhi/DepthStencilState.cpp
    rhi/DepthStencilState.hpp
    rhi/DescriptorPool.cpp
    rhi/DescriptorPool.hpp
    rhi/DescriptorSet.cpp
    rhi/DescriptorSet.hpp
    rhi/DescriptorSetLayout.cpp
    rhi/DescriptorSetLayout.hpp
    rhi/Device.cpp
    rhi/Device.hpp
    rhi/Fence.cpp
    rhi/Fence.hpp
    rhi/Image.cpp
    rhi/Image.hpp
    rhi/InstanceBuilder.cpp
    rhi/InstanceBuilder.hpp
    rhi/PhysicalDeviceSelector.cpp
    rhi/PhysicalDeviceSelector.hpp
    rhi/Pipeline.cpp
    rhi/Pipeline.hpp
    rhi/Queue.cpp
    rhi/Queue.hpp
    rhi/RasterizationState.cpp
    rhi/RasterizationState.hpp
    rhi/Sampler.cpp
    rhi/Sampler.hpp
    rhi/Semaphore.cpp
    rhi/Semaphore.hpp
    rhi/Shader.cpp
    rhi/Shader.hpp
    rhi/Swapchain.cpp
    rhi/Swapchain.hpp
    rhi/VertexInput.cpp
    rhi/VertexInput.hpp
    rhi/VkContext.cpp
    rhi/VkContext.hpp

    rhi/vulkan/vulkan_command.cpp
    rhi/vulkan/vulkan_command.hpp
    rhi/vulkan/vulkan_debug.cpp
    rhi/vulkan/vulkan_debug.hpp
    rhi/vulkan/vulkan_device.cpp
    rhi/vulkan/vulkan_device.hpp
    rhi/vulkan/vulkan_dynamic_rendering.cpp
    rhi/vulkan/vulkan_dynamic_rendering.hpp
    rhi/vulkan/vulkan_framebuffer.cpp
    rhi/vulkan/vulkan_framebuffer.hpp
    rhi/vulkan/vulkan_image.hpp
    rhi/vulkan/vulkan_image.cpp
    rhi/vulkan/vulkan_memory.cpp
    rhi/vulkan/vulkan_memory.hpp
    rhi/vulkan/vulkan_renderpass.cpp
    rhi/vulkan/vulkan_renderpass.hpp
    rhi/vulkan/vulkan_surface.cpp
    rhi/vulkan/vulkan_surface.hpp

    window/glfw_utils.cpp
    window/glfw_utils.hpp
    window/Window.cpp
    window/Window.hpp
)

target_include_directories(Engine
    PUBLIC .
)

target_link_libraries(Engine
    PUBLIC
        Vulkan::Vulkan
        VulkanMemoryAllocator
        glfw
        fmt::fmt
        stb
        fastgltf
        tinyobjloader
        xxhash
)

target_compile_options(Engine PRIVATE
    $<$<CONFIG:Debug>:${CXX_DEBUG_COMPILE_FLAGS}>
    $<$<CONFIG:Release>:${CXX_RELEASE_COMPILE_FLAGS}>)

find_program(GLSLANG_VALIDATOR glslangValidator HINTS $ENV{VULKAN_SDK}/Bin/)

file(GLOB_RECURSE SHADER_SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.glsl"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.vert"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.tesc"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.tese"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.geom"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.frag"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.comp"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.mesh"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.task"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.rgen"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.rint"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.rahit"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.rchit"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.rmiss"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.rcall"
)

foreach(SHADER_FILE ${SHADER_SOURCE_FILES})
    get_filename_component(FILE_EXT ${SHADER_FILE} EXT)
    if(${FILE_EXT} STREQUAL ".glsl")
        continue()
    endif()

    get_filename_component(FILE_NAME ${SHADER_FILE} NAME)

    set(SPIRV "${CMAKE_CURRENT_BINARY_DIR}/shaders/${FILE_NAME}.spv")

    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${GLSLANG_VALIDATOR} -g -V ${SHADER_FILE} -o ${SPIRV}
        DEPENDS ${SHADER_FILE}
        COMMENT "Compiling Shader: ${SHADER_FILE}")

    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(SHADER_FILE)

add_custom_target(Shaders ALL
    DEPENDS ${SPIRV_BINARY_FILES}
    SOURCES ${SHADER_SOURCE_FILES})
