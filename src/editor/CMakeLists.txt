find_package(Vulkan REQUIRED)

add_executable(Editor
    main.cpp

    Application.cpp
    Application.hpp

    ui/IUndoableCommand.hpp
    ui/UIManager.cpp
    ui/UIManager.hpp
    ui/UndoRedoStack.hpp
)

target_link_libraries(Editor
    PRIVATE Engine Vulkan::Vulkan glfw imgui
)

target_compile_options(Editor PRIVATE
    $<$<CONFIG:Debug>:${CXX_DEBUG_COMPILE_FLAGS}>
    $<$<CONFIG:Release>:${CXX_RELEASE_COMPILE_FLAGS}>)
