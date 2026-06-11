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
