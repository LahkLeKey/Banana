cmake_minimum_required(VERSION 3.22)

project(CInteropSharpNative C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BANANA_ENABLE_POSTGRES "Enable PostgreSQL access in native DAL layer" ON)
option(BANANA_ENABLE_NATIVE_TESTS "Build native C tests" ON)
option(BANANA_ENABLE_COVERAGE "Build with coverage instrumentation" OFF)
option(BANANA_BUILD_SHARED_LIBRARY "Build the banana_native shared library target" ON)

if(BANANA_ENABLE_COVERAGE AND NOT MSVC)
    add_compile_options(--coverage -O0)
    add_link_options(--coverage)
endif()

set(BANANA_POSTGRES_ENABLED OFF)
if(BANANA_ENABLE_POSTGRES)
    find_package(PostgreSQL)
    if(PostgreSQL_FOUND)
        set(BANANA_POSTGRES_ENABLED ON)
    else()
        message(WARNING "BANANA_ENABLE_POSTGRES is ON, but PostgreSQL client libraries were not found. Falling back to non-PostgreSQL native mode for this build.")
    endif()
endif()

set(BANANA_CORE_SOURCES
    src/native/core/domain/banana_agriculture.c
    src/native/core/domain/banana_application.c
    src/native/core/domain/banana_cultivation.c
    src/native/core/domain/banana_events.c
    src/native/core/domain/banana_integration.c
    src/native/core/domain/banana_inventory.c
    src/native/core/domain/banana_lifecycle.c
    src/native/core/domain/banana_logistics.c
    src/native/core/domain/banana_ml_models.c
    src/native/core/domain/banana_profile.c
    src/native/core/domain/banana_processing.c
    src/native/core/domain/banana_read_models.c
    src/native/core/domain/banana_repositories.c
    src/native/core/domain/banana_retail.c
    src/native/core/domain/banana_ripeness.c
    src/native/core/domain/banana_services.c
    src/native/core/domain/banana_supply_chain.c
)

set(BANANA_DAL_SOURCES
    src/native/core/dal/banana_db.c
    src/native/core/dal/domain/banana_cultivation.c
    src/native/core/dal/domain/banana_inventory.c
    src/native/core/dal/domain/banana_processing.c
    src/native/core/dal/domain/banana_profile_projection.c
    src/native/core/dal/domain/banana_supply_chain.c
    src/native/core/dal/internal/banana_postgres.c
)

set(BANANA_WRAPPER_SOURCES
    src/native/wrapper/banana_wrapper.c
    src/native/wrapper/dal/banana_db.c
    src/native/wrapper/domain/banana_logistics.c
    src/native/wrapper/domain/banana_processing.c
    src/native/wrapper/domain/banana_profile.c
    src/native/wrapper/domain/banana_ripeness.c
    src/native/wrapper/domain/banana_supply_chain.c
)

if(BANANA_BUILD_SHARED_LIBRARY)
    add_library(banana_native SHARED
        ${BANANA_CORE_SOURCES}
        ${BANANA_DAL_SOURCES}
        src/native/testing/native_test_hooks_default.c
        ${BANANA_WRAPPER_SOURCES}
    )

    target_include_directories(banana_native
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/src/native/wrapper
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src/native/core
            ${CMAKE_CURRENT_SOURCE_DIR}/src/native/core/dal
            ${CMAKE_CURRENT_SOURCE_DIR}/src/native/testing
    )

    if(MSVC)
        target_compile_options(banana_native PRIVATE /W4)
    else()
        target_compile_options(banana_native PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    if(BANANA_POSTGRES_ENABLED)
        target_compile_definitions(banana_native PRIVATE BANANA_ENABLE_POSTGRES)
        target_include_directories(banana_native PRIVATE ${PostgreSQL_INCLUDE_DIRS})
        target_link_libraries(banana_native PRIVATE ${PostgreSQL_LIBRARIES})
    endif()

    set_target_properties(banana_native PROPERTIES
        OUTPUT_NAME "banana_native"
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    )
endif()

if(BANANA_ENABLE_NATIVE_TESTS)
    enable_testing()

    add_executable(native_test_hooks_default_tests
        tests/native/native_test_hooks_default_tests.c
        src/native/testing/native_test_hooks_default.c
    )

    target_include_directories(native_test_hooks_default_tests
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src/native/testing
    )

    if(MSVC)
        target_compile_options(native_test_hooks_default_tests PRIVATE /W4)
    else()
        target_compile_options(native_test_hooks_default_tests PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    add_test(NAME native_test_hooks_default_tests COMMAND native_test_hooks_default_tests)

    add_executable(banana_wrapper_tests
        tests/native/banana_wrapper_tests.c
        tests/native/native_test_hooks_env.c
        ${BANANA_CORE_SOURCES}
        ${BANANA_DAL_SOURCES}
        ${BANANA_WRAPPER_SOURCES}
    )

    target_include_directories(banana_wrapper_tests
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src/native/core
            ${CMAKE_CURRENT_SOURCE_DIR}/src/native/wrapper
            ${CMAKE_CURRENT_SOURCE_DIR}/src/native/core/dal
            ${CMAKE_CURRENT_SOURCE_DIR}/src/native/testing
    )

    if(MSVC)
        target_compile_options(banana_wrapper_tests PRIVATE /W4)
    else()
        target_compile_options(banana_wrapper_tests PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    if(BANANA_POSTGRES_ENABLED)
        target_compile_definitions(banana_wrapper_tests PRIVATE BANANA_ENABLE_POSTGRES)
        target_include_directories(banana_wrapper_tests PRIVATE ${PostgreSQL_INCLUDE_DIRS})
        target_link_libraries(banana_wrapper_tests PRIVATE ${PostgreSQL_LIBRARIES})
    endif()

    add_test(NAME banana_wrapper_tests COMMAND banana_wrapper_tests)

    add_executable(banana_core_tests
        tests/native/banana_core_tests.c
        ${BANANA_DAL_SOURCES}
        src/native/testing/native_test_hooks_default.c
        ${BANANA_CORE_SOURCES}
    )

    target_include_directories(banana_core_tests
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src/native/core
            ${CMAKE_CURRENT_SOURCE_DIR}/src/native/core/dal
            ${CMAKE_CURRENT_SOURCE_DIR}/src/native/testing
    )

    if(MSVC)
        target_compile_options(banana_core_tests PRIVATE /W4)
    else()
        target_compile_options(banana_core_tests PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    if(BANANA_POSTGRES_ENABLED)
        target_compile_definitions(banana_core_tests PRIVATE BANANA_ENABLE_POSTGRES)
        target_include_directories(banana_core_tests PRIVATE ${PostgreSQL_INCLUDE_DIRS})
        target_link_libraries(banana_core_tests PRIVATE ${PostgreSQL_LIBRARIES})
    endif()

    add_test(NAME banana_core_tests COMMAND banana_core_tests)
endif()
