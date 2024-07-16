include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(Tests
    memory/MemoryTest.cpp
    memory/LinearAllocatorTest.cpp
    memory/FreeListAllocatorTest.cpp

    engine/math/TransformationsTest.cpp
    engine/math/TrigonometricTest.cpp
    engine/math/Matrix4x4Test.cpp
    engine/math/Vector2Test.cpp
    engine/math/Vector3Test.cpp
    engine/math/Vector4Test.cpp

    engine/core/dsa/algorithmsTest.cpp
    engine/core/dsa/LinkedListTest.cpp
    engine/core/dsa/DisjointSetTest.cpp
    engine/core/dsa/StackTest.cpp
    engine/core/dsa/QueueTest.cpp
    engine/core/dsa/HeapTest.cpp
    engine/core/dsa/BinarySearchTreeTest.cpp
)

target_link_libraries(Tests
    PRIVATE GTest::gtest_main Engine
)

target_compile_options(Tests PRIVATE
    $<$<CONFIG:Debug>:${CXX_DEBUG_COMPILE_FLAGS}>
    $<$<CONFIG:Release>:${CXX_RELEASE_COMPILE_FLAGS}>)

include(GoogleTest)
gtest_discover_tests(Tests)
