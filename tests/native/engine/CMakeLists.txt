# Engine test binary — Phase 1 (T004)
add_executable(banana_test_engine_phase1
  test_engine_phase1.c
)

target_link_libraries(banana_test_engine_phase1 PRIVATE banana_engine_core)

if(UNIX)
  target_link_libraries(banana_test_engine_phase1 PRIVATE m)
endif()

# Windows: copy engine DLL if needed (banana_engine_core is static, so not needed yet)

add_test(NAME banana_test_engine_phase1 COMMAND banana_test_engine_phase1)
