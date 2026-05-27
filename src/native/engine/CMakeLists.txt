cmake_minimum_required(VERSION 3.20)

# ─────────────────────────────────────────────────────────────────────────────
# banana_engine_core — static C library (render, physics, ai, world modules)
# ─────────────────────────────────────────────────────────────────────────────

set(ENGINE_RENDER_SOURCES
  render/backend/backend.c
  render/backend/backend_dx12.c
  render/dx12/backend_dx12_diagnostics.c
  render/dx12/backend_dx12_projection_policy.c
  render/window/window.c
  render/shader/shader.c
  render/mesh/mesh.c
  render/mesh/mesh_banana.c
  render/mesh/mesh_terrain.c
  render/material/material.c
  render/camera/camera.c
  render/renderer/renderer.c
  render/renderer/renderer_draw.c
  render/registry/actor_mesh_registry.c
)

set(ENGINE_PHYSICS_SOURCES
  physics/body/body.c
  physics/collider/collider_broad_phase.c
  physics/collider/collider_narrow_phase.c
  physics/collider/collider_resolve.c
  physics/dynamics/dynamics.c
  physics/raycast/raycast.c
  physics/world/world_collision.c
  physics/world/world.c
)

set(ENGINE_AI_SOURCES
  ai/controller/controller.c
  ai/controller/controller_system_dispatch.c
  ai/controller/controller_system_lifecycle.c
  ai/navigation/navigation.c
  ai/npc/npc_merchant.c
  ai/perception/perception.c
  ai/state/state_machine.c
  ai/wildlife/wildlife_controller.c
)

set(ENGINE_WORLD_SOURCES
  world/entity/entity.c
  world/query/world_query.c
  world/core/world.c
  world/signals/signals.c
)

set(ENGINE_UI_SOURCES
  ui/core/ui.c
  ui/render/ui_render.c
  ui/inventory/ui_inventory.c
  ui/merchant/ui_merchant.c
  ui/callbacks/ui_callback_bindings.c
)

set(ENGINE_RUNTIME_SOURCES
  runtime/engine/application_service_ports.c
  runtime/orchestration/tick/runtime_tick_orchestration.c
  runtime/orchestration/tick/player_tick_orchestration.c
  runtime/orchestration/tick/render_tick_orchestration.c
  runtime/orchestration/tick/terrain_tick_orchestration.c
  runtime/controller/kind/controller_kind_domain.c
  runtime/camera/basis/camera_basis.c
  runtime/camera/follow/camera_follow.c
  runtime/camera/follow/camera_follow_policy.c
  runtime/controller/attach/controller_attach.c
  runtime/controller/runtime/controller_runtime.c
  runtime/controller/sync/controller_sync.c
  runtime/engine/engine_aux_abi.c
  runtime/engine/engine_composition.c
  runtime/engine/engine_host.c
  runtime/engine/engine_state.c
  runtime/engine/engine_lifecycle.c
  runtime/engine/engine_tick.c
  # Feature 005: keep extracted tick/runtime modularization seams grouped here.
  runtime/engine/gameplay_service.c
   runtime/input/abi/input_abi.c
   runtime/input/click/input_click_policy.c
   runtime/input/contract/input_contract.c
   runtime/input/binding/input_binding.c
   runtime/merchant/query/merchant_query_abi.c
   runtime/merchant/trade/merchant_trade_abi.c
   runtime/merchant/trade/merchant_trade_domain.c
   runtime/merchant/service/merchant_service.c
   runtime/interaction/objects/interactive_objects.c
  runtime/parallel/interlock/interlock.c
   runtime/input/move_target/move_target_service.c
   runtime/input/move_target/move_target_domain.c
  runtime/parallel/contract/parallel_contract.c
  runtime/parallel/model/parallel_model.c
  runtime/abi/physics/physics_abi.c
  runtime/player/player_api.c
  runtime/player/player_merchant_adapter.c
  runtime/player/player_build_abi.c
  runtime/player/player_gateway_abi.c
  runtime/player/player_builds.c
  runtime/player/player_runtime_service.c
  runtime/player/player_resource_abi.c
  runtime/player/player_sync_abi.c
  runtime/player/player_motion_host.c
  runtime/player/player_motion.c
  runtime/player/player_registry.c
  runtime/player/player_resources.c
   runtime/interaction/collection/interactive_collection.c
   runtime/interaction/spawner/interactive_object_spawner.c
  runtime/chunk/chunk_stream_packet.c
  runtime/render/material/render_material.c
  runtime/render/submit/render_submit.c
  runtime/resource/resource_domain.c
  runtime/terrain/terrain_features.c
  runtime/terrain/terrain_abi.c
  runtime/terrain/terrain_generation.c
  runtime/terrain/static_mesh/terrain_static_mesh_domain.c
  runtime/terrain/static_mesh/terrain_static_mesh_generation.c
  runtime/terrain/static_mesh/terrain_static_mesh_routes.c
  runtime/terrain/terrain_static_mesh.c
  runtime/abi/ui/ui_abi.c
  runtime/terrain/terrain_host.c
  runtime/terrain/terrain_height.c
  runtime/terrain/terrain_mutation.c
  runtime/terrain/terrain_runtime.c
  runtime/terrain/terrain_chunks.c
  runtime/tick/tick_input_phase.c
  runtime/tick/tick_post_phase.c
  runtime/tick/tick_budget_policy.c
  runtime/tick/tick_phases.c
  runtime/world/world_abi.c
  runtime/world/world_metrics.c
  runtime/world/world_telemetry.c
  runtime/wildlife/wildlife_gameplay.c
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

if(WIN32)
  set(BANANA_RENDER_BACKEND_DEFAULT "dx12")
else()
  set(BANANA_RENDER_BACKEND_DEFAULT "glfw")
endif()

set(BANANA_RENDER_BACKEND "${BANANA_RENDER_BACKEND_DEFAULT}" CACHE STRING
  "Native renderer backend: glfw or dx12")
set_property(CACHE BANANA_RENDER_BACKEND PROPERTY STRINGS glfw dx12)

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
# Platform: Native runtime targets
# ─────────────────────────────────────────────────────────────────────────────

if(TRUE)
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

  if(BANANA_RENDER_BACKEND STREQUAL "dx12")
    target_compile_definitions(banana_engine_core PUBLIC BANANA_ENGINE_RENDER_BACKEND_DX12=1)
    if(WIN32)
      target_compile_definitions(banana_engine_core PUBLIC BANANA_ENGINE_HAS_DX12=1)
      target_link_libraries(banana_engine_core PUBLIC
        d3d12
        dxgi
        dxguid
        d3dcompiler
      )
      message(STATUS "[engine] Native rendering backend requested: DX12 (runtime probe enabled)")
    else()
      message(WARNING "[engine] DX12 backend requested on non-Windows host; stub renderer path will be used")
    endif()
  elseif(BANANA_RENDER_BACKEND STREQUAL "glfw")
    target_compile_definitions(banana_engine_core PUBLIC BANANA_ENGINE_RENDER_BACKEND_GLFW=1)
    find_package(OpenGL QUIET)
    find_package(glfw3 QUIET)

    if(OpenGL_FOUND AND glfw3_FOUND)
      target_link_libraries(banana_engine_core PUBLIC
        OpenGL::GL
        glfw
      )
      target_compile_definitions(banana_engine_core PUBLIC BANANA_ENGINE_HAS_GLFW=1)
      message(STATUS "[engine] Native rendering backend active: OpenGL + GLFW")
    else()
      message(WARNING "[engine] OpenGL or GLFW not found — rendering stubs will be used")
      target_compile_definitions(banana_engine_core PUBLIC BANANA_ENGINE_STUB_RENDER=1)
    endif()
  else()
    message(FATAL_ERROR "[engine] Unknown BANANA_RENDER_BACKEND='${BANANA_RENDER_BACKEND}'. Expected 'glfw' or 'dx12'.")
  endif()

  if(UNIX)
    target_link_libraries(banana_engine_core PUBLIC m)
  endif()

  if(WIN32)
    add_executable(banana_engine_win32_dx12_poc
      win32_dx12_poc/app/main.c
      win32_dx12_poc/objective/objective_policy.c
      win32_dx12_poc/overlay/scene_overlay.c
      win32_dx12_poc/scene/continent_asset_registry.c
      win32_dx12_poc/scene/demo_scene_catalog.c
      win32_dx12_poc/scene/scene_flow.c
    )

    target_link_libraries(banana_engine_win32_dx12_poc PRIVATE
      banana_engine_core
    )

    if(WIN32)
      target_link_libraries(banana_engine_win32_dx12_poc PRIVATE msimg32)
    endif()

    target_include_directories(banana_engine_win32_dx12_poc PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}
    )

    if(MSVC)
      target_compile_options(banana_engine_win32_dx12_poc PRIVATE /W3)
      target_compile_definitions(banana_engine_win32_dx12_poc PRIVATE _CRT_SECURE_NO_WARNINGS)
    else()
      target_compile_options(banana_engine_win32_dx12_poc PRIVATE
        -std=c11 -O2 -Wall -Wextra -Wpedantic
      )
    endif()
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
# Coverage instrumentation (mirrors root pattern)
# ─────────────────────────────────────────────────────────────────────────────

