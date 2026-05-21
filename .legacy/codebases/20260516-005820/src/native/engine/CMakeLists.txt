cmake_minimum_required(VERSION 3.20)

# ─────────────────────────────────────────────────────────────────────────────
# banana_engine_core — static C library (render, physics, ai, world modules)
# ─────────────────────────────────────────────────────────────────────────────

set(ENGINE_RENDER_SOURCES
  render/window.c
  render/shader.c
  render/mesh.c
  render/material.c
  render/camera.c
  render/renderer.c
)

set(ENGINE_PHYSICS_SOURCES
  physics/body.c
  physics/collider.c
  physics/dynamics.c
  physics/world.c
)

set(ENGINE_AI_SOURCES
  ai/controller.c
  ai/controller_system.c
  ai/ml_agent_controller.c
  ai/state_machine.c
  ai/navigation.c
  ai/perception.c
  ai/player_controller.c
  ai/wildlife_controller.c
)

set(ENGINE_WORLD_SOURCES
  world/entity.c
  world/world.c
  world/signals.c
)

set(ENGINE_DOMAIN_SERVICE_SOURCES
  domain/services/batch_engine_bridge.c
  domain/services/cellular_worldgen_service.c
  domain/services/ml_controller_bridge.c
  domain/services/wfc_worldgen_service.c
)

set(ENGINE_ML_SOURCES
  ../ml/domain/services/ml_controller_service.c
  ../ml/domain/regression/regression_model.c
  ../ml/domain/binary/binary_model.c
  ../ml/domain/transformer/transformer_model.c
  ../ml/domain/histogram/histogram_matrix.c
)

add_library(banana_engine_core STATIC
  engine.c
  ${ENGINE_RENDER_SOURCES}
  ${ENGINE_PHYSICS_SOURCES}
  ${ENGINE_AI_SOURCES}
  ${ENGINE_WORLD_SOURCES}
  ${ENGINE_DOMAIN_SERVICE_SOURCES}
  ${ENGINE_ML_SOURCES}
)

option(BANANA_ENGINE_ENABLE_PARALLEL_WORLDGEN "Enable OpenMP worldgen parallelism" ON)

target_include_directories(banana_engine_core PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/render
  ${CMAKE_CURRENT_SOURCE_DIR}/physics
  ${CMAKE_CURRENT_SOURCE_DIR}/ai
  ${CMAKE_CURRENT_SOURCE_DIR}/world
)

