cmake_minimum_required(VERSION 3.22)

project(CInteropSharpNative C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

option(BUILD_SHARED_LIBS "Build shared libraries" ON)

add_library(cinterop_native SHARED
    src/native/legacy/legacy_points.c
    src/native/wrapper/cinterop_wrapper.c
)

target_include_directories(cinterop_native
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/native/wrapper
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/native/legacy
)

if(MSVC)
    target_compile_options(cinterop_native PRIVATE /W4)
else()
    target_compile_options(cinterop_native PRIVATE -Wall -Wextra -Wpedantic)
endif()

set_target_properties(cinterop_native PROPERTIES
    OUTPUT_NAME "cinterop_native"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
