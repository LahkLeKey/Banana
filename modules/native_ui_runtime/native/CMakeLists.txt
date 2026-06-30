cmake_minimum_required(VERSION 3.20)

if(TARGET banana_native_ui_runtime)
  return()
endif()

add_library(banana_native_ui_runtime STATIC)
set_target_properties(banana_native_ui_runtime PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(banana_native_ui_runtime
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/native/engine
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/native/engine/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/native/engine/runtime
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/native/engine/runtime/abi/ui
)

function(banana_native_ui_runtime_add_source_if_exists relpath)
  set(_abs "${CMAKE_CURRENT_SOURCE_DIR}/${relpath}")
  if(EXISTS "${_abs}")
    target_sources(banana_native_ui_runtime PRIVATE "${_abs}")
  endif()
endfunction()

banana_native_ui_runtime_add_source_if_exists("src/ui/core/ui.c")
banana_native_ui_runtime_add_source_if_exists("src/ui/inventory/ui_inventory.c")
banana_native_ui_runtime_add_source_if_exists("src/ui/merchant/ui_merchant.c")
banana_native_ui_runtime_add_source_if_exists("src/ui/render/ui_render.c")
banana_native_ui_runtime_add_source_if_exists("src/ui/callbacks/ui_callback_bindings.c")
banana_native_ui_runtime_add_source_if_exists("src/runtime/abi/ui/ui_abi.c")
