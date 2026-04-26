# Native test taxonomy (spec 014). Linked against the wrapper SHARED lib.
add_executable(banana_test_calc test_calc.c)
target_link_libraries(banana_test_calc PRIVATE banana_native)
add_test(NAME banana_test_calc COMMAND banana_test_calc)

add_executable(banana_test_ml_models test_ml_models.c)
target_link_libraries(banana_test_ml_models PRIVATE banana_native)
add_test(NAME banana_test_ml_models COMMAND banana_test_ml_models)

add_executable(banana_test_operational_domains test_operational_domains.c)
target_link_libraries(banana_test_operational_domains PRIVATE banana_native)
add_test(NAME banana_test_operational_domains COMMAND banana_test_operational_domains)

add_executable(banana_test_dal_contracts test_dal_contracts.c)
target_link_libraries(banana_test_dal_contracts PRIVATE banana_native)
add_test(NAME banana_test_dal_contracts COMMAND banana_test_dal_contracts)

# Windows: ensure the wrapper DLL sits next to the test executable.
if(WIN32)
  add_custom_command(TARGET banana_test_calc POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      $<TARGET_FILE:banana_native>
      $<TARGET_FILE_DIR:banana_test_calc>)
  add_custom_command(TARGET banana_test_ml_models POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      $<TARGET_FILE:banana_native>
      $<TARGET_FILE_DIR:banana_test_ml_models>)
  add_custom_command(TARGET banana_test_operational_domains POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      $<TARGET_FILE:banana_native>
      $<TARGET_FILE_DIR:banana_test_operational_domains>)
  add_custom_command(TARGET banana_test_dal_contracts POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      $<TARGET_FILE:banana_native>
      $<TARGET_FILE_DIR:banana_test_dal_contracts>)
endif()
