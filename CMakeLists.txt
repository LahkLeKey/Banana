cmake_minimum_required(VERSION 3.22)

project(CInteropSharpNative C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(CINTEROP_ENABLE_POSTGRES "Enable legacy PostgreSQL access in native C layer" ON)
option(CINTEROP_ENABLE_NATIVE_TESTS "Build native C tests" ON)
option(CINTEROP_ENABLE_COVERAGE "Build with coverage instrumentation" OFF)
option(CINTEROP_BUILD_SHARED_LIBRARY "Build the cinterop_native shared library target" ON)

if(CINTEROP_ENABLE_COVERAGE AND NOT MSVC)
    add_compile_options(--coverage -O0)
    add_link_options(--coverage)
endif()

set(CINTEROP_POSTGRES_ENABLED OFF)
if(CINTEROP_ENABLE_POSTGRES)
    find_package(PostgreSQL)
    if(PostgreSQL_FOUND)
        set(CINTEROP_POSTGRES_ENABLED ON)
    else()
        message(WARNING "CINTEROP_ENABLE_POSTGRES is ON, but PostgreSQL client libraries were not found. Falling back to non-PostgreSQL native mode for this build.")
    endif()
endif()

if(CINTEROP_BUILD_SHARED_LIBRARY)
    add_library(cinterop_native SHARED
        src/native/legacy/legacy_points.c
        src/native/legacy/legacy_db.c
        src/native/testing/native_test_hooks_default.c
        src/native/wrapper/cinterop_wrapper.c
    )

    target_include_directories(cinterop_native
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/src/native/wrapper
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src/native/legacy
            ${CMAKE_CURRENT_SOURCE_DIR}/src/native/testing
    )

    if(MSVC)
        target_compile_options(cinterop_native PRIVATE /W4)
    else()
        target_compile_options(cinterop_native PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    if(CINTEROP_POSTGRES_ENABLED)
        target_compile_definitions(cinterop_native PRIVATE CINTEROP_ENABLE_POSTGRES)
        target_include_directories(cinterop_native PRIVATE ${PostgreSQL_INCLUDE_DIRS})
        target_link_libraries(cinterop_native PRIVATE ${PostgreSQL_LIBRARIES})
    endif()

    set_target_properties(cinterop_native PROPERTIES
        OUTPUT_NAME "cinterop_native"
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    )
endif()

if(CINTEROP_ENABLE_NATIVE_TESTS)
    enable_testing()

    add_executable(native_wrapper_tests
        tests/native/native_wrapper_tests.c
        tests/native/native_test_hooks_env.c
        src/native/legacy/legacy_points.c
        src/native/legacy/legacy_db.c
        src/native/wrapper/cinterop_wrapper.c
    )

    target_include_directories(native_wrapper_tests
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src/native/wrapper
            ${CMAKE_CURRENT_SOURCE_DIR}/src/native/legacy
            ${CMAKE_CURRENT_SOURCE_DIR}/src/native/testing
    )

    if(MSVC)
        target_compile_options(native_wrapper_tests PRIVATE /W4)
    else()
        target_compile_options(native_wrapper_tests PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    if(CINTEROP_POSTGRES_ENABLED)
        target_compile_definitions(native_wrapper_tests PRIVATE CINTEROP_ENABLE_POSTGRES)
        target_include_directories(native_wrapper_tests PRIVATE ${PostgreSQL_INCLUDE_DIRS})
        target_link_libraries(native_wrapper_tests PRIVATE ${PostgreSQL_LIBRARIES})
    endif()

    add_test(NAME native_wrapper_tests COMMAND native_wrapper_tests)
endif()
