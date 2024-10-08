include(FetchContent)

find_package(Vulkan REQUIRED)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)

FetchContent_MakeAvailable(glfw)

add_library(imgui STATIC
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

    imgui/backends/imgui_impl_vulkan.cpp
    imgui/backends/imgui_impl_vulkan.h
    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_glfw.h
)

target_include_directories(imgui PUBLIC imgui)
target_link_libraries(imgui PRIVATE Vulkan::Vulkan glfw)

add_library(VulkanMemoryAllocator INTERFACE)
target_include_directories(VulkanMemoryAllocator INTERFACE VulkanMemoryAllocator/include)

add_library(stb
    stb/stb_image.h
    stb/stb_image.c
)

target_include_directories(stb
    INTERFACE stb
)