if(MSVC)
  target_compile_options(banana_engine_core PRIVATE /W3)
  target_compile_definitions(banana_engine_core PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
  target_compile_options(banana_engine_core PRIVATE
    -std=c11 -O2 -Wall -Wextra -Wpedantic
  )
endif()

if(BANANA_ENGINE_ENABLE_PARALLEL_WORLDGEN AND NOT EMSCRIPTEN)
  find_package(OpenMP QUIET)
  if(OpenMP_C_FOUND)
    target_link_libraries(banana_engine_core PUBLIC OpenMP::OpenMP_C)
    target_compile_definitions(banana_engine_core PUBLIC BANANA_ENGINE_PARALLEL_WORLDGEN=1)
    message(STATUS "[engine] OpenMP worldgen parallelism enabled")
  else()
    message(STATUS "[engine] OpenMP not found; worldgen running single-threaded")
  endif()
endif()

# ─────────────────────────────────────────────────────────────────────────────
# Platform: Native (GLFW + OpenGL)
# Only linked when NOT cross-compiling to WASM
# ─────────────────────────────────────────────────────────────────────────────

if(NOT EMSCRIPTEN)
  add_executable(banana_asset_compiler
    tools/banana_asset_compiler.c
    tools/domain/banana_asset_domain.c
    tools/domain/banana_asset_algorithms.c
    tools/application/banana_asset_generation_service.c
    tools/infrastructure/banana_asset_json_writer.c
  )

  target_include_directories(banana_asset_compiler PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/tools
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/domain
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/application
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/infrastructure
  )

  if(MSVC)
    target_compile_options(banana_asset_compiler PRIVATE /W3)
    target_compile_definitions(banana_asset_compiler PRIVATE _CRT_SECURE_NO_WARNINGS)
  else()
    target_compile_options(banana_asset_compiler PRIVATE
      -std=c11 -O2 -Wall -Wextra -Wpedantic
    )
  endif()

  find_package(OpenGL QUIET)
  find_package(glfw3 QUIET)

  if(OpenGL_FOUND AND glfw3_FOUND)
    target_link_libraries(banana_engine_core PUBLIC
      OpenGL::GL
      glfw
    )
    target_compile_definitions(banana_engine_core PUBLIC BANANA_ENGINE_HAS_GLFW=1)
    message(STATUS "[engine] Native rendering: OpenGL + GLFW enabled")
  else()
    message(WARNING "[engine] OpenGL or GLFW not found — rendering stubs will be used")
    target_compile_definitions(banana_engine_core PUBLIC BANANA_ENGINE_STUB_RENDER=1)
  endif()

  if(UNIX)
    target_link_libraries(banana_engine_core PUBLIC m)
  endif()
endif()

# ─────────────────────────────────────────────────────────────────────────────
# Platform: WASM (Emscripten)
# Exports C functions for WASM orchestration layer
# ─────────────────────────────────────────────────────────────────────────────

if(EMSCRIPTEN)
  # EXPORTED_FUNCTIONS list — mirrors the ABI contract in spec 001
  set(WASM_EXPORTS
    "_main"
    "_engine_init"
    "_engine_tick"
    "_engine_shutdown"
    "_engine_render_frame"
    "_engine_get_frame_buffer"
    "_engine_world_spawn"
    "_engine_world_tick"
    "_engine_controller_create"
    "_engine_controller_attach"
    "_engine_controller_update"
    "_engine_controller_signal"
    "_engine_get_entity_count"
    "_engine_get_entity_x"
    "_engine_get_entity_z"
    "_engine_get_entity_state"
    # Keyboard-only movement intent API (mouse move path intentionally inert)
    "_engine_set_move_input"
    "_engine_get_click_count"
    "_engine_get_target_reached_count"
    "_engine_get_has_move_target"
    "_physics_step"
    "_physics_add_body"
    "_physics_update_body"
  )

  # Join into a JSON array string for Emscripten
  string(JOIN "," WASM_EXPORTS_STR ${WASM_EXPORTS})

  add_executable(banana_engine_wasm
    engine.c
    wasm_main.c
    ${ENGINE_RENDER_SOURCES}
    ${ENGINE_PHYSICS_SOURCES}
    ${ENGINE_AI_SOURCES}
    ${ENGINE_WORLD_SOURCES}
    ${ENGINE_DOMAIN_SERVICE_SOURCES}
  )

  target_include_directories(banana_engine_wasm PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
  )

  target_compile_options(banana_engine_wasm PRIVATE
    -std=c11
    -O2
    -Wall
    -fopenmp
    -pthread
  )

  set_target_properties(banana_engine_wasm PROPERTIES
    OUTPUT_NAME "banana_engine"
    # No SUFFIX override — Emscripten emits .js + .wasm automatically
  )

  target_link_options(banana_engine_wasm PRIVATE
    "SHELL:-s WASM=1"
    "SHELL:-fopenmp"
    "SHELL:-pthread"
    "SHELL:-s USE_PTHREADS=1"
    "SHELL:-s PTHREAD_POOL_SIZE=4"
    "SHELL:-s PROXY_TO_PTHREAD=1"
    "SHELL:-s EXPORTED_FUNCTIONS=[${WASM_EXPORTS_STR}]"
    "SHELL:-s EXPORTED_RUNTIME_METHODS=[cwrap,ccall,getValue,setValue]"
    "SHELL:-s ALLOW_MEMORY_GROWTH=1"
    "SHELL:-s INITIAL_MEMORY=33554432"
    "SHELL:-s NO_EXIT_RUNTIME=1"
    # Frame buffer at fixed offset (32 MB base + 8 MB frame buffer region)
    "SHELL:-s TOTAL_STACK=8388608"
    "SHELL:-s MODULARIZE=1"
    "SHELL:-s EXPORT_NAME=BananaEngine"
  )

  target_compile_definitions(banana_engine_wasm PRIVATE BANANA_ENGINE_WASM=1)

  message(STATUS "[engine] WASM build target: banana_engine.wasm")
endif()

# ─────────────────────────────────────────────────────────────────────────────
# Coverage instrumentation (mirrors root pattern)
# ─────────────────────────────────────────────────────────────────────────────

if(BANANA_ENABLE_COVERAGE AND NOT EMSCRIPTEN)
  target_compile_options(banana_engine_core PRIVATE -fprofile-arcs -ftest-coverage)
  target_link_options(banana_engine_core PRIVATE -fprofile-arcs -ftest-coverage)
endif()
