include(FetchContent)

find_package(Vulkan REQUIRED)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)

FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 1.0.1
)

FetchContent_MakeAvailable(glfw)
FetchContent_MakeAvailable(glm)

add_executable(Editor
    main.cpp

    core/files.hpp

    ui/IUndoableCommand.hpp
    ui/UndoRedoStack.hpp

    graphics/Camera.hpp
    graphics/Vertex.hpp
    graphics/glfw_utils.cpp
    graphics/glfw_utils.hpp

    graphics/vulkan/Device.hpp
    graphics/vulkan/DeviceBuilder.cpp
    graphics/vulkan/DeviceBuilder.hpp
    graphics/vulkan/Instance.cpp
    graphics/vulkan/Instance.hpp
    graphics/vulkan/InstanceBuilder.cpp
    graphics/vulkan/InstanceBuilder.hpp
    graphics/vulkan/PhysicalDevice.hpp
    graphics/vulkan/PhysicalDeviceSelector.cpp
    graphics/vulkan/PhysicalDeviceSelector.hpp
    graphics/vulkan/SwapchainBuilder.cpp
    graphics/vulkan/SwapchainBuilder.hpp
    graphics/vulkan/VulkanContext.hpp
    graphics/vulkan/VulkanContext.cpp

    graphics/vulkan/vulkan_buffer.cpp
    graphics/vulkan/vulkan_buffer.hpp
    graphics/vulkan/vulkan_command_buffer.cpp
    graphics/vulkan/vulkan_command_buffer.hpp
    graphics/vulkan/vulkan_descriptors.cpp
    graphics/vulkan/vulkan_descriptors.hpp
    graphics/vulkan/vulkan_device.cpp
    graphics/vulkan/vulkan_device.hpp
    graphics/vulkan/vulkan_framebuffer.cpp
    graphics/vulkan/vulkan_framebuffer.hpp
    graphics/vulkan/vulkan_image.hpp
    graphics/vulkan/vulkan_image.cpp
    graphics/vulkan/vulkan_pipeline.cpp
    graphics/vulkan/vulkan_pipeline.hpp
    graphics/vulkan/vulkan_renderpass.cpp
    graphics/vulkan/vulkan_renderpass.hpp
    graphics/vulkan/vulkan_settings.hpp
    graphics/vulkan/vulkan_shader.cpp
    graphics/vulkan/vulkan_shader.hpp
    graphics/vulkan/vulkan_surface.cpp
    graphics/vulkan/vulkan_surface.hpp
    graphics/vulkan/vulkan_sync.cpp
    graphics/vulkan/vulkan_sync.hpp
    graphics/vulkan/vulkan_utils.cpp
    graphics/vulkan/vulkan_utils.hpp
)

target_include_directories(Editor
    PRIVATE core graphics graphics/vulkan
)

target_link_libraries(Editor
    PRIVATE Engine Vulkan::Vulkan glfw imgui glm::glm stb tiny_obj_loader
)

target_compile_options(Editor PRIVATE
    $<$<CONFIG:Debug>:${CXX_DEBUG_COMPILE_FLAGS}>
    $<$<CONFIG:Release>:${CXX_RELEASE_COMPILE_FLAGS}>)

add_custom_target(Shaders ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/shaders"
    COMMAND glslc shader.vert -o shader.vert.spv
    COMMAND glslc shader.frag -o shader.frag.spv
    COMMAND move ./*.spv "${CMAKE_CURRENT_BINARY_DIR}/shaders"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/shaders"
    COMMENT "Compiling Shaders..."
)
