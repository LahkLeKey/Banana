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

add_executable(banana_test_ml_regression test_ml_regression.c)
target_link_libraries(banana_test_ml_regression PRIVATE banana_native)
add_test(NAME banana_test_ml_regression COMMAND banana_test_ml_regression)

add_executable(banana_test_ml_binary test_ml_binary.c)
target_link_libraries(banana_test_ml_binary PRIVATE banana_native)
add_test(NAME banana_test_ml_binary COMMAND banana_test_ml_binary)

add_executable(banana_test_ml_transformer test_ml_transformer.c)
target_link_libraries(banana_test_ml_transformer PRIVATE banana_native)
add_test(NAME banana_test_ml_transformer COMMAND banana_test_ml_transformer)

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
  add_custom_command(TARGET banana_test_ml_regression POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      $<TARGET_FILE:banana_native>
      $<TARGET_FILE_DIR:banana_test_ml_regression>)
  add_custom_command(TARGET banana_test_ml_binary POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      $<TARGET_FILE:banana_native>
      $<TARGET_FILE_DIR:banana_test_ml_binary>)
  add_custom_command(TARGET banana_test_ml_transformer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      $<TARGET_FILE:banana_native>
      $<TARGET_FILE_DIR:banana_test_ml_transformer>)
endif()
