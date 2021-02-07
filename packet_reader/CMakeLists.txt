cmake_minimum_required(VERSION 3.0)

set(CMAKE_AUTOMOC ON)
find_package(Qt5 COMPONENTS Core Test REQUIRED)

if(ENABLE_TESTS)
    add_executable(
        packet_reader_test packet_reader.cpp packet_reader_test.cpp)
    target_include_directories(
        packet_reader_test PRIVATE "${PROJECT_SOURCE_DIR}")
    target_link_libraries(packet_reader_test PRIVATE test_flags)
    add_test(NAME packet_reader_test COMMAND packet_reader_test)
endif()
