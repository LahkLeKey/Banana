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
  ai/state_machine.c
  ai/navigation.c
  ai/perception.c
)

set(ENGINE_WORLD_SOURCES
  world/entity.c
  world/world.c
  world/signals.c
)

add_library(banana_engine_core STATIC
  engine.c
  ${ENGINE_RENDER_SOURCES}
  ${ENGINE_PHYSICS_SOURCES}
  ${ENGINE_AI_SOURCES}
  ${ENGINE_WORLD_SOURCES}
)

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

# ─────────────────────────────────────────────────────────────────────────────
# Platform: Native (GLFW + OpenGL)
# Only linked when NOT cross-compiling to WASM
# ─────────────────────────────────────────────────────────────────────────────

if(NOT EMSCRIPTEN)
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
    "_engine_tick"
    "_engine_render_frame"
    "_engine_get_frame_buffer"
    "_physics_world_step"
    "_physics_add_body"
    "_physics_update_body"
    "_controller_create"
    "_controller_update"
    "_controller_signal"
    "_world_spawn_entity"
    "_world_tick"
    "_world_send_signal"
    "_world_query_nearby"
  )

  # Join into a JSON array string for Emscripten
  string(JOIN "," WASM_EXPORTS_STR ${WASM_EXPORTS})

  add_executable(banana_engine_wasm
    ${ENGINE_RENDER_SOURCES}
    ${ENGINE_PHYSICS_SOURCES}
    ${ENGINE_AI_SOURCES}
    ${ENGINE_WORLD_SOURCES}
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
  )

  set_target_properties(banana_engine_wasm PROPERTIES
    OUTPUT_NAME "banana_engine"
    SUFFIX ".wasm"
  )

  target_link_options(banana_engine_wasm PRIVATE
    "SHELL:-s WASM=1"
    "SHELL:-s EXPORTED_FUNCTIONS=[${WASM_EXPORTS_STR}]"
    "SHELL:-s EXPORTED_RUNTIME_METHODS=[cwrap,ccall,getValue,setValue]"
    "SHELL:-s ALLOW_MEMORY_GROWTH=1"
    "SHELL:-s INITIAL_MEMORY=33554432"
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
