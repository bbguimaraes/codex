cmake_minimum_required(VERSION 3.0)

set(CMAKE_AUTOMOC ON)
find_package(Qt5 COMPONENTS Core Test REQUIRED)

if(ENABLE_TESTS)
    add_executable(c_wrapper_test c_wrapper.cpp c_wrapper_test.cpp)
    target_include_directories(c_wrapper_test PRIVATE "${PROJECT_SOURCE_DIR}")
    target_link_libraries(c_wrapper_test PRIVATE test_flags)
    add_test(NAME c_wrapper_test COMMAND c_wrapper_test)
endif()
