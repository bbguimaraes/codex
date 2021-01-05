cmake_minimum_required(VERSION 3.0)
project(codex)
option(ENABLE_TESTS "build test binaries")
option(ENABLE_BENCHMARKS "build benchmark binaries")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(WARNING_FLAGS -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion)
add_library(warning_flags INTERFACE)
target_compile_options(
    warning_flags INTERFACE
    $<$<CXX_COMPILER_ID:Clang,GNU>:
        $<BUILD_INTERFACE:${WARNING_FLAGS}>>)

if(ENABLE_TESTS)
    set(
        TEST_FLAGS
        $<$<CXX_COMPILER_ID:Clang,GNU>:
            $<BUILD_INTERFACE:-fsanitize=address,undefined>>)
    set(CMAKE_AUTOMOC ON)
    find_package(Qt5 COMPONENTS Core Test REQUIRED)
    add_library(test_flags INTERFACE)
    target_compile_options(test_flags INTERFACE ${TEST_FLAGS})
    target_link_options(test_flags INTERFACE ${TEST_FLAGS})
    target_link_libraries(test_flags INTERFACE warning_flags Qt5::Test)
    enable_testing()
endif()

if(ENABLE_BENCHMARKS)
    set(CMAKE_AUTOMOC ON)
    find_package(Qt5 COMPONENTS Core Test REQUIRED)
    add_library(benchmark_flags INTERFACE)
    target_link_libraries(benchmark_flags INTERFACE warning_flags Qt5::Test)
    enable_testing()
endif()

add_custom_target(docs doxygen WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
set_target_properties(
    docs PROPERTIES ADDITIONAL_CLEAN_FILES
    "${PROJECT_SOURCE_DIR}/docs/html;${PROJECT_SOURCE_DIR}/docs/latex")
add_subdirectory(base64)
add_subdirectory(lambda)
