cmake_minimum_required(VERSION 3.20)
project(diskscan VERSION 1.0 DESCRIPTION "Сбор информации о заполненности диска" LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build" FORCE)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(DISKSCAN_BUILD_TESTS "Собрать тесты GoogleTest" OFF)

if(POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "src/*.cpp")
list(REMOVE_ITEM SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

add_library(core_lib STATIC ${SRC_FILES})
target_include_directories(core_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

if(WIN32)
    target_compile_definitions(core_lib PUBLIC WIN32_LEAN_AND_MEAN NOMINMAX UNICODE _UNICODE)
    target_link_libraries(core_lib PUBLIC advapi32 shell32)
endif()

if(MSVC)
    target_compile_options(core_lib PUBLIC /utf-8 /EHsc /Zc:__cplusplus)
    target_compile_options(core_lib PRIVATE /W4 /WX)
else()
    target_compile_options(core_lib PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

add_executable(diskscan src/main.cpp)
target_link_libraries(diskscan PRIVATE core_lib)
if(MINGW OR (WIN32 AND CMAKE_CXX_COMPILER_ID MATCHES "GNU"))
    target_link_options(diskscan PRIVATE -static)
endif()

set_target_properties(diskscan PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")

if(DISKSCAN_BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()
    file(GLOB_RECURSE TEST_FILES CONFIGURE_DEPENDS "tests/*.test.cpp")
    add_executable(diskscan_tests ${TEST_FILES})
    target_include_directories(diskscan_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(diskscan_tests PRIVATE core_lib GTest::gtest_main)
    if(MINGW OR (WIN32 AND CMAKE_CXX_COMPILER_ID MATCHES "GNU"))
        target_link_options(diskscan_tests PRIVATE -static)
    endif()

    include(GoogleTest)
    gtest_discover_tests(diskscan_tests DISCOVERY_MODE PRE_TEST)
endif()
