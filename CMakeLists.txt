################################################################################
# emblob CMake script
#
# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: Copyright (c) 2018-2024 Ryan M. Lederman

cmake_minimum_required(VERSION 3.12.0)

# variables
set(PROJECT_NAME emblob)
set(EMBLOB_EXECUTABLE_NAME emblob)
set(C_EXAMPLE_EXECUTABLE_NAME cexample)
set(CXX_EXAMPLE_EXECUTABLE_NAME cxxexample)

# versioning
set(PROJECT_VERSION_MAJOR 2)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 0)
set(PROJECT_VERSION_SUFFIX -dev)
set(PROJECT_IS_REL_BUILD false)

# define the project
project(
    ${PROJECT_NAME}
    VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}
    LANGUAGES C CXX
    DESCRIPTION "blob embedding tool"
)

# compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

# set the C/C++ standards
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.12")
    set(CMAKE_C_STANDARD 17)
    set(CMAKE_C_STANDARD_REQUIRED true)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED true)
else()
    message(FATAL_ERROR, "CMake >= 3.12 is required in order to build libbal")
endif()

# policy settings, so CMake doesn't whine.
cmake_policy(SET CMP0025 NEW) # AppleClang
cmake_policy(SET CMP0048 NEW) # project VERSION
cmake_policy(SET CMP0065 NEW) # ENABLE_EXPORTS
cmake_policy(SET CMP0056 NEW) # CMAKE_EXE_LINKER_FLAGS
cmake_policy(SET CMP0066 NEW) # CMAKE_<LANG>_FLAGS_<BUILDTYPE>

if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.13")
    cmake_policy(SET CMP0077 NEW) # option
endif()

if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.17")
    cmake_policy(SET CMP0102 NEW) # mark_as_advanced
endif()

if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.21")
    cmake_policy(SET CMP0126 NEW) # cache
endif()

# preprocessor defines
add_compile_definitions(
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)

# compiler options
if (MSVC)
    add_compile_options(
        /W4 /MP /GS /experimental:c11atomics /wd4267
        $<$<CONFIG:Debug>:/Od>
        $<$<CONFIG:Debug>:/Zi>
        $<$<CONFIG:Release>:/Ox>
    )
else()
    set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
    set(THREADS_PREFER_PTHREAD_FLAG TRUE)
    find_package(Threads REQUIRED)

    # -Wformat-nonliteral is also an option
    add_compile_options(
        -Wall -Wextra -Wpedantic -Werror -Wdouble-promotion -Wconversion -Wno-sign-conversion -Wno-string-conversion -Wshift-overflow
        $<$<CONFIG:Debug>:-O0>
        $<$<CONFIG:Debug>:-g3>
        $<$<CONFIG:Release>:-O3>
    )

    if (CMAKE_COMPILER_IS_GNUCC)
        add_compile_options(
            -Wformat-truncation
        )
    endif()

    if (THREADS_HAVE_PTHREAD_ARG)
        add_compile_options(
            -pthread
        )
        add_link_options(
            -pthread
        )
    endif()

    add_compile_definitions(
        $<$<CONFIG:Release>:_FORTIFY_SOURCE=2>
    )
endif()

execute_process(
    COMMAND git rev-parse --short --verify HEAD
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ECHO_OUTPUT_VARIABLE
)

configure_file(
    include/emblob/version.hh.in
    include/emblob/version.hh
    NEWLINE_STYLE LF
)

add_executable(
    ${EMBLOB_EXECUTABLE_NAME}
    src/emblob.cc
)

add_custom_target(
    RUN_EMBLOB
    COMMAND rm test.o || true
    COMMAND rm test.S || true
    COMMAND build/emblob -i test.bin -l debug
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    BYPRODUCTS ${CMAKE_CURRENT_SOURCE_DIR}/test.o
    COMMENT "execute emblob with test.bin as input"
)

add_dependencies(
    RUN_EMBLOB
    ${EMBLOB_EXECUTABLE_NAME}
)

add_dependencies(
    RUN_EMBLOB
    ${EMBLOB_EXECUTABLE_NAME}
)

set_source_files_properties(
    test.o
    PROPERTIES
    EXTERNAL_OBJECT true
    GENERATED true
)

add_executable(
    ${C_EXAMPLE_EXECUTABLE_NAME}
    example/cexample.c
)

add_dependencies(
    ${C_EXAMPLE_EXECUTABLE_NAME}
    RUN_EMBLOB
)

add_executable(
    ${CXX_EXAMPLE_EXECUTABLE_NAME}
    example/cxxexample.cc
)

add_dependencies(
    ${CXX_EXAMPLE_EXECUTABLE_NAME}
    RUN_EMBLOB
)

target_include_directories(
    ${EMBLOB_EXECUTABLE_NAME}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/include
)

target_include_directories(
    ${C_EXAMPLE_EXECUTABLE_NAME}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/include
)

target_link_libraries(
    ${C_EXAMPLE_EXECUTABLE_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/test.o
)

target_link_libraries(
    ${CXX_EXAMPLE_EXECUTABLE_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/test.o
)

target_include_directories(
    ${CXX_EXAMPLE_EXECUTABLE_NAME}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/include
)

target_compile_features(
    ${EMBLOB_EXECUTABLE_NAME}
    PUBLIC
    ${CXX_STANDARD}
)

target_compile_features(
    ${C_EXAMPLE_EXECUTABLE_NAME}
    PUBLIC
    ${C_STANDARD}
)

target_compile_features(
    ${CXX_EXAMPLE_EXECUTABLE_NAME}
    PUBLIC
    ${CXX_STANDARD}
)
