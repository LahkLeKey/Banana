cmake_minimum_required(VERSION 3.20)

include(CheckIncludeFile)

if(NOT MSVC)
  # GNU/Clang builds are stricter about NULL visibility; force-include stddef
  # across C translation units to keep cross-platform builds consistent.
  add_compile_options(
    $<$<COMPILE_LANGUAGE:C>:-include>
    $<$<COMPILE_LANGUAGE:C>:stddef.h>
  )
endif()

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
  ai/combat/combat_controller.c
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
  runtime/engine/application/service/application_service_player_port.c
  runtime/engine/application/service/application_service_render_actor_resolver.c
  runtime/engine/application/service/application_service_render_model_cache.c
  runtime/engine/application/service/application_service_render_port.c
  runtime/engine/application/service/application_service_terrain_port.c
  runtime/engine/application/service/application_service_ports.c
  runtime/orchestration/tick/loop/runtime_tick_orchestration.c
  runtime/orchestration/tick/loop/player_tick_orchestration.c
  runtime/orchestration/tick/loop/render_tick_orchestration.c
  runtime/orchestration/tick/loop/terrain_tick_orchestration.c
  runtime/controller/kind/controller_kind_domain.c
  runtime/camera/basis/camera_basis.c
  runtime/camera/follow/camera_follow.c
  runtime/camera/follow/camera_follow_policy.c
  runtime/controller/attach/controller_attach.c
  runtime/controller/runtime/controller_runtime.c
  runtime/controller/sync/controller_sync.c
  runtime/engine/support/engine_aux_abi.c
  runtime/engine/support/engine_aux_launch_gate.c
  runtime/engine/composition/engine_composition.c
  runtime/engine/composition/engine_composition_init.c
  runtime/engine/composition/engine_composition_shutdown.c
  runtime/engine/exports/engine_runtime_context.c
  runtime/engine/exports/engine_exports_core_abi.c
  runtime/engine/exports/engine_exports_ui_economy.c
  runtime/engine/exports/engine_exports_war_metrics.c
  runtime/engine/exports/engine_exports_input_player.c
  runtime/engine/engine_host.c
  runtime/engine/lifecycle/engine_lifecycle_actor_spawn.c
  runtime/engine/lifecycle/engine_lifecycle_launch_gate.c
  runtime/engine/lifecycle/engine_lifecycle_spawn_math.c
  runtime/engine/state/engine_state.c
  runtime/engine/lifecycle/engine_lifecycle.c
  runtime/engine/tick/engine_tick.c
  runtime/engine/gameplay/model/gameplay_model_profile.c
  runtime/engine/gameplay/service/gameplay_service_model_catalog.c
  runtime/engine/gameplay/service/gameplay_service_models.c
  runtime/engine/gameplay/service/gameplay_service_reinforcement_metrics.c
  runtime/engine/gameplay/service/gameplay_service_reinforcement_spawn.c
  runtime/engine/gameplay/service/gameplay_service_sentience_behavior.c
  runtime/engine/gameplay/service/gameplay_service_sentience_decision.c
  runtime/engine/gameplay/service/gameplay_service_sentience_override_config.c
  runtime/engine/gameplay/service/gameplay_service_sentience_overrides.c
  runtime/engine/gameplay/service/gameplay_service_warfront.c
  runtime/engine/gameplay/service/gameplay_service_warfront_expansion.c
  # Feature 005: keep extracted tick/runtime modularization seams grouped here.
  runtime/engine/gameplay/service/gameplay_service.c
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
  runtime/netcode/model/netcode_model.c
  runtime/netcode/reward/netcode_reward.c
  runtime/netcode/link/netcode_link.c
  runtime/netcode/k3h4/netcode_k3h4_metrics.c
  runtime/netcode/k3h4/netcode_k3h4_orchestrator.c
  runtime/netcode/vector/netcode_fixed_point.c
  runtime/netcode/vector/netcode_k3h4.c
  runtime/netcode/vector/netcode_vector.c
  runtime/netcode/k3h4/netcode_k3h4_pipeline.c
  runtime/netcode/k3h4/netcode_k3h4_pipeline_setup.c
  runtime/netcode/k3h4/netcode_k3h4_geometry.c
  runtime/netcode/k3h4/netcode_k3h4_cluster_scoring.c
  runtime/netcode/k3h4/netcode_k3h4_observability.c
  runtime/netcode/k3h4/netcode_k3h4_export.c
  runtime/netcode/contract/netcode_contract.c
  runtime/abi/netcode/netcode_abi.c
  runtime/abi/physics/physics_abi.c
  runtime/player/api/player_api.c
  runtime/player/build/abi/player_build_abi.c
  runtime/player/gateway/player_gateway_abi.c
  runtime/player/build/player_builds.c
  runtime/player/runtime/player_runtime_service.c
  runtime/player/resource/player_resource_abi.c
  runtime/player/sync/player_sync_abi.c
  runtime/player/runtime/player_motion_host.c
  runtime/player/motion/player_motion.c
  runtime/player/registry/player_registry.c
  runtime/player/resource/player_resources.c
  runtime/player/merchant/player_merchant_adapter.c
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

