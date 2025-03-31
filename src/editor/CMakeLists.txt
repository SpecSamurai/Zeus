add_executable(Editor
    EditorApp.cpp
    EditorApp.hpp
    UserInterface.cpp
    UserInterface.hpp

    widgets/Demo.hpp
    widgets/MenuBar.cpp
    widgets/MenuBar.hpp
    widgets/Profiler.cpp
    widgets/Profiler.hpp
    widgets/Viewport.cpp
    widgets/Viewport.hpp
    widgets/Widget.hpp
)

target_include_directories(Editor
    PRIVATE .
)

target_link_libraries(Editor
    PRIVATE Engine imgui
)

target_compile_options(Editor PRIVATE
    $<$<CONFIG:Debug>:${CXX_DEBUG_COMPILE_FLAGS}>
    $<$<CONFIG:Release>:${CXX_RELEASE_COMPILE_FLAGS}>)
