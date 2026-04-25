cmake_minimum_required(VERSION 3.20)
project(banana_native C)

# Spec 006: CMake target name `banana_wrapper` is a hard contract.
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Optional: Postgres DAL (libpq). Loud-gate at runtime via BANANA_PG_CONNECTION.
option(BANANA_ENABLE_DAL "Build the Postgres-backed DAL module" OFF)

set(BANANA_CORE_SOURCES
  src/native/core/banana_status.c
  src/native/core/banana_calc.c
  src/native/core/banana_classify.c
  src/native/core/banana_ripeness.c
  src/native/core/banana_batch.c
  src/native/core/banana_truck.c
  src/native/core/domain/banana_not_banana.c
  src/native/core/dal/banana_dal.c
)

set(BANANA_WRAPPER_SOURCES
  src/native/wrapper/banana_wrapper.c
)

add_library(banana_native SHARED
  ${BANANA_CORE_SOURCES}
  ${BANANA_WRAPPER_SOURCES}
)

# Alias enforces spec 006 contract name.
add_library(banana_wrapper ALIAS banana_native)

target_include_directories(banana_native PUBLIC
  src/native/core
  src/native/core/dal
  src/native/wrapper
)

if(WIN32)
  target_compile_definitions(banana_native PRIVATE BANANA_BUILDING_DLL=1)
endif()

if(BANANA_ENABLE_DAL)
  find_package(PostgreSQL QUIET)
  if(PostgreSQL_FOUND)
    target_link_libraries(banana_native PRIVATE PostgreSQL::PostgreSQL)
    target_compile_definitions(banana_native PRIVATE BANANA_DAL_LIBPQ=1)
  endif()
endif()

# Native test binary (driven by spec 014).
enable_testing()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/native/CMakeLists.txt")
  add_subdirectory(tests/native)
endif()
