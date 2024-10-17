cmake_minimum_required(VERSION 3.28)

project(
    Zeus
    VERSION 0.1.0
    DESCRIPTION "Game Engine"
    LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(libraries)

add_subdirectory(src/editor)
add_subdirectory(src/engine)

add_subdirectory(tests)

execute_process(
    COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_SOURCE_DIR}/compile_commands.json
)
