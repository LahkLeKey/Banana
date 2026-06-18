# Native test extensions are wired by engine CMake includes.

set(BANANA_ENGINE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../src/native/engine")

option(BANANA_ENABLE_CMOCKA_TESTS "Enable CMocka-backed native tests" OFF)

if(BANANA_ENABLE_CMOCKA_TESTS)
	find_package(cmocka QUIET)
	if(NOT cmocka_FOUND)
		message(WARNING "BANANA_ENABLE_CMOCKA_TESTS is ON, but cmocka was not found. CMocka tests will be skipped.")
	endif()
endif()

function(banana_enable_cmocka_legacy_wrapper target wrapper_kind)
	if(NOT BANANA_ENABLE_CMOCKA_TESTS OR NOT cmocka_FOUND)
		return()
	endif()

	if(wrapper_kind STREQUAL "void")
		target_compile_definitions(${target} PRIVATE main=banana_legacy_main_void)
		target_sources(${target} PRIVATE
			${CMAKE_CURRENT_LIST_DIR}/runtime/support/cmocka/legacy_main_void_adapter.c
		)
	elseif(wrapper_kind STREQUAL "args")
		target_compile_definitions(${target} PRIVATE main=banana_legacy_main_args)
		target_sources(${target} PRIVATE
			${CMAKE_CURRENT_LIST_DIR}/runtime/support/cmocka/legacy_main_args_adapter.c
		)
	else()
		message(FATAL_ERROR "Unknown CMocka wrapper kind '${wrapper_kind}' for target '${target}'")
	endif()

	target_link_libraries(${target} PRIVATE cmocka::cmocka)
endfunction()

function(banana_enable_cmocka_native target)
	if(NOT BANANA_ENABLE_CMOCKA_TESTS OR NOT cmocka_FOUND)
		return()
	endif()

	target_compile_definitions(${target} PRIVATE BANANA_USE_CMOCKA=1)
	target_link_libraries(${target} PRIVATE cmocka::cmocka)
endfunction()

add_executable(banana_runtime_netcode_model_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/netcode/netcode_model_test.c
)

target_link_libraries(banana_runtime_netcode_model_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_netcode_model_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_test(NAME banana_runtime_netcode_model_test
	COMMAND banana_runtime_netcode_model_test
)

add_executable(banana_runtime_netcode_k3h4_determinism_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/netcode/netcode_k3h4_determinism_test.c
)

target_link_libraries(banana_runtime_netcode_k3h4_determinism_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_netcode_k3h4_determinism_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_test(NAME banana_runtime_netcode_k3h4_determinism_test
	COMMAND banana_runtime_netcode_k3h4_determinism_test
)

add_executable(banana_runtime_netcode_k3h4_edge_cases_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/netcode/netcode_k3h4_edge_cases_test.c
)

target_link_libraries(banana_runtime_netcode_k3h4_edge_cases_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_netcode_k3h4_edge_cases_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_test(NAME banana_runtime_netcode_k3h4_edge_cases_test
	COMMAND banana_runtime_netcode_k3h4_edge_cases_test
)

add_executable(banana_runtime_netcode_k3h4_assignment_family_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/netcode/netcode_k3h4_assignment_family_test.c
)

target_link_libraries(banana_runtime_netcode_k3h4_assignment_family_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_netcode_k3h4_assignment_family_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_test(NAME banana_runtime_netcode_k3h4_assignment_family_test
	COMMAND banana_runtime_netcode_k3h4_assignment_family_test
)

add_executable(banana_runtime_netcode_k3h4_orchestrator_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/netcode/netcode_k3h4_orchestrator_test.c
)

target_link_libraries(banana_runtime_netcode_k3h4_orchestrator_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_netcode_k3h4_orchestrator_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_test(NAME banana_runtime_netcode_k3h4_orchestrator_test
	COMMAND banana_runtime_netcode_k3h4_orchestrator_test
)

add_executable(banana_runtime_netcode_k3h4_covariance_ellipsoid_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/netcode/netcode_k3h4_covariance_ellipsoid_test.c
)

target_link_libraries(banana_runtime_netcode_k3h4_covariance_ellipsoid_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_netcode_k3h4_covariance_ellipsoid_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_test(NAME banana_runtime_netcode_k3h4_covariance_ellipsoid_test
	COMMAND banana_runtime_netcode_k3h4_covariance_ellipsoid_test
)

add_executable(banana_runtime_netcode_k3h4_pipeline_setup_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/netcode/netcode_k3h4_pipeline_setup_test.c
)

target_link_libraries(banana_runtime_netcode_k3h4_pipeline_setup_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_netcode_k3h4_pipeline_setup_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_test(NAME banana_runtime_netcode_k3h4_pipeline_setup_test
	COMMAND banana_runtime_netcode_k3h4_pipeline_setup_test
)

add_executable(banana_runtime_netcode_k3h4_pipeline_equivalence_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/netcode/netcode_k3h4_pipeline_equivalence_test.c
)

target_link_libraries(banana_runtime_netcode_k3h4_pipeline_equivalence_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_netcode_k3h4_pipeline_equivalence_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_test(NAME banana_runtime_netcode_k3h4_pipeline_equivalence_test
	COMMAND banana_runtime_netcode_k3h4_pipeline_equivalence_test
)

add_executable(banana_runtime_netcode_abi_envelope_endianness_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/netcode/netcode_abi_envelope_endianness_test.c
)

target_link_libraries(banana_runtime_netcode_abi_envelope_endianness_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_netcode_abi_envelope_endianness_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_test(NAME banana_runtime_netcode_abi_envelope_endianness_test
	COMMAND banana_runtime_netcode_abi_envelope_endianness_test
)

add_executable(banana_runtime_netcode_abi_error_contract_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/netcode/netcode_abi_error_contract_test.c
)

add_executable(banana_runtime_netcode_abi_combat_integration_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/netcode/netcode_abi_combat_integration_test.c
)

target_link_libraries(banana_runtime_netcode_abi_combat_integration_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_netcode_abi_combat_integration_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_test(NAME banana_runtime_netcode_abi_combat_integration_test
	COMMAND banana_runtime_netcode_abi_combat_integration_test
)

add_executable(banana_runtime_world_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/world/core/world_test.c
)

add_test(NAME banana_runtime_world_test
	COMMAND banana_runtime_world_test
)

target_link_libraries(banana_runtime_world_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_world_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_runtime_world_query_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/world/query/world_query_test.c
)

add_test(NAME banana_runtime_world_query_test
	COMMAND banana_runtime_world_query_test
)

target_link_libraries(banana_runtime_world_query_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_world_query_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_runtime_ui_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/ui/core/ui_test.c
)

add_test(NAME banana_runtime_ui_test
	COMMAND banana_runtime_ui_test
)

target_link_libraries(banana_runtime_ui_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_ui_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_runtime_physics_body_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/physics/body/body_test.c
)

add_test(NAME banana_runtime_physics_body_test
	COMMAND banana_runtime_physics_body_test
)

target_link_libraries(banana_runtime_physics_body_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_physics_body_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_runtime_physics_collider_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/physics/collider/collider_test.c
)

add_test(NAME banana_runtime_physics_collider_test
	COMMAND banana_runtime_physics_collider_test
)

target_link_libraries(banana_runtime_physics_collider_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_physics_collider_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_runtime_physics_world_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/physics/world/world_test.c
)

add_test(NAME banana_runtime_physics_world_test
	COMMAND banana_runtime_physics_world_test
)

target_link_libraries(banana_runtime_physics_world_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_physics_world_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_runtime_controller_runtime_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/runtime/controller/controller_runtime_test.c
)

add_test(NAME banana_runtime_controller_runtime_test
	COMMAND banana_runtime_controller_runtime_test
)

target_link_libraries(banana_runtime_controller_runtime_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_controller_runtime_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

if(BANANA_ENABLE_CMOCKA_TESTS AND cmocka_FOUND)
	add_executable(banana_runtime_controller_runtime_cmocka_test
		${CMAKE_CURRENT_LIST_DIR}/runtime/runtime/controller/controller_runtime_cmocka_test.c
	)

	add_test(NAME banana_runtime_controller_runtime_cmocka_test
		COMMAND banana_runtime_controller_runtime_cmocka_test
	)

	target_link_libraries(banana_runtime_controller_runtime_cmocka_test PRIVATE
		banana_engine_core
		cmocka::cmocka
	)

	target_include_directories(banana_runtime_controller_runtime_cmocka_test PRIVATE
		${BANANA_ENGINE_DIR}
		${BANANA_ENGINE_DIR}/runtime
	)
endif()

add_executable(banana_runtime_input_contract_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/runtime/input/contract/input_contract_test.c
)

add_test(NAME banana_runtime_input_contract_test
	COMMAND banana_runtime_input_contract_test
)

target_link_libraries(banana_runtime_input_contract_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_input_contract_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_runtime_input_click_policy_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/runtime/input/click/input_click_policy_test.c
)

add_test(NAME banana_runtime_input_click_policy_test
	COMMAND banana_runtime_input_click_policy_test
)

target_link_libraries(banana_runtime_input_click_policy_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_input_click_policy_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_runtime_render_camera_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/render/camera/camera_test.c
)

add_test(NAME banana_runtime_render_camera_test
	COMMAND banana_runtime_render_camera_test
)

target_link_libraries(banana_runtime_render_camera_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_render_camera_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_runtime_navigation_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/ai/navigation/navigation_test.c
)

add_test(NAME banana_runtime_navigation_test
	COMMAND banana_runtime_navigation_test
)

target_link_libraries(banana_runtime_navigation_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_navigation_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_runtime_wildlife_controller_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/ai/wildlife/wildlife_controller_test.c
)

add_test(NAME banana_runtime_wildlife_controller_test
	COMMAND banana_runtime_wildlife_controller_test
)

target_link_libraries(banana_runtime_wildlife_controller_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_wildlife_controller_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_runtime_npc_merchant_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/ai/npc/npc_merchant_test.c
)

add_test(NAME banana_runtime_npc_merchant_test
	COMMAND banana_runtime_npc_merchant_test
)

target_link_libraries(banana_runtime_npc_merchant_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_npc_merchant_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_runtime_perception_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/ai/perception/perception_test.c
)

add_test(NAME banana_runtime_perception_test
	COMMAND banana_runtime_perception_test
)

target_link_libraries(banana_runtime_perception_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_perception_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_runtime_controller_system_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/ai/controller/controller_system_test.c
)

add_test(NAME banana_runtime_controller_system_test
	COMMAND banana_runtime_controller_system_test
)

target_link_libraries(banana_runtime_controller_system_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_controller_system_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_runtime_state_machine_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/ai/state/state_machine_test.c
)

add_test(NAME banana_runtime_state_machine_test
	COMMAND banana_runtime_state_machine_test
)

target_link_libraries(banana_runtime_state_machine_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_state_machine_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_runtime_ai_controller_team_navigation_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/ai/controller/controller_team_navigation_test.c
)

add_test(NAME banana_runtime_ai_controller_team_navigation_test
	COMMAND banana_runtime_ai_controller_team_navigation_test
)

target_link_libraries(banana_runtime_ai_controller_team_navigation_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_ai_controller_team_navigation_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_runtime_combat_controller_k3h4_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/ai/combat/combat_controller_k3h4_test.c
)

# Build the dedicated combat-controller coverage test binary.
# This target isolates the gameplay mutation and fail-closed safety checks from
# the older smoke test and keeps the combat path easier to diagnose in CI.
add_executable(banana_runtime_combat_controller_coverage_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/ai/combat/combat_controller_coverage_test.c
)

# Register the new binary in ctest so it runs as part of the native suite.
# The name matches the combat-controller scenario we want to track in reports.
add_test(NAME banana_runtime_combat_controller_coverage_test
	COMMAND banana_runtime_combat_controller_coverage_test
)

target_link_libraries(banana_runtime_combat_controller_k3h4_test PRIVATE banana_engine_core)

target_link_libraries(banana_runtime_combat_controller_coverage_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_combat_controller_k3h4_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

target_include_directories(banana_runtime_combat_controller_coverage_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_test(NAME banana_runtime_combat_controller_k3h4_test
	COMMAND banana_runtime_combat_controller_k3h4_test
)

target_link_libraries(banana_runtime_netcode_abi_error_contract_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_netcode_abi_error_contract_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_test(NAME banana_runtime_netcode_abi_error_contract_test
	COMMAND banana_runtime_netcode_abi_error_contract_test
)
# ---------------------------------------------------------------------------
# 036-k3h4-transformer-viz: scaling benchmark
# ---------------------------------------------------------------------------

add_executable(banana_k3h4_scaling_benchmark_test
        ${CMAKE_CURRENT_LIST_DIR}/runtime/netcode/netcode_k3h4_scaling_benchmark_test.c
)

target_link_libraries(banana_k3h4_scaling_benchmark_test PRIVATE banana_engine_core)

target_include_directories(banana_k3h4_scaling_benchmark_test PRIVATE
        ${BANANA_ENGINE_DIR}
        ${BANANA_ENGINE_DIR}/runtime
)

if(MSVC)
    target_compile_options(banana_k3h4_scaling_benchmark_test PRIVATE /W3)
    target_compile_definitions(banana_k3h4_scaling_benchmark_test PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
    target_compile_options(banana_k3h4_scaling_benchmark_test PRIVATE -Wall -Wextra)
    target_link_libraries(banana_k3h4_scaling_benchmark_test PRIVATE m)
endif()

add_test(NAME banana_k3h4_scaling_benchmark_test
        COMMAND banana_k3h4_scaling_benchmark_test
                "${CMAKE_CURRENT_LIST_DIR}/../../artifacts/native/k3h4-scaling-benchmark.json"
        WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../.."
)

# 036-k3h4-transformer-viz: training artifact export
add_executable(banana_k3h4_export_test
        ${CMAKE_CURRENT_LIST_DIR}/runtime/netcode/netcode_k3h4_export_test.c
)

target_link_libraries(banana_k3h4_export_test PRIVATE banana_engine_core)

target_include_directories(banana_k3h4_export_test PRIVATE
        ${BANANA_ENGINE_DIR}
        ${BANANA_ENGINE_DIR}/runtime
)

if(MSVC)
    target_compile_options(banana_k3h4_export_test PRIVATE /W3)
    target_compile_definitions(banana_k3h4_export_test PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
    target_compile_options(banana_k3h4_export_test PRIVATE -Wall -Wextra)
endif()

add_test(NAME banana_k3h4_export_test
        COMMAND banana_k3h4_export_test
        WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../.."
)

set(BANANA_RUNTIME_LEGACY_VOID_MAIN_TARGETS
	banana_runtime_netcode_k3h4_assignment_family_test
	banana_runtime_netcode_k3h4_orchestrator_test
	banana_runtime_netcode_k3h4_covariance_ellipsoid_test
	banana_runtime_netcode_k3h4_pipeline_setup_test
	banana_runtime_netcode_k3h4_pipeline_equivalence_test
	banana_runtime_netcode_abi_combat_integration_test
	banana_runtime_netcode_abi_error_contract_test
	banana_k3h4_export_test
)

foreach(target IN LISTS BANANA_RUNTIME_LEGACY_VOID_MAIN_TARGETS)
	banana_enable_cmocka_legacy_wrapper(${target} "void")
endforeach()

banana_enable_cmocka_legacy_wrapper(banana_k3h4_scaling_benchmark_test "args")

set(BANANA_RUNTIME_NATIVE_CMOCKA_TARGETS
	banana_runtime_navigation_test
	banana_runtime_wildlife_controller_test
	banana_runtime_npc_merchant_test
	banana_runtime_perception_test
	banana_runtime_state_machine_test
	banana_runtime_controller_runtime_test
	banana_runtime_input_contract_test
	banana_runtime_input_click_policy_test
	banana_runtime_physics_body_test
	banana_runtime_physics_collider_test
	banana_runtime_physics_world_test
	banana_runtime_world_test
	banana_runtime_world_query_test
	banana_runtime_ui_test
	banana_runtime_render_camera_test
	banana_runtime_controller_system_test
	banana_runtime_combat_controller_k3h4_test
	banana_runtime_combat_controller_coverage_test
	banana_runtime_netcode_model_test
	banana_runtime_netcode_k3h4_determinism_test
	banana_runtime_netcode_k3h4_edge_cases_test
	banana_runtime_netcode_abi_envelope_endianness_test
)

foreach(target IN LISTS BANANA_RUNTIME_NATIVE_CMOCKA_TARGETS)
	banana_enable_cmocka_native(${target})
endforeach()