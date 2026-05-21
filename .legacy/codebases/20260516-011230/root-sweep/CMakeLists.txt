cmake_minimum_required(VERSION 3.20)
project(banana_native C)

# Spec 006: CMake target name `banana_wrapper` is a hard contract.
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Optional: Postgres DAL (libpq). Loud-gate at runtime via BANANA_PG_CONNECTION.
option(BANANA_ENABLE_DAL "Build the Postgres-backed DAL module" OFF)

set(BANANA_DOMAIN_SOURCES
  src/native/calc/domain/services/calc_service.c
  src/native/classify/domain/services/classify_service.c
  src/native/ripeness/domain/services/ripeness_service.c
  src/native/status/domain/services/status_service.c
  src/native/batch/domain/services/batch_service.c
  src/native/anticheat/domain/entities/anticheat_session.c
  src/native/anticheat/domain/services/anticheat_scoring.c
  src/native/anticheat/domain/services/anticheat_trust.c
  src/native/anticheat/domain/services/anticheat_evidence.c
  src/native/anticheat/domain/services/anticheat_config.c
  src/native/anticheat/domain/services/anticheat_metrics.c
  src/native/anticheat/domain/services/anticheat_synthetic.c
  src/native/ml/domain/regression/regression_model.c
  src/native/ml/domain/binary/binary_model.c
  src/native/ml/domain/transformer/transformer_model.c
  src/native/ml/domain/histogram/histogram_matrix.c
  src/native/ml/domain/services/ml_controller_service.c
  src/native/wrapper/banana_core_compat.c
  src/native/wrapper/banana_dal_compat.c
)

set(BANANA_WRAPPER_SOURCES
  src/native/wrapper/banana_wrapper.c
  src/native/wrapper/domain/ml/shared/banana_wrapper_ml_json.c
  src/native/wrapper/domain/ml/regression/banana_wrapper_ml_regression.c
  src/native/wrapper/domain/ml/binary/banana_wrapper_ml_binary.c
  src/native/wrapper/domain/ml/transformer/banana_wrapper_ml_transformer.c
)

add_library(banana_native SHARED
  ${BANANA_DOMAIN_SOURCES}
  ${BANANA_WRAPPER_SOURCES}
)

# Alias enforces spec 006 contract name.
add_library(banana_wrapper ALIAS banana_native)

target_include_directories(banana_native PUBLIC
  src/native/wrapper
  src/native/calc/domain/services
  src/native/classify/domain/services
  src/native/ripeness/domain/services
  src/native/status/domain/services
  src/native/batch/domain/entities
  src/native/batch/domain/services
  src/native/anticheat/domain/entities
  src/native/anticheat/domain/services
  src/native/ml/domain/regression
  src/native/ml/domain/binary
  src/native/ml/domain/transformer
  src/native/ml/domain/histogram
  src/native/ml/domain/services
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

option(BANANA_ENABLE_COVERAGE "Enable gcov code coverage instrumentation" OFF)
if(BANANA_ENABLE_COVERAGE)
  target_compile_options(banana_native PRIVATE -fprofile-arcs -ftest-coverage)
  target_link_options(banana_native PRIVATE -fprofile-arcs -ftest-coverage)
endif()

# Game engine core modules (Phase 1 pivot).
option(BANANA_ENABLE_ENGINE "Build the game engine core (render, physics, ai, world)" OFF)
if(BANANA_ENABLE_ENGINE)
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/native/engine/CMakeLists.txt")
    add_subdirectory(src/native/engine)
  endif()
endif()

# Native test binary (driven by spec 014).
enable_testing()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/native/CMakeLists.txt")
  add_subdirectory(tests/native)
endif()

# Engine test binary (Phase 1 setup).
if(BANANA_ENABLE_ENGINE AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/native/engine/CMakeLists.txt")
  add_subdirectory(tests/native/engine)
endif()
