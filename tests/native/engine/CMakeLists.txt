# Engine domain contract test binary (DDD/SOLID aligned)
add_executable(banana_test_engine_domain_contracts
  test_engine_domain_contracts.c
  test_engine_physics_suite.c
  test_engine_ai_fsm_suite.c
  test_engine_ai_navigation_suite.c
  test_engine_ai_controller_suite.c
  test_engine_render_suite.c
  test_engine_world_suite.c
  test_engine_interaction_suite.c
)

target_link_libraries(banana_test_engine_domain_contracts PRIVATE banana_engine_core)

if(UNIX)
  target_link_libraries(banana_test_engine_domain_contracts PRIVATE m)
endif()

# Windows: copy engine DLL if needed (banana_engine_core is static, so not needed yet)

add_test(NAME banana_test_engine_domain_contracts COMMAND banana_test_engine_domain_contracts)

add_executable(banana_test_engine_asset_generation
  test_engine_asset_generation.c
  ../../../src/native/engine/tools/domain/banana_asset_domain.c
  ../../../src/native/engine/tools/domain/banana_asset_algorithms.c
  ../../../src/native/engine/tools/application/banana_asset_generation_service.c
)

target_include_directories(banana_test_engine_asset_generation PRIVATE
  ../../../src/native/engine/tools
  ../../../src/native/engine/tools/domain
  ../../../src/native/engine/tools/application
)

if(UNIX)
  target_link_libraries(banana_test_engine_asset_generation PRIVATE m)
endif()

add_test(NAME banana_test_engine_asset_generation COMMAND banana_test_engine_asset_generation)

add_executable(banana_test_engine_asset_compiler_integration
  test_engine_asset_compiler_integration.c
)

add_dependencies(banana_test_engine_asset_compiler_integration banana_asset_compiler)
target_compile_definitions(
  banana_test_engine_asset_compiler_integration
  PRIVATE BANANA_ASSET_COMPILER_PATH="$<TARGET_FILE:banana_asset_compiler>")

add_test(
  NAME banana_test_engine_asset_compiler_integration
  COMMAND banana_test_engine_asset_compiler_integration)

add_executable(banana_test_engine_runtime_contracts
  test_engine_runtime_contracts.c
)

target_link_libraries(banana_test_engine_runtime_contracts PRIVATE banana_engine_core)

if(UNIX)
  target_link_libraries(banana_test_engine_runtime_contracts PRIVATE m)
endif()

add_test(NAME banana_test_engine_runtime_contracts COMMAND banana_test_engine_runtime_contracts)
