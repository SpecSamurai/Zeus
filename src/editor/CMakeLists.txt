add_executable(Editor
    EditorApp.cpp
)

target_include_directories(Editor
    PRIVATE componenets
)

target_link_libraries(Editor
    PRIVATE Engine imgui
)

target_compile_options(Editor PRIVATE
    $<$<CONFIG:Debug>:${CXX_DEBUG_COMPILE_FLAGS}>
    $<$<CONFIG:Release>:${CXX_RELEASE_COMPILE_FLAGS}>)
