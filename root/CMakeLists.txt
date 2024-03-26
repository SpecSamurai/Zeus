cmake_minimum_required(VERSION 3.28)
project(
    Zeus
    VERSION 0.1.0
    DESCRIPTION "Game Engine"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Vulkan REQUIRED)

add_subdirectory(Source/Editor)
add_subdirectory(Source/Engine)

add_subdirectory(Tests)
