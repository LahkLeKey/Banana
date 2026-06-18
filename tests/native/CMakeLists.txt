# Native test extensions are wired by engine CMake includes.

set(BANANA_ENGINE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../src/native/engine")

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

add_executable(banana_runtime_combat_controller_k3h4_test
	${CMAKE_CURRENT_LIST_DIR}/runtime/combat_controller_k3h4_test.c
)

target_link_libraries(banana_runtime_combat_controller_k3h4_test PRIVATE banana_engine_core)

target_include_directories(banana_runtime_combat_controller_k3h4_test PRIVATE
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