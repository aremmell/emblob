################################################################################
# emblob/examples CMake script
#
# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: Copyright (c) 2018-2024 Ryan M. Lederman

set(SIMPLE_EXAMPLE_EXE_NAME simple)
set(STRUCT_EXAMPLE_EXE_NAME struct)

add_executable(
    ${SIMPLE_EXAMPLE_EXE_NAME}
    simple.cc
)

add_executable(
    ${STRUCT_EXAMPLE_EXE_NAME}
    struct.cc
)

target_compile_features(
    ${SIMPLE_EXAMPLE_EXE_NAME}
    PRIVATE
    ${CXX_STANDARD}
)

target_compile_features(
    ${STRUCT_EXAMPLE_EXE_NAME}
    PRIVATE
    ${CXX_STANDARD}
)

target_include_directories(
    ${SIMPLE_EXAMPLE_EXE_NAME}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../
)

target_include_directories(
    ${STRUCT_EXAMPLE_EXE_NAME}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../
)

add_dependencies(
    ${SIMPLE_EXAMPLE_EXE_NAME}
    RUN_EMBLOB_SIMPLE
)

add_dependencies(
    ${STRUCT_EXAMPLE_EXE_NAME}
    RUN_EMBLOB_STRUCT
)

target_link_libraries(
    ${SIMPLE_EXAMPLE_EXE_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/../simple.o
)

target_link_libraries(
    ${STRUCT_EXAMPLE_EXE_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/../struct.o
)