if(BANANA_ENABLE_COVERAGE)
  target_compile_options(banana_engine_core PRIVATE -fprofile-arcs -ftest-coverage)
  target_link_options(banana_engine_core PRIVATE -fprofile-arcs -ftest-coverage)
endif()

if(BUILD_TESTING)
  add_executable(banana_engine_serialize_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/engine/engine_serialize_test.c
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
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/terrain/runtime_terrain_mutation_test.c
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
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/terrain/runtime_terrain_generation_test.c
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

  if(BANANA_ENABLE_DETERMINISTIC_PERSISTENCE_TESTS)
    add_executable(banana_runtime_terrain_revisit_runtime_test
      ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/terrain/runtime_terrain_revisit_runtime_test.c
    )
    target_link_libraries(banana_runtime_terrain_revisit_runtime_test PRIVATE banana_engine_core)
    target_include_directories(banana_runtime_terrain_revisit_runtime_test PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_CURRENT_SOURCE_DIR}/render
      ${CMAKE_CURRENT_SOURCE_DIR}/physics
      ${CMAKE_CURRENT_SOURCE_DIR}/ai
      ${CMAKE_CURRENT_SOURCE_DIR}/world
      ${CMAKE_CURRENT_SOURCE_DIR}/runtime
    )
    add_test(NAME banana_runtime_terrain_revisit_runtime_test COMMAND banana_runtime_terrain_revisit_runtime_test)
  endif()

  if(BANANA_ENABLE_DETERMINISTIC_PERSISTENCE_TESTS)
    add_executable(banana_terrain_chunks_identity_partition_test
      ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/terrain/terrain_chunks_identity_partition_test.c
    )
    target_link_libraries(banana_terrain_chunks_identity_partition_test PRIVATE banana_engine_core)
    target_include_directories(banana_terrain_chunks_identity_partition_test PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_CURRENT_SOURCE_DIR}/render
      ${CMAKE_CURRENT_SOURCE_DIR}/physics
      ${CMAKE_CURRENT_SOURCE_DIR}/ai
      ${CMAKE_CURRENT_SOURCE_DIR}/world
      ${CMAKE_CURRENT_SOURCE_DIR}/runtime
    )
    add_test(NAME banana_terrain_chunks_identity_partition_test COMMAND banana_terrain_chunks_identity_partition_test)
  endif()

  if(BANANA_ENABLE_DETERMINISTIC_PERSISTENCE_TESTS)
    add_executable(banana_runtime_terrain_seed_derivation_test
      ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/terrain/runtime_terrain_seed_derivation_test.c
    )
    target_link_libraries(banana_runtime_terrain_seed_derivation_test PRIVATE banana_engine_core)
    target_include_directories(banana_runtime_terrain_seed_derivation_test PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_CURRENT_SOURCE_DIR}/render
      ${CMAKE_CURRENT_SOURCE_DIR}/physics
      ${CMAKE_CURRENT_SOURCE_DIR}/ai
      ${CMAKE_CURRENT_SOURCE_DIR}/world
      ${CMAKE_CURRENT_SOURCE_DIR}/runtime
    )
    add_test(NAME banana_runtime_terrain_seed_derivation_test COMMAND banana_runtime_terrain_seed_derivation_test)

    add_executable(banana_runtime_terrain_generation_revisit_parity_test
      ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/terrain/runtime_terrain_generation_revisit_parity_test.c
    )
    target_link_libraries(banana_runtime_terrain_generation_revisit_parity_test PRIVATE banana_engine_core)
    target_include_directories(banana_runtime_terrain_generation_revisit_parity_test PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_CURRENT_SOURCE_DIR}/render
      ${CMAKE_CURRENT_SOURCE_DIR}/physics
      ${CMAKE_CURRENT_SOURCE_DIR}/ai
      ${CMAKE_CURRENT_SOURCE_DIR}/world
      ${CMAKE_CURRENT_SOURCE_DIR}/runtime
    )
    add_test(NAME banana_runtime_terrain_generation_revisit_parity_test COMMAND banana_runtime_terrain_generation_revisit_parity_test)

    add_executable(banana_terrain_chunk_seam_continuity_test
      ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/terrain/terrain_chunk_seam_continuity_test.c
    )
    target_link_libraries(banana_terrain_chunk_seam_continuity_test PRIVATE banana_engine_core)
    target_include_directories(banana_terrain_chunk_seam_continuity_test PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_CURRENT_SOURCE_DIR}/render
      ${CMAKE_CURRENT_SOURCE_DIR}/physics
      ${CMAKE_CURRENT_SOURCE_DIR}/ai
      ${CMAKE_CURRENT_SOURCE_DIR}/world
      ${CMAKE_CURRENT_SOURCE_DIR}/runtime
    )
    add_test(NAME banana_terrain_chunk_seam_continuity_test COMMAND banana_terrain_chunk_seam_continuity_test)

    add_executable(banana_runtime_terrain_unexplored_parity_test
      ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/terrain/runtime_terrain_unexplored_parity_test.c
    )
    target_link_libraries(banana_runtime_terrain_unexplored_parity_test PRIVATE banana_engine_core)
    target_include_directories(banana_runtime_terrain_unexplored_parity_test PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_CURRENT_SOURCE_DIR}/render
      ${CMAKE_CURRENT_SOURCE_DIR}/physics
      ${CMAKE_CURRENT_SOURCE_DIR}/ai
      ${CMAKE_CURRENT_SOURCE_DIR}/world
      ${CMAKE_CURRENT_SOURCE_DIR}/runtime
    )
    add_test(NAME banana_runtime_terrain_unexplored_parity_test COMMAND banana_runtime_terrain_unexplored_parity_test)

    add_executable(banana_runtime_terrain_retry_determinism_test
      ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/terrain/runtime_terrain_retry_determinism_test.c
    )
    target_link_libraries(banana_runtime_terrain_retry_determinism_test PRIVATE banana_engine_core)
    target_include_directories(banana_runtime_terrain_retry_determinism_test PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_CURRENT_SOURCE_DIR}/render
      ${CMAKE_CURRENT_SOURCE_DIR}/physics
      ${CMAKE_CURRENT_SOURCE_DIR}/ai
      ${CMAKE_CURRENT_SOURCE_DIR}/world
      ${CMAKE_CURRENT_SOURCE_DIR}/runtime
    )
    add_test(NAME banana_runtime_terrain_retry_determinism_test COMMAND banana_runtime_terrain_retry_determinism_test)

    add_executable(banana_runtime_terrain_static_mesh_generation_test
      ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/terrain/runtime_terrain_static_mesh_generation_test.c
    )
    target_link_libraries(banana_runtime_terrain_static_mesh_generation_test PRIVATE banana_engine_core)
    target_include_directories(banana_runtime_terrain_static_mesh_generation_test PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_CURRENT_SOURCE_DIR}/render
      ${CMAKE_CURRENT_SOURCE_DIR}/physics
      ${CMAKE_CURRENT_SOURCE_DIR}/ai
      ${CMAKE_CURRENT_SOURCE_DIR}/world
      ${CMAKE_CURRENT_SOURCE_DIR}/runtime
    )
    add_test(NAME banana_runtime_terrain_static_mesh_generation_test COMMAND banana_runtime_terrain_static_mesh_generation_test)
  endif()

  add_executable(banana_collider_broad_phase_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/physics/collider_broad_phase_test.c
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
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/physics/physics_raycast_test.c
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
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/render/runtime/render_mesh_terrain_test.c
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
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/ai/controller_system_dispatch_test.c
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
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/render/runtime/render_headless_frame_test.c
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

  if(BANANA_ENABLE_DETERMINISTIC_PERSISTENCE_TESTS)
    add_executable(banana_runtime_render_submit_continuity_test
      ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/render/runtime_render_submit_continuity_test.c
    )
    target_link_libraries(banana_runtime_render_submit_continuity_test PRIVATE banana_engine_core)
    target_include_directories(banana_runtime_render_submit_continuity_test PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_CURRENT_SOURCE_DIR}/render
      ${CMAKE_CURRENT_SOURCE_DIR}/physics
      ${CMAKE_CURRENT_SOURCE_DIR}/ai
      ${CMAKE_CURRENT_SOURCE_DIR}/world
      ${CMAKE_CURRENT_SOURCE_DIR}/runtime
    )
    add_test(NAME banana_runtime_render_submit_continuity_test COMMAND banana_runtime_render_submit_continuity_test)
  endif()

  add_executable(banana_world_query_nearby_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/world/world_query_nearby_test.c
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
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/camera/basis/runtime_math_helpers_test.c
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
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/controller/runtime/runtime_controller_world_metrics_test.c
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

  add_executable(banana_runtime_engine_aux_abi_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/engine/runtime_engine_aux_abi_test.c
  )
  target_link_libraries(banana_runtime_engine_aux_abi_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_engine_aux_abi_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
  )
  add_test(NAME banana_runtime_engine_aux_abi_test COMMAND banana_runtime_engine_aux_abi_test)

  add_executable(banana_runtime_launch_gate_matrix_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/engine/launch_gate_matrix_test.c
  )
  target_link_libraries(banana_runtime_launch_gate_matrix_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_launch_gate_matrix_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
  )
  add_test(NAME banana_runtime_launch_gate_matrix_test COMMAND banana_runtime_launch_gate_matrix_test)

  add_executable(banana_runtime_launch_gate_staleness_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/player/player_sync_staleness_test.c
  )
  target_link_libraries(banana_runtime_launch_gate_staleness_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_launch_gate_staleness_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
  )
  add_test(NAME banana_runtime_launch_gate_staleness_test COMMAND banana_runtime_launch_gate_staleness_test)

  add_executable(banana_runtime_launch_gate_mode_policy_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/engine/launch_gate_mode_policy_test.c
  )
  target_link_libraries(banana_runtime_launch_gate_mode_policy_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_launch_gate_mode_policy_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
  )
  add_test(NAME banana_runtime_launch_gate_mode_policy_test COMMAND banana_runtime_launch_gate_mode_policy_test)

  add_executable(banana_runtime_demo_scene_catalog_bootstrap_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/engine/runtime_demo_scene_catalog_bootstrap_test.c
    win32_dx12_poc/scene/continent_asset_registry.c
    win32_dx12_poc/scene/demo_scene_catalog.c
  )
  target_link_libraries(banana_runtime_demo_scene_catalog_bootstrap_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_demo_scene_catalog_bootstrap_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
    ${CMAKE_CURRENT_SOURCE_DIR}/win32_dx12_poc
  )
  add_test(NAME banana_runtime_demo_scene_catalog_bootstrap_test COMMAND banana_runtime_demo_scene_catalog_bootstrap_test)

  add_executable(banana_runtime_demo_scene_catalog_modularity_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/engine/runtime_demo_scene_catalog_modularity_test.c
    win32_dx12_poc/scene/continent_asset_registry.c
    win32_dx12_poc/scene/demo_scene_catalog.c
  )
  target_link_libraries(banana_runtime_demo_scene_catalog_modularity_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_demo_scene_catalog_modularity_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
    ${CMAKE_CURRENT_SOURCE_DIR}/win32_dx12_poc
  )
  add_test(NAME banana_runtime_demo_scene_catalog_modularity_test COMMAND banana_runtime_demo_scene_catalog_modularity_test)

  add_executable(banana_runtime_demo_scene_catalog_gameplay_validation_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/engine/runtime_demo_scene_catalog_gameplay_validation_test.c
    win32_dx12_poc/scene/continent_asset_registry.c
    win32_dx12_poc/scene/demo_scene_catalog.c
  )
  target_link_libraries(banana_runtime_demo_scene_catalog_gameplay_validation_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_demo_scene_catalog_gameplay_validation_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
    ${CMAKE_CURRENT_SOURCE_DIR}/win32_dx12_poc
  )
  add_test(NAME banana_runtime_demo_scene_catalog_gameplay_validation_test COMMAND banana_runtime_demo_scene_catalog_gameplay_validation_test)

  add_executable(banana_runtime_demo_scene_catalog_asset_resolution_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/engine/runtime_demo_scene_catalog_asset_resolution_test.c
    win32_dx12_poc/scene/continent_asset_registry.c
    win32_dx12_poc/scene/demo_scene_catalog.c
  )
  target_link_libraries(banana_runtime_demo_scene_catalog_asset_resolution_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_demo_scene_catalog_asset_resolution_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
    ${CMAKE_CURRENT_SOURCE_DIR}/win32_dx12_poc
  )
  add_test(NAME banana_runtime_demo_scene_catalog_asset_resolution_test COMMAND banana_runtime_demo_scene_catalog_asset_resolution_test)

  add_executable(banana_runtime_demo_scene_catalog_theme_coverage_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/engine/runtime_demo_scene_catalog_theme_coverage_test.c
    win32_dx12_poc/scene/continent_asset_registry.c
    win32_dx12_poc/scene/demo_scene_catalog.c
  )
  target_link_libraries(banana_runtime_demo_scene_catalog_theme_coverage_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_demo_scene_catalog_theme_coverage_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
    ${CMAKE_CURRENT_SOURCE_DIR}/win32_dx12_poc
  )
  add_test(NAME banana_runtime_demo_scene_catalog_theme_coverage_test COMMAND banana_runtime_demo_scene_catalog_theme_coverage_test)

  add_executable(banana_runtime_demo_scene_catalog_theme_determinism_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/engine/runtime_demo_scene_catalog_theme_determinism_test.c
    win32_dx12_poc/scene/continent_asset_registry.c
    win32_dx12_poc/scene/demo_scene_catalog.c
  )
  target_link_libraries(banana_runtime_demo_scene_catalog_theme_determinism_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_demo_scene_catalog_theme_determinism_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
    ${CMAKE_CURRENT_SOURCE_DIR}/win32_dx12_poc
  )
  add_test(NAME banana_runtime_demo_scene_catalog_theme_determinism_test COMMAND banana_runtime_demo_scene_catalog_theme_determinism_test)

  add_executable(banana_runtime_demo_scene_catalog_mutation_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/engine/runtime_demo_scene_catalog_mutation_test.c
    win32_dx12_poc/scene/continent_asset_registry.c
    win32_dx12_poc/scene/demo_scene_catalog.c
  )
  target_link_libraries(banana_runtime_demo_scene_catalog_mutation_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_demo_scene_catalog_mutation_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
    ${CMAKE_CURRENT_SOURCE_DIR}/win32_dx12_poc
  )
  add_test(NAME banana_runtime_demo_scene_catalog_mutation_test COMMAND banana_runtime_demo_scene_catalog_mutation_test)

  add_executable(banana_runtime_demo_scene_catalog_disabled_diagnostics_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/engine/runtime_demo_scene_catalog_disabled_diagnostics_test.c
    win32_dx12_poc/scene/continent_asset_registry.c
    win32_dx12_poc/scene/demo_scene_catalog.c
  )
  target_link_libraries(banana_runtime_demo_scene_catalog_disabled_diagnostics_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_demo_scene_catalog_disabled_diagnostics_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
    ${CMAKE_CURRENT_SOURCE_DIR}/win32_dx12_poc
  )
  add_test(NAME banana_runtime_demo_scene_catalog_disabled_diagnostics_test COMMAND banana_runtime_demo_scene_catalog_disabled_diagnostics_test)

  add_executable(banana_runtime_demo_scene_catalog_coherent_profile_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/engine/runtime_demo_scene_catalog_coherent_profile_test.c
    win32_dx12_poc/scene/continent_asset_registry.c
    win32_dx12_poc/scene/demo_scene_catalog.c
  )
  target_link_libraries(banana_runtime_demo_scene_catalog_coherent_profile_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_demo_scene_catalog_coherent_profile_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
    ${CMAKE_CURRENT_SOURCE_DIR}/win32_dx12_poc
  )
  add_test(NAME banana_runtime_demo_scene_catalog_coherent_profile_test COMMAND banana_runtime_demo_scene_catalog_coherent_profile_test)

  add_executable(banana_runtime_demo_scene_catalog_coherent_launch_signature_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/engine/runtime_demo_scene_catalog_coherent_launch_signature_test.c
    win32_dx12_poc/scene/continent_asset_registry.c
    win32_dx12_poc/scene/demo_scene_catalog.c
  )
  target_link_libraries(banana_runtime_demo_scene_catalog_coherent_launch_signature_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_demo_scene_catalog_coherent_launch_signature_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
    ${CMAKE_CURRENT_SOURCE_DIR}/win32_dx12_poc
  )
  add_test(NAME banana_runtime_demo_scene_catalog_coherent_launch_signature_test COMMAND banana_runtime_demo_scene_catalog_coherent_launch_signature_test)

  add_executable(banana_runtime_demo_scene_catalog_coherent_failure_path_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/engine/runtime_demo_scene_catalog_coherent_failure_path_test.c
    win32_dx12_poc/scene/continent_asset_registry.c
    win32_dx12_poc/scene/demo_scene_catalog.c
  )
  target_link_libraries(banana_runtime_demo_scene_catalog_coherent_failure_path_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_demo_scene_catalog_coherent_failure_path_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
    ${CMAKE_CURRENT_SOURCE_DIR}/win32_dx12_poc
  )
  add_test(NAME banana_runtime_demo_scene_catalog_coherent_failure_path_test COMMAND banana_runtime_demo_scene_catalog_coherent_failure_path_test)

  add_executable(banana_runtime_demo_scene_catalog_transition_continuity_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/engine/runtime_demo_scene_catalog_transition_continuity_test.c
    win32_dx12_poc/scene/continent_asset_registry.c
    win32_dx12_poc/scene/demo_scene_catalog.c
  )
  target_link_libraries(banana_runtime_demo_scene_catalog_transition_continuity_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_demo_scene_catalog_transition_continuity_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
    ${CMAKE_CURRENT_SOURCE_DIR}/win32_dx12_poc
  )
  add_test(NAME banana_runtime_demo_scene_catalog_transition_continuity_test COMMAND banana_runtime_demo_scene_catalog_transition_continuity_test)

  add_executable(banana_runtime_demo_scene_catalog_transition_drift_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/engine/runtime_demo_scene_catalog_transition_drift_test.c
    win32_dx12_poc/scene/continent_asset_registry.c
    win32_dx12_poc/scene/demo_scene_catalog.c
  )
  target_link_libraries(banana_runtime_demo_scene_catalog_transition_drift_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_demo_scene_catalog_transition_drift_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
    ${CMAKE_CURRENT_SOURCE_DIR}/win32_dx12_poc
  )
  add_test(NAME banana_runtime_demo_scene_catalog_transition_drift_test COMMAND banana_runtime_demo_scene_catalog_transition_drift_test)

  add_executable(banana_runtime_demo_scene_catalog_continuity_roundtrip_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/engine/runtime_demo_scene_catalog_continuity_roundtrip_test.c
    win32_dx12_poc/scene/continent_asset_registry.c
    win32_dx12_poc/scene/demo_scene_catalog.c
  )
  target_link_libraries(banana_runtime_demo_scene_catalog_continuity_roundtrip_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_demo_scene_catalog_continuity_roundtrip_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
    ${CMAKE_CURRENT_SOURCE_DIR}/win32_dx12_poc
  )
  add_test(NAME banana_runtime_demo_scene_catalog_continuity_roundtrip_test COMMAND banana_runtime_demo_scene_catalog_continuity_roundtrip_test)

  add_executable(banana_runtime_demo_scene_catalog_coherent_mutation_safety_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/engine/runtime_demo_scene_catalog_coherent_mutation_safety_test.c
    win32_dx12_poc/scene/continent_asset_registry.c
    win32_dx12_poc/scene/demo_scene_catalog.c
  )
  target_link_libraries(banana_runtime_demo_scene_catalog_coherent_mutation_safety_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_demo_scene_catalog_coherent_mutation_safety_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
    ${CMAKE_CURRENT_SOURCE_DIR}/win32_dx12_poc
  )
  add_test(NAME banana_runtime_demo_scene_catalog_coherent_mutation_safety_test COMMAND banana_runtime_demo_scene_catalog_coherent_mutation_safety_test)

  add_executable(banana_runtime_demo_scene_catalog_coherent_disabled_profile_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/engine/runtime_demo_scene_catalog_coherent_disabled_profile_test.c
    win32_dx12_poc/scene/continent_asset_registry.c
    win32_dx12_poc/scene/demo_scene_catalog.c
  )
  target_link_libraries(banana_runtime_demo_scene_catalog_coherent_disabled_profile_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_demo_scene_catalog_coherent_disabled_profile_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
    ${CMAKE_CURRENT_SOURCE_DIR}/win32_dx12_poc
  )
  add_test(NAME banana_runtime_demo_scene_catalog_coherent_disabled_profile_test COMMAND banana_runtime_demo_scene_catalog_coherent_disabled_profile_test)

  add_executable(banana_runtime_telemetry_input_contract_test
      ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/input/contract/runtime_telemetry_input_contract_test.c
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

  add_executable(banana_runtime_input_abi_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/input/runtime_input_abi_test.c
  )
  target_link_libraries(banana_runtime_input_abi_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_input_abi_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
  )
  add_test(NAME banana_runtime_input_abi_test COMMAND banana_runtime_input_abi_test)

  add_executable(banana_runtime_input_click_policy_test
      ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/input/click/runtime_input_click_policy_test.c
  )
  target_link_libraries(banana_runtime_input_click_policy_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_input_click_policy_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
  )
  add_test(NAME banana_runtime_input_click_policy_test COMMAND banana_runtime_input_click_policy_test)

  add_executable(banana_engine_input_contract_transition_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/engine/engine_input_contract_transition_test.c
  )
  target_link_libraries(banana_engine_input_contract_transition_test PRIVATE banana_engine_core)
  target_include_directories(banana_engine_input_contract_transition_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
  )
  add_test(NAME banana_engine_input_contract_transition_test COMMAND banana_engine_input_contract_transition_test)

  add_executable(banana_engine_click_to_move_target_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/engine/engine_click_to_move_target_test.c
  )
  target_link_libraries(banana_engine_click_to_move_target_test PRIVATE banana_engine_core)
  target_include_directories(banana_engine_click_to_move_target_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
  )
  add_test(NAME banana_engine_click_to_move_target_test COMMAND banana_engine_click_to_move_target_test)

  add_executable(banana_engine_dx12_runtime_smoke_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/render/dx12/engine_dx12_runtime_smoke_test.c
  )
  target_link_libraries(banana_engine_dx12_runtime_smoke_test PRIVATE banana_engine_core)
  target_include_directories(banana_engine_dx12_runtime_smoke_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
  )
  add_test(NAME banana_engine_dx12_runtime_smoke_test COMMAND banana_engine_dx12_runtime_smoke_test)

  if(WIN32)
    add_test(NAME banana_engine_win32_dx12_poc_startup_smoke COMMAND banana_engine_win32_dx12_poc)
    set_tests_properties(banana_engine_win32_dx12_poc_startup_smoke PROPERTIES
      ENVIRONMENT "BANANA_DX12_POC_AUTOTEST=1;BANANA_DX12_POC_AUTOTEST_SECONDS=2"
      TIMEOUT 10
    )

    add_executable(banana_dx12_scene_overlay_frame_test
      ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/render/dx12/dx12_scene_overlay_frame_test.c
      win32_dx12_poc/overlay/scene_overlay.c
      win32_dx12_poc/scene/demo_scene_catalog.c
      win32_dx12_poc/scene/continent_asset_registry.c
    )
    target_link_libraries(banana_dx12_scene_overlay_frame_test PRIVATE banana_engine_core)
    target_include_directories(banana_dx12_scene_overlay_frame_test PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_CURRENT_SOURCE_DIR}/render
      ${CMAKE_CURRENT_SOURCE_DIR}/physics
      ${CMAKE_CURRENT_SOURCE_DIR}/ai
      ${CMAKE_CURRENT_SOURCE_DIR}/world
      ${CMAKE_CURRENT_SOURCE_DIR}/runtime
      ${CMAKE_CURRENT_SOURCE_DIR}/win32_dx12_poc
    )
    add_test(NAME banana_dx12_scene_overlay_frame_test COMMAND banana_dx12_scene_overlay_frame_test)

    add_executable(banana_dx12_projection_policy_test
      ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/render/dx12/dx12_projection_policy_test.c
    )
    target_link_libraries(banana_dx12_projection_policy_test PRIVATE banana_engine_core)
    target_include_directories(banana_dx12_projection_policy_test PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_CURRENT_SOURCE_DIR}/render
      ${CMAKE_CURRENT_SOURCE_DIR}/physics
      ${CMAKE_CURRENT_SOURCE_DIR}/ai
      ${CMAKE_CURRENT_SOURCE_DIR}/world
    )
    add_test(NAME banana_dx12_projection_policy_test COMMAND banana_dx12_projection_policy_test)

    add_executable(banana_dx12_objective_policy_test
      ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/render/dx12/dx12_objective_policy_test.c
      win32_dx12_poc/objective/objective_policy.c
    )
    target_include_directories(banana_dx12_objective_policy_test PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_CURRENT_SOURCE_DIR}/win32_dx12_poc
    )
    add_test(NAME banana_dx12_objective_policy_test COMMAND banana_dx12_objective_policy_test)

    add_executable(banana_dx12_scene_flow_test
      ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/render/dx12/dx12_scene_flow_test.c
      win32_dx12_poc/scene/scene_flow.c
      win32_dx12_poc/scene/demo_scene_catalog.c
      win32_dx12_poc/scene/continent_asset_registry.c
    )
    target_link_libraries(banana_dx12_scene_flow_test PRIVATE banana_engine_core)
    target_include_directories(banana_dx12_scene_flow_test PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_CURRENT_SOURCE_DIR}/win32_dx12_poc
    )
    add_test(NAME banana_dx12_scene_flow_test COMMAND banana_dx12_scene_flow_test)

    add_executable(banana_dx12_gameplay_lane_test
      ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/render/dx12/dx12_gameplay_lane_test.c
      win32_dx12_poc/scene/scene_flow.c
      win32_dx12_poc/scene/demo_scene_catalog.c
      win32_dx12_poc/scene/continent_asset_registry.c
    )
    target_link_libraries(banana_dx12_gameplay_lane_test PRIVATE banana_engine_core)
    target_include_directories(banana_dx12_gameplay_lane_test PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_CURRENT_SOURCE_DIR}/win32_dx12_poc
    )
    add_test(NAME banana_dx12_gameplay_lane_test COMMAND banana_dx12_gameplay_lane_test)

    add_executable(banana_dx12_gameplay_click_lane_test
      ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/render/dx12/dx12_gameplay_click_lane_test.c
      win32_dx12_poc/scene/scene_flow.c
      win32_dx12_poc/scene/demo_scene_catalog.c
      win32_dx12_poc/scene/continent_asset_registry.c
    )
    target_link_libraries(banana_dx12_gameplay_click_lane_test PRIVATE banana_engine_core)
    target_include_directories(banana_dx12_gameplay_click_lane_test PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_CURRENT_SOURCE_DIR}/win32_dx12_poc
    )
    add_test(NAME banana_dx12_gameplay_click_lane_test COMMAND banana_dx12_gameplay_click_lane_test)
  endif()

  add_executable(banana_runtime_physics_world_abi_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/abi/physics/runtime_physics_world_abi_test.c
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
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/engine/runtime_engine_lifecycle_test.c
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
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/controller/attach/runtime_controller_attach_test.c
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
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/tick/runtime_engine_tick_test.c
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

  add_executable(banana_runtime_tick_budget_policy_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/tick/runtime_tick_budget_policy_test.c
  )
  target_link_libraries(banana_runtime_tick_budget_policy_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_tick_budget_policy_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
  )
  add_test(NAME banana_runtime_tick_budget_policy_test COMMAND banana_runtime_tick_budget_policy_test)

  add_executable(banana_runtime_tick_input_phase_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/tick/runtime_tick_input_phase_test.c
  )
  target_link_libraries(banana_runtime_tick_input_phase_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_tick_input_phase_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
  )
  add_test(NAME banana_runtime_tick_input_phase_test COMMAND banana_runtime_tick_input_phase_test)

  add_executable(banana_runtime_tick_post_phase_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/tick/runtime_tick_post_phase_test.c
  )
  target_link_libraries(banana_runtime_tick_post_phase_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_tick_post_phase_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
  )
  add_test(NAME banana_runtime_tick_post_phase_test COMMAND banana_runtime_tick_post_phase_test)

  add_executable(banana_runtime_camera_follow_policy_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/camera/follow/runtime_camera_follow_policy_test.c
  )
  target_link_libraries(banana_runtime_camera_follow_policy_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_camera_follow_policy_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
  )
  add_test(NAME banana_runtime_camera_follow_policy_test COMMAND banana_runtime_camera_follow_policy_test)

  add_executable(banana_runtime_move_target_domain_test
      ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/input/move_target/runtime_move_target_domain_test.c
  )
  target_link_libraries(banana_runtime_move_target_domain_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_move_target_domain_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
  )
  add_test(NAME banana_runtime_move_target_domain_test COMMAND banana_runtime_move_target_domain_test)

  add_executable(banana_runtime_terrain_host_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/terrain/runtime_terrain_host_test.c
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

  add_executable(banana_runtime_terrain_abi_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/terrain/runtime_terrain_abi_test.c
  )
  target_link_libraries(banana_runtime_terrain_abi_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_terrain_abi_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
  )
  add_test(NAME banana_runtime_terrain_abi_test COMMAND banana_runtime_terrain_abi_test)

  add_executable(banana_runtime_player_api_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/player/runtime_player_api_test.c
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

  add_executable(banana_runtime_player_gateway_abi_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/player/runtime_player_gateway_abi_test.c
  )
  target_link_libraries(banana_runtime_player_gateway_abi_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_player_gateway_abi_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
  )
  add_test(NAME banana_runtime_player_gateway_abi_test COMMAND banana_runtime_player_gateway_abi_test)

  add_executable(banana_runtime_player_merchant_adapter_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/player/runtime_player_merchant_adapter_test.c
  )
  target_link_libraries(banana_runtime_player_merchant_adapter_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_player_merchant_adapter_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
  )
  add_test(NAME banana_runtime_player_merchant_adapter_test COMMAND banana_runtime_player_merchant_adapter_test)

  add_executable(banana_runtime_player_builds_integration_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/player/runtime_player_builds_integration_test.c
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

  add_executable(banana_runtime_player_build_abi_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/player/runtime_player_build_abi_test.c
  )
  target_link_libraries(banana_runtime_player_build_abi_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_player_build_abi_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
  )
  add_test(NAME banana_runtime_player_build_abi_test COMMAND banana_runtime_player_build_abi_test)

  add_executable(banana_runtime_render_material_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/render/runtime_render_material_test.c
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
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/player/runtime_player_motion_host_test.c
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
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/engine/runtime_engine_host_test.c
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

  add_executable(banana_runtime_engine_state_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/engine/runtime_engine_state_test.c
  )
  target_link_libraries(banana_runtime_engine_state_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_engine_state_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
  )
  add_test(NAME banana_runtime_engine_state_test COMMAND banana_runtime_engine_state_test)

  add_executable(banana_runtime_controller_kind_domain_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/controller/kind/runtime_controller_kind_domain_test.c
  )
  target_link_libraries(banana_runtime_controller_kind_domain_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_controller_kind_domain_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
  )
  add_test(NAME banana_runtime_controller_kind_domain_test COMMAND banana_runtime_controller_kind_domain_test)

  add_executable(banana_runtime_application_service_ports_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/engine/runtime_application_service_ports_test.c
  )
  target_link_libraries(banana_runtime_application_service_ports_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_application_service_ports_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
  )
  add_test(NAME banana_runtime_application_service_ports_test COMMAND banana_runtime_application_service_ports_test)

  add_executable(banana_runtime_interlock_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/parallel/interlock/runtime_interlock_test.c
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
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/parallel/model/runtime_parallel_amdahl_test.c
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
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/parallel/contract/runtime_parallel_contract_test.c
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
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/controller/sync/runtime_controller_sync_strict_test.c
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
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/controller/sync/runtime_controller_sync_map_test.c
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
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/player/player_sync_staleness_test.c
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

  add_executable(banana_runtime_player_sync_abi_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/player/runtime_player_sync_abi_test.c
  )
  target_link_libraries(banana_runtime_player_sync_abi_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_player_sync_abi_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
  )
  add_test(NAME banana_runtime_player_sync_abi_test COMMAND banana_runtime_player_sync_abi_test)

  add_executable(banana_interactive_objects_test
      ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/interaction/objects/interactive_objects_test.c
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
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/merchant/npc_merchant_test.c
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
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/engine/engine_merchant_trade_test.c
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

  add_executable(banana_engine_typed_abi_validation_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/engine/engine_typed_abi_validation_test.c
  )
  target_link_libraries(banana_engine_typed_abi_validation_test PRIVATE banana_engine_core)
  target_include_directories(banana_engine_typed_abi_validation_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
  )
  add_test(NAME banana_engine_typed_abi_validation_test COMMAND banana_engine_typed_abi_validation_test)

  add_executable(banana_terrain_features_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/terrain/terrain_features_test.c
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
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/ui/ui_system_test.c
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

  add_executable(banana_runtime_ui_abi_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/abi/ui/runtime_ui_abi_test.c
  )
  target_link_libraries(banana_runtime_ui_abi_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_ui_abi_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
  )
  add_test(NAME banana_runtime_ui_abi_test COMMAND banana_runtime_ui_abi_test)

  add_executable(banana_native_ui_abi_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/ui/native_ui_abi_test.c
  )
  target_include_directories(banana_native_ui_abi_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
  )
  if(MSVC)
    target_compile_options(banana_native_ui_abi_test PRIVATE /W3)
  else()
    target_compile_options(banana_native_ui_abi_test PRIVATE
      -std=c11 -O2 -Wall -Wextra -Wpedantic
    )
  endif()
  add_test(NAME banana_native_ui_abi_test COMMAND banana_native_ui_abi_test)

  add_executable(banana_input_binding_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/input/input_binding_test.c
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
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/terrain/terrain_chunks_test.c
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
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/player/player_resources_test.c
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

  add_executable(banana_runtime_domain_adapters_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/contracts/runtime_domain_adapters_test.c
  )
  target_link_libraries(banana_runtime_domain_adapters_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_domain_adapters_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
  )
  add_test(NAME banana_runtime_domain_adapters_test COMMAND banana_runtime_domain_adapters_test)

  add_executable(banana_runtime_compile_guard_abi_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/contracts/runtime_compile_guard_abi_test.c
  )
  target_link_libraries(banana_runtime_compile_guard_abi_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_compile_guard_abi_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
  )
  add_test(NAME banana_runtime_compile_guard_abi_test COMMAND banana_runtime_compile_guard_abi_test)

  add_executable(banana_runtime_architecture_dependency_guard_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/architecture/runtime_architecture_dependency_guard_test.c
  )
  target_include_directories(banana_runtime_architecture_dependency_guard_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
  )
  target_compile_definitions(banana_runtime_architecture_dependency_guard_test PRIVATE
    BANANA_ENGINE_SOURCE_ROOT="${CMAKE_CURRENT_SOURCE_DIR}"
  )
  add_test(NAME banana_runtime_architecture_dependency_guard_test COMMAND banana_runtime_architecture_dependency_guard_test)

  add_executable(banana_runtime_merchant_service_test
      ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/merchant/service/runtime_merchant_service_test.c
  )
  target_link_libraries(banana_runtime_merchant_service_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_merchant_service_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
  )
  add_test(NAME banana_runtime_merchant_service_test COMMAND banana_runtime_merchant_service_test)

  add_executable(banana_runtime_merchant_query_abi_test
      ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/merchant/query/runtime_merchant_query_abi_test.c
  )
  target_link_libraries(banana_runtime_merchant_query_abi_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_merchant_query_abi_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
  )
  add_test(NAME banana_runtime_merchant_query_abi_test COMMAND banana_runtime_merchant_query_abi_test)

  add_executable(banana_runtime_merchant_trade_abi_test
      ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/merchant/trade/runtime_merchant_trade_abi_test.c
  )
  target_link_libraries(banana_runtime_merchant_trade_abi_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_merchant_trade_abi_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
  )
  add_test(NAME banana_runtime_merchant_trade_abi_test COMMAND banana_runtime_merchant_trade_abi_test)

  add_executable(banana_runtime_player_resource_abi_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/player/runtime_player_resource_abi_test.c
  )
  target_link_libraries(banana_runtime_player_resource_abi_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_player_resource_abi_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
  )
  add_test(NAME banana_runtime_player_resource_abi_test COMMAND banana_runtime_player_resource_abi_test)

  add_executable(banana_runtime_gameplay_service_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/engine/runtime_gameplay_service_test.c
  )
  target_link_libraries(banana_runtime_gameplay_service_test PRIVATE banana_engine_core)
  target_include_directories(banana_runtime_gameplay_service_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
  )
  add_test(NAME banana_runtime_gameplay_service_test COMMAND banana_runtime_gameplay_service_test)

  add_executable(banana_player_builds_test
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/player/player_builds_test.c
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
      ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/interaction/collection/interactive_collection_test.c
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
      ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/interaction/spawner/interactive_object_spawner_test.c
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
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/runtime/chunk/chunk_stream_packet_test.c
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
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/render/registry/actor_mesh_registry_test.c
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
