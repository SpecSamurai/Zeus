add_executable(Editor
    EditorApp.cpp
    EditorApp.hpp
    UserInterface.cpp
    UserInterface.hpp

    widgets/AssetBrowser.cpp
    widgets/AssetBrowser.hpp
    widgets/LogViewer.cpp
    widgets/LogViewer.hpp
    widgets/MenuBar.cpp
    widgets/MenuBar.hpp
    widgets/Properties.cpp
    widgets/Properties.hpp
    widgets/Settings.cpp
    widgets/Settings.hpp
    widgets/ShaderEditor.cpp
    widgets/ShaderEditor.hpp
    widgets/Viewport.cpp
    widgets/Viewport.hpp
    widgets/Widget.hpp
    widgets/WorldViewer.cpp
    widgets/WorldViewer.hpp
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
