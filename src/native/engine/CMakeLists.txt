cmake_minimum_required(VERSION 3.20)

# ─────────────────────────────────────────────────────────────────────────────
# banana_engine_core — static C library (render, physics, ai, world modules)
# ─────────────────────────────────────────────────────────────────────────────

set(ENGINE_RENDER_SOURCES
  render/window.c
  render/shader.c
  render/mesh.c
  render/mesh_banana.c
  render/mesh_terrain.c
  render/material.c
  render/camera.c
  render/renderer.c
  render/renderer_draw.c
  render/actor_mesh_registry.c
)

set(ENGINE_PHYSICS_SOURCES
  physics/body.c
  physics/collider_broad_phase.c
  physics/collider_narrow_phase.c
  physics/collider_resolve.c
  physics/dynamics.c
  physics/raycast.c
  physics/world_collision.c
  physics/world.c
)

set(ENGINE_AI_SOURCES
  ai/controller.c
  ai/controller_system_dispatch.c
  ai/controller_system_lifecycle.c
  ai/navigation.c
  ai/npc_merchant.c
  ai/perception.c
  ai/state_machine.c
  ai/wildlife_controller.c
)

set(ENGINE_WORLD_SOURCES
  world/entity.c
  world/world_query.c
  world/world.c
  world/signals.c
)

set(ENGINE_UI_SOURCES
  ui/ui.c
  ui/ui_render.c
  ui/ui_inventory.c
  ui/ui_merchant.c
  ui/ui_callback_bindings.c
)

set(ENGINE_RUNTIME_SOURCES
  runtime/camera_basis.c
  runtime/camera_follow.c
  runtime/controller_attach.c
  runtime/controller_runtime.c
  runtime/controller_sync.c
  runtime/engine_host.c
  runtime/engine_lifecycle.c
  runtime/engine_tick.c
  runtime/input_contract.c
  runtime/input_binding.c
  runtime/interactive_objects.c
  runtime/interlock.c
  runtime/parallel_contract.c
  runtime/parallel_model.c
  runtime/physics_abi.c
  runtime/player_api.c
  runtime/player_builds.c
  runtime/player_motion_host.c
  runtime/player_motion.c
  runtime/player_registry.c
  runtime/player_resources.c
  runtime/interactive_collection.c
  runtime/interactive_object_spawner.c
  runtime/chunk_stream_packet.c
  runtime/render_material.c
  runtime/render_submit.c
  runtime/terrain_features.c
  runtime/terrain_generation.c
  runtime/terrain_host.c
  runtime/terrain_height.c
  runtime/terrain_mutation.c
  runtime/terrain_runtime.c
  runtime/terrain_chunks.c
  runtime/tick_phases.c
  runtime/world_abi.c
  runtime/world_metrics.c
  runtime/world_telemetry.c
  runtime/wildlife_gameplay.c
  tools/domain/banana_asset_domain.c
)

add_library(banana_engine_core STATIC
  engine.c
  engine_serialize.c
  ${ENGINE_RUNTIME_SOURCES}
  ${ENGINE_RENDER_SOURCES}
  ${ENGINE_PHYSICS_SOURCES}
  ${ENGINE_AI_SOURCES}
  ${ENGINE_WORLD_SOURCES}
  ${ENGINE_UI_SOURCES}
)

