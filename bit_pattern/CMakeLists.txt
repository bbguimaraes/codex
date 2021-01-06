cmake_minimum_required(VERSION 3.0)

set(CMAKE_AUTOMOC ON)
find_package(Qt5 COMPONENTS Core Test REQUIRED)

if(ENABLE_TESTS)
    add_executable(bit_pattern_test bit_pattern_test.cpp)
    target_link_libraries(bit_pattern_test PRIVATE test_flags)
    add_test(NAME bit_pattern_test COMMAND bit_pattern_test)
endif()
