# Native test taxonomy (spec 014). Linked against the wrapper SHARED lib.
add_executable(banana_test_calc test_calc.c)
target_link_libraries(banana_test_calc PRIVATE banana_native)
add_test(NAME banana_test_calc COMMAND banana_test_calc)
# Windows: ensure the wrapper DLL sits next to the test executable.
if(WIN32)
  add_custom_command(TARGET banana_test_calc POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      $<TARGET_FILE:banana_native>
      $<TARGET_FILE_DIR:banana_test_calc>)
endif()
