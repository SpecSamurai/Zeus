include(FetchContent)

find_package(Vulkan REQUIRED)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)

#FetchContent_Declare(
#    assimp
#    GIT_REPOSITORY https://github.com/assimp/assimp.git
#    GIT_TAG v5.3.1
#)

FetchContent_MakeAvailable(glfw)
#FetchContent_MakeAvailable(assimp)

#target_compile_options(assimp PUBLIC -Wno-error)

add_executable(Editor
    main.cpp
    Application.hpp
    ui/UndoRedoStack.hpp
    ui/IUndoableCommand.hpp
    core/logger.hpp
    core/files.hpp
    graphics/glfw_utils.hpp
    graphics/glfw_utils.cpp
    graphics/vulkan_utils.hpp
    graphics/vulkan_utils.cpp
    graphics/vulkan_settings.hpp
    graphics/VulkanInstance.cpp
    graphics/VulkanInstance.hpp
    graphics/VulkanDevice.cpp
)

target_include_directories(Editor
    PRIVATE core graphics
)

target_link_libraries(Editor
    PRIVATE Engine Vulkan::Vulkan glfw imgui
)

target_compile_options(Editor PRIVATE
    $<$<CONFIG:Debug>:${CXX_DEBUG_COMPILE_FLAGS}>
    $<$<CONFIG:Release>:${CXX_RELEASE_COMPILE_FLAGS}>)

add_custom_command(TARGET Editor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/shaders"
    COMMAND glslc shader.vert -o shader.vert.spv
    COMMAND glslc shader.frag -o shader.frag.spv
    COMMAND mv ./*.spv "${CMAKE_CURRENT_BINARY_DIR}/shaders"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/shaders"
    COMMENT "Compiling shaders..."
)
