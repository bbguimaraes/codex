cmake_minimum_required(VERSION 3.0)

set(CMAKE_AUTOMOC ON)
find_package(Qt5 COMPONENTS Core Test REQUIRED)

add_executable(thread thread.c)
target_include_directories(thread PRIVATE "${PROJECT_SOURCE_DIR}")
target_link_libraries(thread PRIVATE warning_flags)

#if(ENABLE_BENCHMARKS)
#    add_executable(bsearch_bench bench.cpp ../bench/bench.cpp)
#    set_property(TARGET bsearch_bench PROPERTY OUTPUT_NAME bench)
#    target_include_directories(bsearch_bench PRIVATE "${PROJECT_SOURCE_DIR}")
#    target_link_libraries(bsearch_bench PRIVATE benchmark_flags)
#endif()

#if(ENABLE_TESTS)
#    add_executable(bsearch_test test.cpp)
#    target_include_directories(bsearch_test PRIVATE "${PROJECT_SOURCE_DIR}")
#    set_property(TARGET bsearch_test PROPERTY OUTPUT_NAME test)
#    target_link_libraries(bsearch_test PRIVATE test_flags)
#    add_test(NAME bsearch_test COMMAND test)
#endif()
