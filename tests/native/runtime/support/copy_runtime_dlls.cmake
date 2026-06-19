if(NOT DEFINED banana_runtime_dlls OR banana_runtime_dlls STREQUAL "")
	return()
endif()

string(REPLACE "|" ";" banana_runtime_dlls_list "${banana_runtime_dlls}")

execute_process(
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${banana_runtime_dlls_list} "${banana_target_dir}"
	RESULT_VARIABLE banana_copy_result
)

if(NOT banana_copy_result EQUAL 0)
	message(FATAL_ERROR "Failed to copy runtime DLLs into ${banana_target_dir}")
endif()