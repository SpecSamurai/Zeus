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
    defines.hpp
    entry_point.cpp
    entry_point.hpp

    application/Application.cpp
    application/Application.hpp
    application/CommandLineArgs.cpp
    application/CommandLineArgs.hpp

    camera/camera_utils.cpp
    camera/camera_utils.hpp
    camera/ArcballCamera.cpp
    camera/ArcballCamera.hpp
    camera/EditorCamera.hpp
    camera/FreeflyCamera.cpp
    camera/FreeflyCamera.hpp

    commands/CommandStack.hpp
    commands/ICommand.hpp

    events/Event.hpp
    events/EventDispatcher.hpp
    events/EventHandler.hpp
    events/EventQueue.hpp
    events/KeyEvent.hpp
    events/MouseEvent.hpp
    events/WindowEvent.hpp

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

    rendering/Renderer.cpp
    rendering/Renderer.hpp

    time/Timer.cpp
    time/Timer.hpp

    vulkan/Buffer.cpp
    vulkan/Buffer.hpp
    vulkan/CommandBuffer.cpp
    vulkan/CommandBuffer.hpp
    vulkan/Definitions.hpp
    vulkan/DeletionQueue.cpp
    vulkan/DeletionQueue.hpp

    vulkan/DescriptorAllocator.cpp
    vulkan/DescriptorAllocator.hpp
    vulkan/DescriptorLayoutBuilder.cpp
    vulkan/DescriptorLayoutBuilder.hpp
    vulkan/DescriptorWriter.cpp
    vulkan/DescriptorWriter.hpp

    vulkan/Device.cpp
    vulkan/Device.hpp
    vulkan/DeviceBuilder.cpp
    vulkan/DeviceBuilder.hpp
    vulkan/DynamicDescriptorAllocator.cpp
    vulkan/DynamicDescriptorAllocator.hpp
    vulkan/Fence.cpp
    vulkan/Fence.hpp
    vulkan/InstanceBuilder.cpp
    vulkan/InstanceBuilder.hpp
    vulkan/PhysicalDeviceSelector.cpp
    vulkan/PhysicalDeviceSelector.hpp
    vulkan/Pipeline.cpp
    vulkan/Pipeline.hpp
    vulkan/PipelineBuilder.cpp
    vulkan/PipelineBuilder.hpp
    vulkan/Queue.cpp
    vulkan/Queue.hpp
    vulkan/Sampler.cpp
    vulkan/Sampler.hpp
    vulkan/Semaphore.cpp
    vulkan/Semaphore.hpp
    vulkan/Shader.cpp
    vulkan/Shader.hpp
    vulkan/Swapchain.cpp
    vulkan/Swapchain.hpp
    vulkan/SwapchainBuilder.cpp
    vulkan/SwapchainBuilder.hpp
    vulkan/VkContext.cpp
    vulkan/VkContext.hpp

    vulkan/api/vulkan_buffer.cpp
    vulkan/api/vulkan_buffer.hpp
    vulkan/api/vulkan_command.cpp
    vulkan/api/vulkan_command.hpp
    vulkan/api/vulkan_debug.cpp
    vulkan/api/vulkan_debug.hpp
    vulkan/api/vulkan_descriptors.cpp
    vulkan/api/vulkan_descriptors.hpp
    vulkan/api/vulkan_device.cpp
    vulkan/api/vulkan_device.hpp
    vulkan/api/vulkan_dynamic_rendering.cpp
    vulkan/api/vulkan_dynamic_rendering.hpp
    vulkan/api/vulkan_framebuffer.cpp
    vulkan/api/vulkan_framebuffer.hpp
    vulkan/api/vulkan_image.hpp
    vulkan/api/vulkan_image.cpp
    vulkan/api/vulkan_memory.cpp
    vulkan/api/vulkan_memory.hpp
    vulkan/api/vulkan_pipeline.cpp
    vulkan/api/vulkan_pipeline.hpp
    vulkan/api/vulkan_renderpass.cpp
    vulkan/api/vulkan_renderpass.hpp
    vulkan/api/vulkan_shader.cpp
    vulkan/api/vulkan_shader.hpp
    vulkan/api/vulkan_surface.cpp
    vulkan/api/vulkan_surface.hpp
    vulkan/api/vulkan_sync.cpp
    vulkan/api/vulkan_sync.hpp

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
