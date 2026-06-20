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

	if(WIN32)
		add_custom_command(TARGET ${target} POST_BUILD
			COMMAND ${CMAKE_COMMAND}
				-Dbanana_runtime_dlls=$<JOIN:$<TARGET_RUNTIME_DLLS:${target}>,|>
				-Dbanana_target_dir=$<TARGET_FILE_DIR:${target}>
				-P ${CMAKE_CURRENT_LIST_DIR}/runtime/support/copy_runtime_dlls.cmake
			COMMENT "Copying runtime DLLs next to ${target}"
		)
	endif()
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

add_executable(banana_runtime_ui_alloc_failure_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/ui/core/ui_alloc_failure_test.c
	${BANANA_ENGINE_DIR}/ui/core/ui.c
)

add_test(NAME banana_runtime_ui_alloc_failure_test
	COMMAND banana_runtime_ui_alloc_failure_test
)

target_compile_definitions(banana_runtime_ui_alloc_failure_test PRIVATE
	malloc=banana_test_malloc
	calloc=banana_test_calloc
	free=banana_test_free
)

target_include_directories(banana_runtime_ui_alloc_failure_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/ui
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_runtime_ui_inventory_impl_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/ui/inventory/ui_inventory_impl_test.c
	${BANANA_ENGINE_DIR}/ui/inventory/ui_inventory.c
)

add_test(NAME banana_runtime_ui_inventory_impl_test
	COMMAND banana_runtime_ui_inventory_impl_test
)

target_include_directories(banana_runtime_ui_inventory_impl_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/ui
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_runtime_ui_merchant_impl_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/ui/merchant/ui_merchant_impl_test.c
	${BANANA_ENGINE_DIR}/ui/merchant/ui_merchant.c
)

add_test(NAME banana_runtime_ui_merchant_impl_test
	COMMAND banana_runtime_ui_merchant_impl_test
)

target_include_directories(banana_runtime_ui_merchant_impl_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/ui
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_runtime_ui_abi_and_headers_smoke_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/ui/abi/native_ui_abi_and_headers_smoke_test.c
)

add_test(NAME banana_runtime_ui_abi_and_headers_smoke_test
	COMMAND banana_runtime_ui_abi_and_headers_smoke_test
)

target_include_directories(banana_runtime_ui_abi_and_headers_smoke_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
	${CMAKE_CURRENT_LIST_DIR}/../../src/native
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
	)

	target_include_directories(banana_runtime_controller_runtime_cmocka_test PRIVATE
		${BANANA_ENGINE_DIR}
		${BANANA_ENGINE_DIR}/runtime
	)

	banana_enable_cmocka_native(banana_runtime_controller_runtime_cmocka_test)
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

add_executable(banana_runtime_tick_budget_policy_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/tick/tick_budget_policy_test.c
)

add_test(NAME banana_runtime_tick_budget_policy_test
	COMMAND banana_runtime_tick_budget_policy_test
)

target_link_libraries(banana_runtime_tick_budget_policy_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_tick_budget_policy_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_runtime_tick_post_phase_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/tick/tick_post_phase_test.c
)

add_test(NAME banana_runtime_tick_post_phase_test
	COMMAND banana_runtime_tick_post_phase_test
)

target_link_libraries(banana_runtime_tick_post_phase_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_tick_post_phase_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_runtime_terrain_tick_orchestration_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/orchestration/tick/loop/terrain_tick_orchestration_test.c
)

add_test(NAME banana_runtime_terrain_tick_orchestration_test
	COMMAND banana_runtime_terrain_tick_orchestration_test
)

