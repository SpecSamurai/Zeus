add_executable(Editor
    EditorApp.cpp
)

target_link_libraries(Editor
    PRIVATE Engine
)

target_compile_options(Editor PRIVATE
    $<$<CONFIG:Debug>:${CXX_DEBUG_COMPILE_FLAGS}>
    $<$<CONFIG:Release>:${CXX_RELEASE_COMPILE_FLAGS}>)
