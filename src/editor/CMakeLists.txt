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
    Application.hpp
    ui/UndoRedoStack.hpp
    ui/IUndoableCommand.hpp
    core/files.hpp
    graphics/glfw_utils.hpp
    graphics/glfw_utils.cpp
    graphics/vulkan/VulkanContext.hpp
    graphics/vulkan/vulkan_settings.hpp
    graphics/vulkan/vulkan_utils.hpp
    graphics/vulkan/vulkan_utils.cpp
    graphics/vulkan/vulkan_instance.hpp
    graphics/vulkan/vulkan_instance.cpp
    graphics/vulkan/vulkan_debug.hpp
    graphics/vulkan/vulkan_debug.cpp
    graphics/vulkan/vulkan_sync.hpp
    graphics/vulkan/vulkan_sync.cpp
    graphics/vulkan/vulkan_surface.hpp
    graphics/vulkan/vulkan_surface.cpp
    graphics/vulkan/vulkan_device.hpp
    graphics/vulkan/vulkan_device.cpp
    graphics/vulkan/vulkan_swapchain.hpp
    graphics/vulkan/vulkan_swapchain.cpp
    graphics/vulkan/vulkan_image.hpp
    graphics/vulkan/vulkan_image.cpp
    graphics/vulkan/vulkan_shader.hpp
    graphics/vulkan/vulkan_shader.cpp
    graphics/vulkan/vulkan_pipeline.hpp
    graphics/vulkan/vulkan_pipeline.cpp
    graphics/vulkan/vulkan_command_buffer.hpp
    graphics/vulkan/vulkan_command_buffer.cpp
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
    COMMAND mv ./*.spv "${CMAKE_CURRENT_BINARY_DIR}/shaders"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/shaders"
    COMMENT "Compiling Shaders..."
)