set_target_properties(banana_engine_core PROPERTIES POSITION_INDEPENDENT_CODE ON)

if(WIN32)
  set(BANANA_RENDER_BACKEND_DEFAULT "dx12")
else()
  set(BANANA_RENDER_BACKEND_DEFAULT "glfw")
endif()

set(BANANA_RENDER_BACKEND "${BANANA_RENDER_BACKEND_DEFAULT}" CACHE STRING
  "Native renderer backend: glfw or dx12")
set_property(CACHE BANANA_RENDER_BACKEND PROPERTY STRINGS glfw dx12)

option(BANANA_NETCODE_ENABLE_BLAS "Enable optional BLAS acceleration for netcode k3h4 distance kernels" ON)

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

  if(BANANA_NETCODE_ENABLE_BLAS)
    find_package(BLAS QUIET)
    if(BLAS_FOUND)
      set(CMAKE_REQUIRED_LIBRARIES ${BLAS_LIBRARIES})
      check_include_file(cblas.h BANANA_HAS_CBLAS_HEADER)
      unset(CMAKE_REQUIRED_LIBRARIES)

      if(BANANA_HAS_CBLAS_HEADER)
        target_link_libraries(banana_engine_core PUBLIC BLAS::BLAS)
        target_compile_definitions(banana_engine_core PUBLIC BANANA_ENGINE_HAS_CBLAS=1)
        message(STATUS "[engine] Netcode k3h4 acceleration: CBLAS enabled")
      else()
        message(STATUS "[engine] BLAS found without cblas.h; using deterministic scalar k3h4 backend")
      endif()
    else()
      message(STATUS "[engine] BLAS not found; using deterministic scalar k3h4 backend")
    endif()
  else()
    message(STATUS "[engine] Netcode k3h4 BLAS acceleration disabled by BANANA_NETCODE_ENABLE_BLAS=OFF")
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
  add_executable(banana_native_feedback_loop_factory
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/feedback/native_feedback_loop_factory.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../scaffold/native_entry.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../k3h4/k3h4_native_orchestrator.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../k3h4/k3h4_metrics_orchestration_layer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../k3h4/application/k3h4_metrics_application_service.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../k3h4/infrastructure/k3h4_metrics_infrastructure_adapter.c
  )
  target_link_libraries(banana_native_feedback_loop_factory PRIVATE banana_engine_core)
  target_include_directories(banana_native_feedback_loop_factory PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/render
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_SOURCE_DIR}/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/world
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
  )
  add_test(NAME banana_native_feedback_loop_factory_smoke
    COMMAND banana_native_feedback_loop_factory
      --script ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/feedback/scripts/warfront.dx12play
      --non-interactive
  )
  add_test(NAME banana_native_feedback_loop_factory_dx12_scaffold_smoke
    COMMAND banana_native_feedback_loop_factory
      --script ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/feedback/scripts/dx12-client-scaffold.dx12play
      --non-interactive
  )
  add_test(NAME banana_native_feedback_loop_factory_negotiate_smoke
    COMMAND banana_native_feedback_loop_factory
      --script ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/feedback/scripts/negotiate.dx12play
      --non-interactive
  )
  add_test(NAME banana_native_feedback_loop_factory_comeback_smoke
    COMMAND banana_native_feedback_loop_factory
      --script ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/feedback/scripts/comeback.dx12play
      --non-interactive
  )
  add_test(NAME banana_native_feedback_loop_factory_flank_smoke
    COMMAND banana_native_feedback_loop_factory
      --script ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/feedback/scripts/flank.dx12play
      --non-interactive
  )

  include(${CMAKE_CURRENT_SOURCE_DIR}/../../../tests/native/CMakeLists.txt)
endif()
