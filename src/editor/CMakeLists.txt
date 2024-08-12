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

    graphics/temp.hpp

    graphics/Camera.hpp

    graphics/vulkan/VulkanContext.hpp
    graphics/vulkan/VulkanContext.cpp
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

find_program(GLSLANG_VALIDATOR glslangValidator HINTS $ENV{VULKAN_SDK}/Bin/)

file(GLOB_RECURSE SHADER_SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.vert"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.tesc"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.tese"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.geom"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.frag"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.comp"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.mesh"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.task")

foreach(SHADER_FILE ${SHADER_SOURCE_FILES})
    get_filename_component(FILE_NAME ${SHADER_FILE} NAME)

    set(SPIRV "${CMAKE_CURRENT_BINARY_DIR}/shaders/${FILE_NAME}.spv")

    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${GLSLANG_VALIDATOR} -g -V ${SHADER_FILE} -o ${SPIRV}
        DEPENDS ${SHADER_FILE}
        COMMENT "Compiling Shader: ${SHADER_FILE}")

    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(SHADER_FILE)

add_custom_target(Shaders ALL DEPENDS ${SPIRV_BINARY_FILES})
