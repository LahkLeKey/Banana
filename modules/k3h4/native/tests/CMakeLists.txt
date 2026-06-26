cmake_minimum_required(VERSION 3.20)

# Scaffolding only: test targets are created when moved test files exist.
function(banana_k3h4_add_test_if_exists target_name rel_source)
  set(_src "${CMAKE_CURRENT_SOURCE_DIR}/${rel_source}")
  if(NOT EXISTS "${_src}")
    return()
  endif()

  add_executable(${target_name} "${_src}")

  if(TARGET banana_k3h4_core)
    target_link_libraries(${target_name} PRIVATE banana_k3h4_core)
  endif()

  if(TARGET banana_engine_core)
    target_link_libraries(${target_name} PRIVATE banana_engine_core)
  endif()

  if(TARGET banana_native)
    target_link_libraries(${target_name} PRIVATE banana_native)
  endif()

  add_test(NAME ${target_name} COMMAND ${target_name})
endfunction()

banana_k3h4_add_test_if_exists(
  banana_k3h4_metrics_orchestration_layer_test
  "k3h4/k3h4_metrics_orchestration_layer_test.c"
)

banana_k3h4_add_test_if_exists(
  banana_k3h4_dialogue_cluster_router_test
  "k3h4/k3h4_dialogue_cluster_router_test.c"
)

banana_k3h4_add_test_if_exists(
  banana_k3h4_dialogue_template_layer_test
  "k3h4/k3h4_dialogue_template_layer_test.c"
)

banana_k3h4_add_test_if_exists(
  banana_k3h4_dialogue_generative_surface_layer_test
  "k3h4/k3h4_dialogue_generative_surface_layer_test.c"
)

banana_k3h4_add_test_if_exists(
  banana_k3h4_metrics_infrastructure_adapter_test
  "k3h4/infrastructure/k3h4_metrics_infrastructure_adapter_test.c"
)

banana_k3h4_add_test_if_exists(
  banana_runtime_combat_controller_k3h4_test
  "runtime/ai/combat/combat_controller_k3h4_test.c"
)

banana_k3h4_add_test_if_exists(
  banana_runtime_netcode_k3h4_assignment_family_test
  "runtime/netcode/netcode_k3h4_assignment_family_test.c"
)

banana_k3h4_add_test_if_exists(
  banana_runtime_netcode_k3h4_covariance_ellipsoid_test
  "runtime/netcode/netcode_k3h4_covariance_ellipsoid_test.c"
)

banana_k3h4_add_test_if_exists(
  banana_runtime_netcode_k3h4_determinism_test
  "runtime/netcode/netcode_k3h4_determinism_test.c"
)

banana_k3h4_add_test_if_exists(
  banana_runtime_netcode_k3h4_edge_cases_test
  "runtime/netcode/netcode_k3h4_edge_cases_test.c"
)

banana_k3h4_add_test_if_exists(
  banana_k3h4_export_io_failure_test
  "runtime/netcode/netcode_k3h4_export_io_failure_test.c"
)

banana_k3h4_add_test_if_exists(
  banana_k3h4_export_test
  "runtime/netcode/netcode_k3h4_export_test.c"
)

banana_k3h4_add_test_if_exists(
  banana_runtime_netcode_k3h4_orchestrator_failure_paths_test
  "runtime/netcode/netcode_k3h4_orchestrator_failure_paths_test.c"
)

banana_k3h4_add_test_if_exists(
  banana_runtime_netcode_k3h4_orchestrator_test
  "runtime/netcode/netcode_k3h4_orchestrator_test.c"
)

banana_k3h4_add_test_if_exists(
  banana_runtime_netcode_k3h4_pipeline_equivalence_test
  "runtime/netcode/netcode_k3h4_pipeline_equivalence_test.c"
)

banana_k3h4_add_test_if_exists(
  banana_runtime_netcode_k3h4_pipeline_setup_test
  "runtime/netcode/netcode_k3h4_pipeline_setup_test.c"
)

banana_k3h4_add_test_if_exists(
  banana_k3h4_scaling_benchmark_test
  "runtime/netcode/netcode_k3h4_scaling_benchmark_test.c"
)
