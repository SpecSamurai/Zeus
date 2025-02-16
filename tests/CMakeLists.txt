include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set_target_properties(
    gmock
    gmock_main
    gtest
    gtest_main
    PROPERTIES FOLDER "Libraries")

enable_testing()

add_executable(Tests
    engine/commands/CommandStackTest.cpp

    engine/ecs/ComponentSparseSetTest.cpp
    engine/ecs/FamilyIdTest.cpp
    engine/ecs/SparseSetIteratorTest.cpp
    engine/ecs/SparseSetTest.cpp
    engine/ecs/WorldTest.cpp

    engine/events/EventDispatcherTest.cpp
    engine/events/EventQueueTest.cpp
    engine/events/EventTest_types.hpp

    engine/math/GeometricTest.cpp
    engine/math/Matrix3x3Test.cpp
    engine/math/Matrix4x4Test.cpp
    engine/math/QuaternionTest.cpp
    engine/math/TransformationsTest.cpp
    engine/math/TrigonometricTest.cpp
    engine/math/Vector2Test.cpp
    engine/math/Vector3Test.cpp
    engine/math/Vector4Test.cpp

    engine/memory/FreeListAllocatorTest.cpp
    engine/memory/LinearAllocatorTest.cpp
    engine/memory/MemoryTest.cpp
)

target_link_libraries(Tests
    PRIVATE GTest::gtest_main Engine
)

target_compile_options(Tests PRIVATE
    $<$<CONFIG:Debug>:${CXX_DEBUG_COMPILE_FLAGS}>
    $<$<CONFIG:Release>:${CXX_RELEASE_COMPILE_FLAGS}>)

include(GoogleTest)
gtest_discover_tests(Tests)
