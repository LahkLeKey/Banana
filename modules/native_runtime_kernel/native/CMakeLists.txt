cmake_minimum_required(VERSION 3.20)

if(TARGET banana_native_runtime_kernel)
  return()
endif()

add_library(banana_native_runtime_kernel STATIC)
set_target_properties(banana_native_runtime_kernel PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(banana_native_runtime_kernel
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/native/engine
)

find_package(OpenMP QUIET)
if(OpenMP_C_FOUND)
  target_link_libraries(banana_native_runtime_kernel PUBLIC OpenMP::OpenMP_C)
elseif(NOT MSVC)
  target_link_libraries(banana_native_runtime_kernel PUBLIC pthread)
endif()

function(banana_native_runtime_kernel_add_source_if_exists relpath)
  set(_abs "${CMAKE_CURRENT_SOURCE_DIR}/${relpath}")
  if(EXISTS "${_abs}")
    target_sources(banana_native_runtime_kernel PRIVATE "${_abs}")
  endif()
endfunction()

banana_native_runtime_kernel_add_source_if_exists("src/runtime/parallel/contract/parallel_contract.c")
banana_native_runtime_kernel_add_source_if_exists("src/runtime/parallel/model/parallel_model.c")
banana_native_runtime_kernel_add_source_if_exists("src/runtime/parallel/interlock/interlock.c")
