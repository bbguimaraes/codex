cmake_minimum_required(VERSION 3.0)

set(CMAKE_AUTOMOC ON)
find_package(Qt5 COMPONENTS Core Test REQUIRED)

if(ENABLE_TESTS)
    add_executable(
        event_dispatcher_test event_dispatcher.cpp event_dispatcher_test.cpp)
    target_include_directories(
        event_dispatcher_test PRIVATE "${PROJECT_SOURCE_DIR}")
    target_link_libraries(event_dispatcher_test PRIVATE test_flags)
    add_test(NAME event_dispatcher_test COMMAND event_dispatcher_test)
endif()
