add_executable(Editor
    EditorApp.cpp
    EditorApp.hpp
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
