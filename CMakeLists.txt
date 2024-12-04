cmake_minimum_required(VERSION 3.29)
project(AOC_2024
        VERSION 1.0
        DESCRIPTION "Advent of Code 2024 Solutions"
        LANGUAGES CXX
)

include(FetchContent)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_DEBUG_POSTFIX "d")

set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILDS ON)

include(CheckCXXCompilerFlag)

# Options
# --------------------------------------------------------------------------------
option(AOC_ENABLE_TESTING "Enable test suite" ON)
option(AOC_ENABLE_SANITIZERS "Enable sanitizers in debug builds" OFF)
option(AOC_ENABLE_ASAN "Enable Address Sanitizer" ON)
option(AOC_ENABLE_UBSAN "Enable Undefined Behavior Sanitizer" ON)
option(AOC_ENABLE_TSAN "Enable Thread Sanitizer" OFF)
# --------------------------------------------------------------------------------

function(add_cxx_compiler_flag FLAG)
    check_cxx_compiler_flag("${FLAG}" FLAG_SUPPORTED)
    if (FLAG_SUPPORTED)
        add_compile_options("${FLAG}")
    endif ()
endfunction()

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3)
    add_compile_options(-flto)
    add_link_options(-flto)
endif ()

if (MSVC)
    add_compile_options(/W4 /permissive-)
endif ()

function(add_strict_compile_options target visibility)
    if (NOT MSVC)
        target_compile_options(${target} ${visibility}
                -march=native
                -mtune=native
                -Wall
                -Wextra
                -Wpedantic
                -Wconversion
                -Wshadow
                -Wcast-align
                -Wcast-qual
                -Wformat=2
                -Wunused
                -Wnon-virtual-dtor
                -Woverloaded-virtual
        )
    endif ()
endfunction()

include(CheckIPOSupported)
check_ipo_supported(RESULT IPO_SUPPORTED)
if (IPO_SUPPORTED)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif ()

if (AOC_ENABLE_SANITIZERS AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        if (AOC_ENABLE_ASAN)
            add_compile_options(-fsanitize=address)
            add_link_options(-fsanitize=address)
        endif ()
        if (AOC_ENABLE_UBSAN)
            add_compile_options(-fsanitize=undefined)
            add_link_options(-fsanitize=undefined)
        endif ()
        if (AOC_ENABLE_TSAN)
            add_compile_options(-fsanitize=thread)
            add_link_options(-fsanitize=thread)
        endif ()
    endif ()
endif ()

find_package(TBB QUIET)
if (NOT TBB_FOUND)
    if (WIN32)
        message(FATAL_ERROR "TBB not found - please install Intel oneAPI Threading Building Blocks")
    endif ()
    message(STATUS "TBB not found - will download and build from source")

    FetchContent_Declare(
            tbb
            GIT_REPOSITORY https://github.com/oneapi-src/oneTBB.git
            GIT_TAG v2022.0.0
    )

    set(TBB_TEST OFF)
    set(TBB_EXAMPLES OFF)

    FetchContent_MakeAvailable(tbb)
endif ()

find_package(Boost 1.86 COMPONENTS container QUIET)
if (NOT Boost_FOUND)
    message(STATUS "Boost not found - will download and build from source")

    FetchContent_Declare(
            boost
            URL https://github.com/boostorg/boost/releases/download/boost-1.86.0/boost-1.86.0-cmake.tar.gz
    )
    set(BOOST_INCLUDE_LIBRARIES container)
    set(BOOST_ENABLE_CMAKE ON)
    FetchContent_MakeAvailable(boost)
endif ()

if (AOC_ENABLE_TESTING)
    enable_testing()

    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.15.2
    )
    FetchContent_MakeAvailable(googletest)

    # Test executable
    add_executable(${PROJECT_NAME}_test
            test/Day01Test.cpp
            test/Day02Test.cpp
            test/Day03Test.cpp)
    add_strict_compile_options(${PROJECT_NAME}_test PRIVATE)
    target_compile_definitions(${PROJECT_NAME}_test
            PRIVATE
            TESTING
    )
    target_link_libraries(${PROJECT_NAME}_test
            PRIVATE
            aoc_lib
            GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(${PROJECT_NAME}_test)
endif ()

# Library
add_library(aoc_lib INTERFACE
        src/Day01.h
        src/Day02.h
        src/aoc/AocTemplates.h
        src/aoc/AocExceptions.h
        src/Day03.h
        src/aoc/Profiler.h
)
add_strict_compile_options(aoc_lib INTERFACE)
target_include_directories(aoc_lib
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/aoc
)
target_link_libraries(aoc_lib
        INTERFACE
        TBB::tbb
        Boost::container
)

# Main executable
add_executable(${PROJECT_NAME} src/main.cpp)
add_strict_compile_options(${PROJECT_NAME} PRIVATE)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        aoc_lib
)

target_precompile_headers(aoc_lib INTERFACE
        <vector>
        <span>
        <ranges>
        <expected>
        <filesystem>
        <execution>
        <algorithm>
        <numeric>
        <boost/container/small_vector.hpp>
)

if (WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_libraries(${PROJECT_NAME}
            PRIVATE
            "-lstdc++exp"
    )
endif ()