target_link_libraries(banana_runtime_terrain_tick_orchestration_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_terrain_tick_orchestration_test PRIVATE
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

add_executable(banana_runtime_render_material_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/render/material/material_test.c
)

add_test(NAME banana_runtime_render_material_test
	COMMAND banana_runtime_render_material_test
)

target_link_libraries(banana_runtime_render_material_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_render_material_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_runtime_camera_basis_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/camera/basis/camera_basis_test.c
)

add_test(NAME banana_runtime_camera_basis_test
	COMMAND banana_runtime_camera_basis_test
)

target_link_libraries(banana_runtime_camera_basis_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_camera_basis_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_runtime_engine_runtime_context_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/engine/exports/engine_runtime_context_test.c
)

add_test(NAME banana_runtime_engine_runtime_context_test
	COMMAND banana_runtime_engine_runtime_context_test
)

target_link_libraries(banana_runtime_engine_runtime_context_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_engine_runtime_context_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_runtime_application_service_ports_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/engine/application/service/application_service_ports_test.c
)

add_test(NAME banana_runtime_application_service_ports_test
	COMMAND banana_runtime_application_service_ports_test
)

target_link_libraries(banana_runtime_application_service_ports_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_application_service_ports_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_runtime_application_service_render_port_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/engine/application/service/application_service_render_port_test.c
)

add_test(NAME banana_runtime_application_service_render_port_test
	COMMAND banana_runtime_application_service_render_port_test
)

target_link_libraries(banana_runtime_application_service_render_port_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_application_service_render_port_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_runtime_engine_tick_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/engine/tick/engine_tick_test.c
)

add_test(NAME banana_runtime_engine_tick_test
	COMMAND banana_runtime_engine_tick_test
)

target_link_libraries(banana_runtime_engine_tick_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_engine_tick_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_runtime_engine_lifecycle_spawn_math_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/engine/lifecycle/engine_lifecycle_spawn_math_test.c
)

add_test(NAME banana_runtime_engine_lifecycle_spawn_math_test
	COMMAND banana_runtime_engine_lifecycle_spawn_math_test
)

target_link_libraries(banana_runtime_engine_lifecycle_spawn_math_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_engine_lifecycle_spawn_math_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_runtime_engine_lifecycle_launch_gate_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/engine/lifecycle/engine_lifecycle_launch_gate_test.c
)

add_test(NAME banana_runtime_engine_lifecycle_launch_gate_test
	COMMAND banana_runtime_engine_lifecycle_launch_gate_test
)

target_link_libraries(banana_runtime_engine_lifecycle_launch_gate_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_engine_lifecycle_launch_gate_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_runtime_engine_lifecycle_launch_gate_decide_failure_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/engine/lifecycle/engine_lifecycle_launch_gate_decide_failure_test.c
	${BANANA_ENGINE_DIR}/runtime/engine/lifecycle/engine_lifecycle_launch_gate.c
)

add_test(NAME banana_runtime_engine_lifecycle_launch_gate_decide_failure_test
	COMMAND banana_runtime_engine_lifecycle_launch_gate_decide_failure_test
)

target_include_directories(banana_runtime_engine_lifecycle_launch_gate_decide_failure_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_runtime_engine_lifecycle_actor_spawn_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/engine/lifecycle/engine_lifecycle_actor_spawn_test.c
)

add_test(NAME banana_runtime_engine_lifecycle_actor_spawn_test
	COMMAND banana_runtime_engine_lifecycle_actor_spawn_test
)

target_link_libraries(banana_runtime_engine_lifecycle_actor_spawn_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_engine_lifecycle_actor_spawn_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_runtime_engine_composition_shutdown_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/engine/composition/engine_composition_shutdown_test.c
)

add_test(NAME banana_runtime_engine_composition_shutdown_test
	COMMAND banana_runtime_engine_composition_shutdown_test
)

target_link_libraries(banana_runtime_engine_composition_shutdown_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_engine_composition_shutdown_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_runtime_engine_composition_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/engine/composition/engine_composition_test.c
)

add_test(NAME banana_runtime_engine_composition_test
	COMMAND banana_runtime_engine_composition_test
)

target_link_libraries(banana_runtime_engine_composition_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_engine_composition_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_runtime_engine_state_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/engine/state/engine_state_test.c
)

add_test(NAME banana_runtime_engine_state_test
	COMMAND banana_runtime_engine_state_test
)

target_link_libraries(banana_runtime_engine_state_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_engine_state_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_runtime_engine_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/engine/engine_test.c
)

add_test(NAME banana_runtime_engine_test
	COMMAND banana_runtime_engine_test
)

target_link_libraries(banana_runtime_engine_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_engine_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_runtime_engine_service_ports_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/engine/engine_service_ports_test.c
	${BANANA_ENGINE_DIR}/engine.c
)

add_test(NAME banana_runtime_engine_service_ports_test
	COMMAND banana_runtime_engine_service_ports_test
)

target_include_directories(banana_runtime_engine_service_ports_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_runtime_gameplay_service_sentience_decision_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/engine/gameplay/service/gameplay_service_sentience_decision_test.c
	${BANANA_ENGINE_DIR}/runtime/engine/gameplay/service/gameplay_service_sentience_decision.c
)

add_test(NAME banana_runtime_gameplay_service_sentience_decision_test
	COMMAND banana_runtime_gameplay_service_sentience_decision_test
)

target_include_directories(banana_runtime_gameplay_service_sentience_decision_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_runtime_gameplay_service_sentience_behavior_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/engine/gameplay/service/gameplay_service_sentience_behavior_test.c
	${BANANA_ENGINE_DIR}/runtime/engine/gameplay/service/gameplay_service_sentience_behavior.c
)

add_test(NAME banana_runtime_gameplay_service_sentience_behavior_test
	COMMAND banana_runtime_gameplay_service_sentience_behavior_test
)

target_include_directories(banana_runtime_gameplay_service_sentience_behavior_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_runtime_gameplay_service_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/engine/gameplay/service/gameplay_service_test.c
	${BANANA_ENGINE_DIR}/runtime/engine/gameplay/service/gameplay_service.c
)

add_test(NAME banana_runtime_gameplay_service_test
	COMMAND banana_runtime_gameplay_service_test
)

target_include_directories(banana_runtime_gameplay_service_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_runtime_merchant_query_abi_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/merchant/query/merchant_query_abi_test.c
	${BANANA_ENGINE_DIR}/runtime/merchant/query/merchant_query_abi.c
)

add_test(NAME banana_runtime_merchant_query_abi_test
	COMMAND banana_runtime_merchant_query_abi_test
)

target_include_directories(banana_runtime_merchant_query_abi_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_asset_algorithms_test
	${CMAKE_CURRENT_LIST_DIR}/tools/domain/banana_asset_algorithms_test.c
	${BANANA_ENGINE_DIR}/tools/domain/banana_asset_algorithms.c
	${BANANA_ENGINE_DIR}/tools/domain/banana_asset_wfc.c
	${BANANA_ENGINE_DIR}/tools/domain/banana_asset_cellular.c
	${BANANA_ENGINE_DIR}/tools/domain/banana_asset_domain.c
)

add_test(NAME banana_asset_algorithms_test
	COMMAND banana_asset_algorithms_test
)

target_include_directories(banana_asset_algorithms_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

target_compile_definitions(banana_asset_algorithms_test PRIVATE BANANA_ENGINE_HAS_OPENMP=1)

add_executable(banana_native_k3h4_metrics_infrastructure_adapter_test
	${CMAKE_CURRENT_LIST_DIR}/k3h4/infrastructure/k3h4_metrics_infrastructure_adapter_test.c
)

add_test(NAME banana_native_k3h4_metrics_infrastructure_adapter_test
	COMMAND banana_native_k3h4_metrics_infrastructure_adapter_test
)

target_link_libraries(banana_native_k3h4_metrics_infrastructure_adapter_test PRIVATE banana_engine_core)
target_link_libraries(banana_native_k3h4_metrics_infrastructure_adapter_test PRIVATE banana_native)

if(WIN32)
	add_custom_command(TARGET banana_native_k3h4_metrics_infrastructure_adapter_test POST_BUILD
		COMMAND ${CMAKE_COMMAND}
			-Dbanana_runtime_dlls=$<JOIN:$<TARGET_RUNTIME_DLLS:banana_native_k3h4_metrics_infrastructure_adapter_test>,|>
			-Dbanana_target_dir=$<TARGET_FILE_DIR:banana_native_k3h4_metrics_infrastructure_adapter_test>
			-P ${CMAKE_CURRENT_LIST_DIR}/runtime/support/copy_runtime_dlls.cmake
		COMMENT "Copying runtime DLLs next to banana_native_k3h4_metrics_infrastructure_adapter_test"
	)
endif()

target_include_directories(banana_native_k3h4_metrics_infrastructure_adapter_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
	${CMAKE_CURRENT_LIST_DIR}/../../src/native
)

add_executable(banana_native_k3h4_metrics_application_service_test
	${CMAKE_CURRENT_LIST_DIR}/k3h4/application/k3h4_metrics_application_service_test.c
)

add_test(NAME banana_native_k3h4_metrics_application_service_test
	COMMAND banana_native_k3h4_metrics_application_service_test
)

target_link_libraries(banana_native_k3h4_metrics_application_service_test PRIVATE banana_engine_core)
target_link_libraries(banana_native_k3h4_metrics_application_service_test PRIVATE banana_native)

if(WIN32)
	add_custom_command(TARGET banana_native_k3h4_metrics_application_service_test POST_BUILD
		COMMAND ${CMAKE_COMMAND}
			-Dbanana_runtime_dlls=$<JOIN:$<TARGET_RUNTIME_DLLS:banana_native_k3h4_metrics_application_service_test>,|>
			-Dbanana_target_dir=$<TARGET_FILE_DIR:banana_native_k3h4_metrics_application_service_test>
			-P ${CMAKE_CURRENT_LIST_DIR}/runtime/support/copy_runtime_dlls.cmake
		COMMENT "Copying runtime DLLs next to banana_native_k3h4_metrics_application_service_test"
	)
endif()

target_include_directories(banana_native_k3h4_metrics_application_service_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
	${CMAKE_CURRENT_LIST_DIR}/../../src/native
)

add_executable(banana_native_k3h4_metrics_orchestration_layer_test
	${CMAKE_CURRENT_LIST_DIR}/k3h4/k3h4_metrics_orchestration_layer_test.c
)

add_test(NAME banana_native_k3h4_metrics_orchestration_layer_test
	COMMAND banana_native_k3h4_metrics_orchestration_layer_test
)

target_link_libraries(banana_native_k3h4_metrics_orchestration_layer_test PRIVATE banana_engine_core)
target_link_libraries(banana_native_k3h4_metrics_orchestration_layer_test PRIVATE banana_native)

if(WIN32)
	add_custom_command(TARGET banana_native_k3h4_metrics_orchestration_layer_test POST_BUILD
		COMMAND ${CMAKE_COMMAND}
			-Dbanana_runtime_dlls=$<JOIN:$<TARGET_RUNTIME_DLLS:banana_native_k3h4_metrics_orchestration_layer_test>,|>
			-Dbanana_target_dir=$<TARGET_FILE_DIR:banana_native_k3h4_metrics_orchestration_layer_test>
			-P ${CMAKE_CURRENT_LIST_DIR}/runtime/support/copy_runtime_dlls.cmake
		COMMENT "Copying runtime DLLs next to banana_native_k3h4_metrics_orchestration_layer_test"
	)
endif()

target_include_directories(banana_native_k3h4_metrics_orchestration_layer_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
	${CMAKE_CURRENT_LIST_DIR}/../../src/native
)

add_executable(banana_runtime_terrain_generation_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/terrain/terrain_generation_test.c
)

add_test(NAME banana_runtime_terrain_generation_test
	COMMAND banana_runtime_terrain_generation_test
)

target_link_libraries(banana_runtime_terrain_generation_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_terrain_generation_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_runtime_world_abi_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/world/world_abi_test.c
)

add_test(NAME banana_runtime_world_abi_test
	COMMAND banana_runtime_world_abi_test
)

target_link_libraries(banana_runtime_world_abi_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_world_abi_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_runtime_player_builds_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/player/build/player_builds_test.c
)

add_test(NAME banana_runtime_player_builds_test
	COMMAND banana_runtime_player_builds_test
)

target_link_libraries(banana_runtime_player_builds_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_player_builds_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_runtime_player_registry_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/player/registry/player_registry_test.c
)

add_test(NAME banana_runtime_player_registry_test
	COMMAND banana_runtime_player_registry_test
)

target_link_libraries(banana_runtime_player_registry_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_player_registry_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_runtime_resource_domain_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/resource/resource_domain_test.c
)

add_test(NAME banana_runtime_resource_domain_test
	COMMAND banana_runtime_resource_domain_test
)

target_link_libraries(banana_runtime_resource_domain_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_resource_domain_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_runtime_move_target_service_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/input/move_target/move_target_service_test.c
)

add_test(NAME banana_runtime_move_target_service_test
	COMMAND banana_runtime_move_target_service_test
)

target_link_libraries(banana_runtime_move_target_service_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_move_target_service_test PRIVATE
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

add_executable(banana_runtime_controller_system_lifecycle_alloc_failure_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/ai/controller/controller_system_lifecycle_alloc_failure_test.c
	${BANANA_ENGINE_DIR}/ai/controller/controller_system_lifecycle.c
)

add_test(NAME banana_runtime_controller_system_lifecycle_alloc_failure_test
	COMMAND banana_runtime_controller_system_lifecycle_alloc_failure_test
)

target_compile_definitions(banana_runtime_controller_system_lifecycle_alloc_failure_test PRIVATE
	calloc=banana_test_calloc
)

target_include_directories(banana_runtime_controller_system_lifecycle_alloc_failure_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/ai
	${BANANA_ENGINE_DIR}/runtime
)

add_executable(banana_runtime_controller_system_dispatch_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/ai/controller/controller_system_dispatch_test.c
)

add_test(NAME banana_runtime_controller_system_dispatch_test
	COMMAND banana_runtime_controller_system_dispatch_test
)

target_link_libraries(banana_runtime_controller_system_dispatch_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_controller_system_dispatch_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

if(BANANA_ENABLE_CMOCKA_TESTS AND cmocka_FOUND)
	banana_enable_cmocka_native(banana_runtime_controller_system_test)
endif()
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

if(BANANA_ENABLE_CMOCKA_TESTS AND cmocka_FOUND)
	add_executable(banana_runtime_ai_controller_team_navigation_cmocka_test
		${CMAKE_CURRENT_LIST_DIR}/runtime/ai/controller/controller_team_navigation_test.c
	)

	add_test(NAME banana_runtime_ai_controller_team_navigation_cmocka_test
		COMMAND banana_runtime_ai_controller_team_navigation_cmocka_test
	)

	target_link_libraries(banana_runtime_ai_controller_team_navigation_cmocka_test PRIVATE
		banana_engine_core
	)

	target_include_directories(banana_runtime_ai_controller_team_navigation_cmocka_test PRIVATE
		${BANANA_ENGINE_DIR}
		${BANANA_ENGINE_DIR}/runtime
	)

	banana_enable_cmocka_native(banana_runtime_ai_controller_team_navigation_cmocka_test)
endif()

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

add_executable(banana_k3h4_export_io_failure_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/netcode/netcode_k3h4_export_io_failure_test.c
	${BANANA_ENGINE_DIR}/runtime/netcode/k3h4/netcode_k3h4_export.c
)

target_compile_definitions(banana_k3h4_export_io_failure_test PRIVATE
	fopen=banana_test_fopen
	fwrite=banana_test_fwrite
	rename=banana_test_rename
	remove=banana_test_remove
	_mkdir=banana_test_mkdir
)

target_include_directories(banana_k3h4_export_io_failure_test PRIVATE
	${BANANA_ENGINE_DIR}
	${BANANA_ENGINE_DIR}/runtime
)

if(MSVC)
    target_compile_options(banana_k3h4_export_io_failure_test PRIVATE /W3)
    target_compile_definitions(banana_k3h4_export_io_failure_test PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
    target_compile_options(banana_k3h4_export_io_failure_test PRIVATE -Wall -Wextra)
endif()

add_test(NAME banana_k3h4_export_io_failure_test
	COMMAND banana_k3h4_export_io_failure_test
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
	banana_runtime_gameplay_service_sentience_decision_test
	banana_runtime_gameplay_service_sentience_behavior_test
	banana_runtime_gameplay_service_test
	banana_runtime_merchant_query_abi_test
	banana_runtime_ui_inventory_impl_test
	banana_runtime_ui_merchant_impl_test
	banana_runtime_ui_abi_and_headers_smoke_test
	banana_asset_algorithms_test
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
	banana_runtime_ui_alloc_failure_test
	banana_runtime_ui_inventory_impl_test
	banana_runtime_ui_merchant_impl_test
	banana_runtime_ui_abi_and_headers_smoke_test
	banana_runtime_render_camera_test
	banana_runtime_controller_system_test
	banana_runtime_combat_controller_k3h4_test
	banana_runtime_combat_controller_coverage_test
	banana_runtime_netcode_model_test
	banana_runtime_netcode_k3h4_determinism_test
	banana_runtime_netcode_k3h4_edge_cases_test
	banana_runtime_netcode_abi_envelope_endianness_test
	banana_runtime_merchant_query_abi_test
	banana_asset_algorithms_test
	banana_k3h4_export_io_failure_test
)

foreach(target IN LISTS BANANA_RUNTIME_NATIVE_CMOCKA_TARGETS)
	banana_enable_cmocka_native(${target})
endforeach()