target_include_directories(banana_engine_core PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/render
  ${CMAKE_CURRENT_SOURCE_DIR}/physics
  ${CMAKE_CURRENT_SOURCE_DIR}/ai
  ${CMAKE_CURRENT_SOURCE_DIR}/world
  ${CMAKE_CURRENT_SOURCE_DIR}/ui
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
  add_executable(banana_asset_compiler
    tools/banana_asset_compiler.c
    tools/domain/banana_asset_domain.c
    tools/domain/banana_asset_algorithms.c
    tools/domain/banana_asset_wfc.c
    tools/domain/banana_asset_cellular.c
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

  find_package(OpenMP QUIET)
  if(OpenMP_C_FOUND)
    target_link_libraries(banana_engine_core PUBLIC OpenMP::OpenMP_C)
    target_link_libraries(banana_asset_compiler PRIVATE OpenMP::OpenMP_C)
    target_compile_definitions(banana_engine_core PUBLIC BANANA_ENGINE_HAS_OPENMP=1)
    target_compile_definitions(banana_asset_compiler PRIVATE BANANA_ENGINE_HAS_OPENMP=1)
    message(STATUS "[engine] Parallel controller sync: OpenMP enabled")
  else()
    message(FATAL_ERROR "[engine] OpenMP is required: strict parallel runtime has no serial fallback")
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
    "_engine_get_active_player_count"
    # Keyboard-only movement intent API (mouse move path intentionally inert)
    "_engine_set_move_input"
    "_engine_player_upsert"
    "_engine_player_apply_input"
    "_engine_player_set_transform"
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
    engine_serialize.c
    wasm_main.c
    ${ENGINE_RUNTIME_SOURCES}
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
    # No SUFFIX override — Emscripten emits .js + .wasm automatically
  )

  target_link_options(banana_engine_wasm PRIVATE
    "SHELL:-s WASM=1"
    "SHELL:-s USE_WEBGL2=1"
    "SHELL:-s FULL_ES3=1"
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
  target_compile_definitions(banana_engine_wasm PRIVATE BANANA_ENGINE_HAS_OPENMP=1)

  message(STATUS "[engine] WASM build target: banana_engine.wasm")
endif()

# ─────────────────────────────────────────────────────────────────────────────
# Coverage instrumentation (mirrors root pattern)
# ─────────────────────────────────────────────────────────────────────────────

if(BANANA_ENABLE_COVERAGE AND NOT EMSCRIPTEN)
  target_compile_options(banana_engine_core PRIVATE -fprofile-arcs -ftest-coverage)
  target_link_options(banana_engine_core PRIVATE -fprofile-arcs -ftest-coverage)
endif()

if(BUILD_TESTING AND NOT EMSCRIPTEN)
  add_executable(banana_engine_serialize_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/engine_serialize_test.c
  )
  target_link_libraries(banana_engine_serialize_test PRIVATE banana_engine_core)
  target_include_directories(banana_engine_serialize_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
  )
  add_test(NAME banana_engine_serialize_test COMMAND banana_engine_serialize_test)

  add_executable(banana_runtime_terrain_mutation_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime_terrain_mutation_test.c
  )
  target_link_libraries(banana_runtime_terrain_mutation_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_terrain_mutation_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
  )
  add_test(NAME banana_runtime_terrain_mutation_test COMMAND banana_runtime_terrain_mutation_test)

  add_executable(banana_runtime_terrain_generation_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime_terrain_generation_test.c
  )
  target_link_libraries(banana_runtime_terrain_generation_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_terrain_generation_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
  )
  add_test(NAME banana_runtime_terrain_generation_test COMMAND banana_runtime_terrain_generation_test)

  add_executable(banana_collider_broad_phase_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/collider_broad_phase_test.c
  )
  target_link_libraries(banana_collider_broad_phase_test PRIVATE banana_engine_core)
  target_include_directories(banana_collider_broad_phase_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
  )
  add_test(NAME banana_collider_broad_phase_test COMMAND banana_collider_broad_phase_test)

  add_executable(banana_physics_raycast_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/physics_raycast_test.c
  )
  target_link_libraries(banana_physics_raycast_test PRIVATE banana_engine_core)
  target_include_directories(banana_physics_raycast_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
  )
  add_test(NAME banana_physics_raycast_test COMMAND banana_physics_raycast_test)

  add_executable(banana_render_mesh_terrain_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/render_mesh_terrain_test.c
  )
  target_link_libraries(banana_render_mesh_terrain_test PRIVATE banana_engine_core)
  target_include_directories(banana_render_mesh_terrain_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
  )
  add_test(NAME banana_render_mesh_terrain_test COMMAND banana_render_mesh_terrain_test)

  add_executable(banana_controller_system_dispatch_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/controller_system_dispatch_test.c
  )
  target_link_libraries(banana_controller_system_dispatch_test PRIVATE banana_engine_core)
  target_include_directories(banana_controller_system_dispatch_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
  )
  add_test(NAME banana_controller_system_dispatch_test COMMAND banana_controller_system_dispatch_test)

  add_executable(banana_render_headless_frame_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/render_headless_frame_test.c
  )
  target_link_libraries(banana_render_headless_frame_test PRIVATE banana_engine_core)
  target_include_directories(banana_render_headless_frame_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
  )
  add_test(NAME banana_render_headless_frame_test COMMAND banana_render_headless_frame_test)

  add_executable(banana_world_query_nearby_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/world_query_nearby_test.c
  )
  target_link_libraries(banana_world_query_nearby_test PRIVATE banana_engine_core)
  target_include_directories(banana_world_query_nearby_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
  )
  add_test(NAME banana_world_query_nearby_test COMMAND banana_world_query_nearby_test)

  add_executable(banana_runtime_math_helpers_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime_math_helpers_test.c
  )
  target_link_libraries(banana_runtime_math_helpers_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_math_helpers_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
  )
  add_test(NAME banana_runtime_math_helpers_test COMMAND banana_runtime_math_helpers_test)

  add_executable(banana_runtime_controller_world_metrics_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime_controller_world_metrics_test.c
  )
  target_link_libraries(banana_runtime_controller_world_metrics_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_controller_world_metrics_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
  )
  add_test(NAME banana_runtime_controller_world_metrics_test COMMAND banana_runtime_controller_world_metrics_test)

  add_executable(banana_runtime_telemetry_input_contract_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime_telemetry_input_contract_test.c
  )
  target_link_libraries(banana_runtime_telemetry_input_contract_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_telemetry_input_contract_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
  )
  add_test(NAME banana_runtime_telemetry_input_contract_test COMMAND banana_runtime_telemetry_input_contract_test)

  add_executable(banana_runtime_physics_world_abi_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime_physics_world_abi_test.c
  )
  target_link_libraries(banana_runtime_physics_world_abi_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_physics_world_abi_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
  )
  add_test(NAME banana_runtime_physics_world_abi_test COMMAND banana_runtime_physics_world_abi_test)

  add_executable(banana_runtime_engine_lifecycle_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime_engine_lifecycle_test.c
  )
  target_link_libraries(banana_runtime_engine_lifecycle_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_engine_lifecycle_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
  )
  add_test(NAME banana_runtime_engine_lifecycle_test COMMAND banana_runtime_engine_lifecycle_test)

  add_executable(banana_runtime_controller_attach_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime_controller_attach_test.c
  )
  target_link_libraries(banana_runtime_controller_attach_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_controller_attach_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
  )
  add_test(NAME banana_runtime_controller_attach_test COMMAND banana_runtime_controller_attach_test)

  add_executable(banana_runtime_engine_tick_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime_engine_tick_test.c
  )
  target_link_libraries(banana_runtime_engine_tick_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_engine_tick_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
  )
  add_test(NAME banana_runtime_engine_tick_test COMMAND banana_runtime_engine_tick_test)

  add_executable(banana_runtime_terrain_host_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime_terrain_host_test.c
  )
  target_link_libraries(banana_runtime_terrain_host_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_terrain_host_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
  )
  add_test(NAME banana_runtime_terrain_host_test COMMAND banana_runtime_terrain_host_test)

  add_executable(banana_runtime_player_api_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime_player_api_test.c
  )
  target_link_libraries(banana_runtime_player_api_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_player_api_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
  )
  add_test(NAME banana_runtime_player_api_test COMMAND banana_runtime_player_api_test)

  add_executable(banana_runtime_player_builds_integration_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime_player_builds_integration_test.c
  )
  target_link_libraries(banana_runtime_player_builds_integration_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_player_builds_integration_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
  )
  add_test(NAME banana_runtime_player_builds_integration_test COMMAND banana_runtime_player_builds_integration_test)

  add_executable(banana_runtime_render_material_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime_render_material_test.c
  )
  target_link_libraries(banana_runtime_render_material_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_render_material_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
  )
  add_test(NAME banana_runtime_render_material_test COMMAND banana_runtime_render_material_test)

  add_executable(banana_runtime_player_motion_host_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime_player_motion_host_test.c
  )
  target_link_libraries(banana_runtime_player_motion_host_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_player_motion_host_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
  )
  add_test(NAME banana_runtime_player_motion_host_test COMMAND banana_runtime_player_motion_host_test)

  add_executable(banana_runtime_engine_host_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime_engine_host_test.c
  )
  target_link_libraries(banana_runtime_engine_host_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_engine_host_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
  )
  add_test(NAME banana_runtime_engine_host_test COMMAND banana_runtime_engine_host_test)

  add_executable(banana_runtime_interlock_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime_interlock_test.c
  )
  target_link_libraries(banana_runtime_interlock_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_interlock_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
  )
  add_test(NAME banana_runtime_interlock_test COMMAND banana_runtime_interlock_test)

  add_executable(banana_runtime_parallel_amdahl_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime_parallel_amdahl_test.c
  )
  target_link_libraries(banana_runtime_parallel_amdahl_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_parallel_amdahl_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
  )
  add_test(NAME banana_runtime_parallel_amdahl_test COMMAND banana_runtime_parallel_amdahl_test)

  add_executable(banana_runtime_parallel_contract_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime_parallel_contract_test.c
  )
  target_link_libraries(banana_runtime_parallel_contract_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_parallel_contract_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
  )
  add_test(NAME banana_runtime_parallel_contract_test COMMAND banana_runtime_parallel_contract_test)

  add_executable(banana_runtime_controller_sync_strict_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime_controller_sync_strict_test.c
  )
  target_link_libraries(banana_runtime_controller_sync_strict_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_controller_sync_strict_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
  )
  add_test(NAME banana_runtime_controller_sync_strict_test COMMAND banana_runtime_controller_sync_strict_test)

  add_executable(banana_runtime_controller_sync_map_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime_controller_sync_map_test.c
  )
  target_link_libraries(banana_runtime_controller_sync_map_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_controller_sync_map_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
  )
  add_test(NAME banana_runtime_controller_sync_map_test COMMAND banana_runtime_controller_sync_map_test)

  add_executable(banana_player_sync_staleness_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/player_sync_staleness_test.c
  )
  target_link_libraries(banana_player_sync_staleness_test PRIVATE banana_engine_core)
  target_include_directories(banana_player_sync_staleness_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
  )
  add_test(NAME banana_player_sync_staleness_test COMMAND banana_player_sync_staleness_test)

  add_executable(banana_interactive_objects_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/interactive_objects_test.c
  )
  target_link_libraries(banana_interactive_objects_test PRIVATE banana_engine_core)
  target_include_directories(banana_interactive_objects_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
  )
  add_test(NAME banana_interactive_objects_test COMMAND banana_interactive_objects_test)

  add_executable(banana_npc_merchant_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/npc_merchant_test.c
  )
  target_link_libraries(banana_npc_merchant_test PRIVATE banana_engine_core)
  target_include_directories(banana_npc_merchant_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
  )
  add_test(NAME banana_npc_merchant_test COMMAND banana_npc_merchant_test)

  add_executable(banana_engine_merchant_trade_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/engine_merchant_trade_test.c
  )
  target_link_libraries(banana_engine_merchant_trade_test PRIVATE banana_engine_core)
  target_include_directories(banana_engine_merchant_trade_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
  )
  add_test(NAME banana_engine_merchant_trade_test COMMAND banana_engine_merchant_trade_test)

  add_executable(banana_terrain_features_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/terrain_features_test.c
  )
  target_link_libraries(banana_terrain_features_test PRIVATE banana_engine_core)
  target_include_directories(banana_terrain_features_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
  )
  add_test(NAME banana_terrain_features_test COMMAND banana_terrain_features_test)

  add_executable(banana_ui_system_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/ui_system_test.c
  )
  target_link_libraries(banana_ui_system_test PRIVATE banana_engine_core)
  target_include_directories(banana_ui_system_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/ui
  )
  add_test(NAME banana_ui_system_test COMMAND banana_ui_system_test)

  add_executable(banana_input_binding_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/input_binding_test.c
  )
  target_link_libraries(banana_input_binding_test PRIVATE banana_engine_core)
  target_include_directories(banana_input_binding_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
  )
  add_test(NAME banana_input_binding_test COMMAND banana_input_binding_test)

  add_executable(banana_terrain_chunks_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/terrain_chunks_test.c
  )
  target_link_libraries(banana_terrain_chunks_test PRIVATE banana_engine_core)
  target_include_directories(banana_terrain_chunks_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
  )
  add_test(NAME banana_terrain_chunks_test COMMAND banana_terrain_chunks_test)

  add_executable(banana_player_resources_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/player_resources_test.c
  )
  target_link_libraries(banana_player_resources_test PRIVATE banana_engine_core)
  target_include_directories(banana_player_resources_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
  )
  add_test(NAME banana_player_resources_test COMMAND banana_player_resources_test)

  add_executable(banana_player_builds_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/player_builds_test.c
  )
  target_link_libraries(banana_player_builds_test PRIVATE banana_engine_core)
  target_include_directories(banana_player_builds_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
  )
  add_test(NAME banana_player_builds_test COMMAND banana_player_builds_test)

  add_executable(banana_interactive_collection_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/interactive_collection_test.c
  )
  target_link_libraries(banana_interactive_collection_test PRIVATE banana_engine_core)
  target_include_directories(banana_interactive_collection_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
  )
  add_test(NAME banana_interactive_collection_test COMMAND banana_interactive_collection_test)

  add_executable(banana_interactive_object_spawner_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/interactive_object_spawner_test.c
  )
  target_link_libraries(banana_interactive_object_spawner_test PRIVATE banana_engine_core)
  target_include_directories(banana_interactive_object_spawner_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
  )
  add_test(NAME banana_interactive_object_spawner_test COMMAND banana_interactive_object_spawner_test)

  add_executable(banana_chunk_stream_packet_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/chunk_stream_packet_test.c
  )
  target_link_libraries(banana_chunk_stream_packet_test PRIVATE banana_engine_core)
  target_include_directories(banana_chunk_stream_packet_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
  )
  add_test(NAME banana_chunk_stream_packet_test COMMAND banana_chunk_stream_packet_test)

  add_executable(banana_actor_mesh_registry_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/actor_mesh_registry_test.c
  )
  target_link_libraries(banana_actor_mesh_registry_test PRIVATE banana_engine_core)
  target_include_directories(banana_actor_mesh_registry_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
  )
  add_test(NAME banana_actor_mesh_registry_test COMMAND banana_actor_mesh_registry_test)
endif()
