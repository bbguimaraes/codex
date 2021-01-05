cmake_minimum_required(VERSION 3.0)

set(CMAKE_AUTOMOC ON)
find_package(Qt5 COMPONENTS Core Test REQUIRED)

if(ENABLE_TESTS)
    add_executable(mutable_test mutable_test.cpp)
    target_link_libraries(mutable_test PRIVATE test_flags)
    add_test(NAME lambda_mutable_test COMMAND mutable_test)
endif()
