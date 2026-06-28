cmake_minimum_required(VERSION 3.20)

if(TARGET banana_native_netcode_runtime)
  return()
endif()

add_library(banana_native_netcode_runtime STATIC)
set_target_properties(banana_native_netcode_runtime PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(banana_native_netcode_runtime
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/native/engine
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/native/engine/runtime
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/native/engine/runtime/netcode/model
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/native/engine/runtime/netcode/reward
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/native/engine/runtime/netcode/link
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/native/engine/runtime/netcode/vector
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/native/engine/runtime/netcode/contract
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/native/engine/runtime/abi/netcode
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../modules/k3h4/native/include
)

target_link_libraries(banana_native_netcode_runtime PUBLIC banana_k3h4_core)

function(banana_native_netcode_runtime_add_source_if_exists relpath)
  set(_abs "${CMAKE_CURRENT_SOURCE_DIR}/${relpath}")
  if(EXISTS "${_abs}")
    target_sources(banana_native_netcode_runtime PRIVATE "${_abs}")
  endif()
endfunction()

banana_native_netcode_runtime_add_source_if_exists("src/runtime/netcode/model/netcode_model.c")
banana_native_netcode_runtime_add_source_if_exists("src/runtime/netcode/reward/netcode_reward.c")
banana_native_netcode_runtime_add_source_if_exists("src/runtime/netcode/link/netcode_link.c")
banana_native_netcode_runtime_add_source_if_exists("src/runtime/netcode/vector/netcode_vector.c")
banana_native_netcode_runtime_add_source_if_exists("src/runtime/netcode/contract/netcode_contract.c")
banana_native_netcode_runtime_add_source_if_exists("src/runtime/abi/netcode/netcode_abi.c")
