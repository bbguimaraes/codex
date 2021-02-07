cmake_minimum_required(VERSION 3.0)

set(CMAKE_AUTOMOC ON)
find_package(Qt5 COMPONENTS Core Test REQUIRED)

if(ENABLE_TESTS)
    add_executable(rotate_test rotate.cpp rotate_test.cpp)
    target_compile_options(
        rotate_test PRIVATE
        -O2 -funsafe-math-optimizations -ffinite-math-only)
    target_include_directories(rotate_test PRIVATE "${PROJECT_SOURCE_DIR}")
    target_link_libraries(rotate_test PRIVATE Qt5::Test)
    add_test(NAME rotate_test COMMAND rotate_test)
endif()
