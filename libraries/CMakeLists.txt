add_library(imgui
    imgui/imconfig.h
    imgui/imgui.cpp
    imgui/imgui.h
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_internal.h
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/imstb_rectpack.h
    imgui/imstb_textedit.h
    imgui/imstb_truetype.h
)

target_include_directories(imgui
    INTERFACE imgui imgui/backends
)

target_compile_options(imgui PRIVATE
    $<$<CONFIG:Debug>:${CXX_DEBUG_COMPILE_FLAGS}>
    $<$<CONFIG:Release>:${CXX_RELEASE_COMPILE_FLAGS}>)

add_library(stb
    stb/stb_image.h
    stb/stb_image.c
)

target_include_directories(stb
    INTERFACE stb
)

add_library(tiny_obj_loader
    tiny_obj_loader/tiny_obj_loader.h
    tiny_obj_loader/tiny_obj_loader.cc
)

target_include_directories(tiny_obj_loader
    INTERFACE tiny_obj_loader
)

target_compile_options(tiny_obj_loader PRIVATE
    $<$<CONFIG:Debug>:${CXX_DEBUG_COMPILE_FLAGS}>
    $<$<CONFIG:Release>:${CXX_RELEASE_COMPILE_FLAGS}>)
