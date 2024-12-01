find_package(Vulkan REQUIRED)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt
    GIT_TAG 11.0.2)
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

add_library(Engine
    application/Application.cpp
    application/Application.hpp
    application/CommandLineArgs.cpp
    application/CommandLineArgs.hpp
    application/entry_point.cpp
    application/entry_point.hpp

    camera/camera_utils.cpp
    camera/camera_utils.hpp
    camera/ArcballCamera.cpp
    camera/ArcballCamera.hpp
    camera/EditorCamera.hpp
    camera/FreeflyCamera.cpp
    camera/FreeflyCamera.hpp

    commands/CommandStack.hpp
    commands/ICommand.hpp

    core/defines.hpp
    core/Engine.cpp
    core/Engine.hpp

    ecs/Definitions.hpp
    ecs/FamilyId.hpp

    events/Event.hpp
    events/EventDispatcher.hpp
    events/EventHandler.hpp
    events/EventQueue.hpp
    events/KeyEvent.hpp
    events/MouseEvent.hpp
    events/WindowEvent.hpp

    gltf/GltfLoader.cpp
    gltf/GltfLoader.hpp
    gltf/utility.cpp
    gltf/utility.hpp

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

    profiling/Timer.cpp
    profiling/Timer.hpp

    rendering/Renderer.cpp
    rendering/Renderer.hpp

    rendering/renderer_types.cpp
    rendering/renderer_types.hpp

    rendering/resources.cpp
    rendering/resources.hpp

    vulkan/BlendState.cpp
    vulkan/BlendState.hpp
    vulkan/Buffer.cpp
    vulkan/Buffer.hpp
    vulkan/CommandBuffer.cpp
    vulkan/CommandBuffer.hpp
    vulkan/CommandPool.cpp
    vulkan/CommandPool.hpp
    vulkan/Definitions.hpp
    vulkan/DeletionQueue.cpp
    vulkan/DeletionQueue.hpp
    vulkan/DepthStencilState.cpp
    vulkan/DepthStencilState.hpp
    vulkan/Descriptor.cpp
    vulkan/Descriptor.hpp
    vulkan/DescriptorPool.cpp
    vulkan/DescriptorPool.hpp
    vulkan/DescriptorSet.cpp
    vulkan/DescriptorSet.hpp
    vulkan/DescriptorSetLayout.cpp
    vulkan/DescriptorSetLayout.hpp
    vulkan/Device.cpp
    vulkan/Device.hpp
    vulkan/Fence.cpp
    vulkan/Fence.hpp
    vulkan/Image.cpp
    vulkan/Image.hpp
    vulkan/InstanceBuilder.cpp
    vulkan/InstanceBuilder.hpp
    vulkan/PhysicalDeviceSelector.cpp
    vulkan/PhysicalDeviceSelector.hpp
    vulkan/Pipeline.cpp
    vulkan/Pipeline.hpp
    vulkan/PipelineState.cpp
    vulkan/PipelineState.hpp
    vulkan/PushConstants.cpp
    vulkan/PushConstants.hpp
    vulkan/Queue.cpp
    vulkan/Queue.hpp
    vulkan/RasterizationState.cpp
    vulkan/RasterizationState.hpp
    vulkan/Sampler.cpp
    vulkan/Sampler.hpp
    vulkan/Semaphore.cpp
    vulkan/Semaphore.hpp
    vulkan/Shader.cpp
    vulkan/Shader.hpp
    vulkan/Swapchain.cpp
    vulkan/Swapchain.hpp
    vulkan/Vertex.hpp
    vulkan/VertexInput.cpp
    vulkan/VertexInput.hpp
    vulkan/VkContext.cpp
    vulkan/VkContext.hpp

    vulkan/rhi/vulkan_command.cpp
    vulkan/rhi/vulkan_command.hpp
    vulkan/rhi/vulkan_debug.cpp
    vulkan/rhi/vulkan_debug.hpp
    vulkan/rhi/vulkan_device.cpp
    vulkan/rhi/vulkan_device.hpp
    vulkan/rhi/vulkan_dynamic_rendering.cpp
    vulkan/rhi/vulkan_dynamic_rendering.hpp
    vulkan/rhi/vulkan_framebuffer.cpp
    vulkan/rhi/vulkan_framebuffer.hpp
    vulkan/rhi/vulkan_image.hpp
    vulkan/rhi/vulkan_image.cpp
    vulkan/rhi/vulkan_memory.cpp
    vulkan/rhi/vulkan_memory.hpp
    vulkan/rhi/vulkan_renderpass.cpp
    vulkan/rhi/vulkan_renderpass.hpp
    vulkan/rhi/vulkan_surface.cpp
    vulkan/rhi/vulkan_surface.hpp

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

file(GLOB_RECURSE ASSETS_SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/assets/*"
)

foreach(ASSET_FILE ${ASSETS_SOURCE_FILES})
    get_filename_component(FILE_NAME ${ASSET_FILE} NAME)

    configure_file(
        ${ASSET_FILE} "${CMAKE_CURRENT_BINARY_DIR}/assets/${FILE_NAME}"
        COPYONLY)
endforeach(ASSET_FILE)
