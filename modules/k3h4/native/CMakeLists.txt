cmake_minimum_required(VERSION 3.20)

if(TARGET banana_k3h4_core)
  return()
endif()

add_library(banana_k3h4_core STATIC)
set_target_properties(banana_k3h4_core PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(banana_k3h4_core
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/native/engine
)

# Only add files that exist so this scaffold can land before move phase.
function(banana_k3h4_add_source_if_exists relpath)
  set(_abs "${CMAKE_CURRENT_SOURCE_DIR}/${relpath}")
  if(EXISTS "${_abs}")
    target_sources(banana_k3h4_core PRIVATE "${_abs}")
  endif()
endfunction()

# Native K3H4 core
banana_k3h4_add_source_if_exists("src/k3h4/k3h4_native_orchestrator.c")
banana_k3h4_add_source_if_exists("src/k3h4/k3h4_metrics_orchestration_layer.c")
banana_k3h4_add_source_if_exists("src/k3h4/k3h4_dialogue_cluster_router.c")
banana_k3h4_add_source_if_exists("src/k3h4/k3h4_dialogue_template_layer.c")
banana_k3h4_add_source_if_exists("src/k3h4/application/k3h4_metrics_application_service.c")
banana_k3h4_add_source_if_exists("src/k3h4/infrastructure/k3h4_metrics_infrastructure_adapter.c")

# Runtime netcode K3H4
banana_k3h4_add_source_if_exists("src/runtime/netcode/k3h4/netcode_k3h4_metrics.c")
banana_k3h4_add_source_if_exists("src/runtime/netcode/k3h4/netcode_k3h4_orchestrator.c")
banana_k3h4_add_source_if_exists("src/runtime/netcode/k3h4/netcode_k3h4_pipeline.c")
banana_k3h4_add_source_if_exists("src/runtime/netcode/k3h4/netcode_k3h4_pipeline_setup.c")
banana_k3h4_add_source_if_exists("src/runtime/netcode/k3h4/netcode_k3h4_geometry.c")
banana_k3h4_add_source_if_exists("src/runtime/netcode/k3h4/netcode_k3h4_cluster_scoring.c")
banana_k3h4_add_source_if_exists("src/runtime/netcode/k3h4/netcode_k3h4_observability.c")
banana_k3h4_add_source_if_exists("src/runtime/netcode/k3h4/netcode_k3h4_export.c")

# Runtime vector bridge
banana_k3h4_add_source_if_exists("src/runtime/netcode/vector/netcode_k3h4.c")

# Keep fixed-point math with the extracted k3h4 core so static-link order on
# Linux always resolves runtime_netcode_q16_* symbols for k3h4 call sites.
set(_banana_k3h4_fixed_point_src
  "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/native/engine/runtime/netcode/vector/netcode_fixed_point.c")
if(EXISTS "${_banana_k3h4_fixed_point_src}")
  target_sources(banana_k3h4_core PRIVATE "${_banana_k3h4_fixed_point_src}")
endif()

# Test targets are currently defined in tests/native/CMakeLists.txt.
# Keep this module focused on core library wiring during migration.
