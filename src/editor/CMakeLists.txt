include(FetchContent)

find_package(Vulkan REQUIRED)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)

FetchContent_MakeAvailable(glfw)

add_executable(Editor
    main.cpp

    ui/IUndoableCommand.hpp
    ui/UndoRedoStack.hpp

    graphics/Camera.hpp
    graphics/Vertex.hpp

    graphics/vulkan/Device.hpp
    graphics/vulkan/DeviceBuilder.cpp
    graphics/vulkan/DeviceBuilder.hpp

    graphics/vulkan/PhysicalDevice.hpp
    graphics/vulkan/PhysicalDeviceSelector.cpp
    graphics/vulkan/PhysicalDeviceSelector.hpp
    graphics/vulkan/SwapchainBuilder.cpp
    graphics/vulkan/SwapchainBuilder.hpp
    graphics/vulkan/VulkanContext.hpp
    graphics/vulkan/VulkanContext.cpp

    graphics/vulkan/vulkan_buffer.cpp
    graphics/vulkan/vulkan_buffer.hpp
    graphics/vulkan/vulkan_descriptors.cpp
    graphics/vulkan/vulkan_descriptors.hpp
    graphics/vulkan/vulkan_image.hpp
    graphics/vulkan/vulkan_image.cpp
    graphics/vulkan/vulkan_pipeline.cpp
    graphics/vulkan/vulkan_pipeline.hpp
    graphics/vulkan/vulkan_renderpass.cpp
    graphics/vulkan/vulkan_renderpass.hpp
)

target_include_directories(Editor
    PRIVATE core graphics graphics/vulkan
)

target_link_libraries(Editor
    PRIVATE Engine Vulkan::Vulkan glfw imgui stb tiny_obj_loader
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
