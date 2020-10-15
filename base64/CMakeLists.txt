cmake_minimum_required(VERSION 3.0)

set(CMAKE_AUTOMOC ON)
find_package(Qt5 COMPONENTS Core Test REQUIRED)

add_executable(base64 base64.cpp main.cpp)
target_link_libraries(base64 PRIVATE warning_flags)

add_executable(base64_rfc rfc/base64.c rfc/main.cpp)
set_property(TARGET base64_rfc PROPERTY OUTPUT_NAME base64)
set_property(TARGET base64_rfc PROPERTY RUNTIME_OUTPUT_DIRECTORY rfc)
target_include_directories(base64_rfc PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(base64_rfc PRIVATE warning_flags)

if(ENABLE_BENCHMARKS)
    add_executable(
        base64_bench
        base64.cpp rfc/base64.c bench/bench.cpp ../bench/bench.cpp)
    set_property(TARGET base64_bench PROPERTY OUTPUT_NAME bench)
    set_property(TARGET base64_bench PROPERTY RUNTIME_OUTPUT_DIRECTORY bench)
    target_include_directories(
        base64_bench
        PRIVATE "${PROJECT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
    target_link_libraries(base64_bench PRIVATE benchmark_flags)
endif()

if(ENABLE_TESTS)
    add_executable(base64_test base64.cpp rfc/base64.c test/test.cpp)
    target_include_directories(
        base64_test
        PRIVATE "${PROJECT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
    set_property(TARGET base64_test PROPERTY OUTPUT_NAME test)
    set_property(TARGET base64_test PROPERTY RUNTIME_OUTPUT_DIRECTORY test)
    target_link_libraries(base64_test PRIVATE test_flags)
    add_test(NAME base64_test COMMAND test/test)
endif()
