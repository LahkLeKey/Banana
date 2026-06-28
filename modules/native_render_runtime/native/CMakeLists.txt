cmake_minimum_required(VERSION 3.20)

if(TARGET banana_native_render_runtime)
  return()
endif()

add_library(banana_native_render_runtime STATIC)
set_target_properties(banana_native_render_runtime PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(banana_native_render_runtime
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/native/engine
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/native/engine/runtime
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/native/engine/runtime/render/material
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/native/engine/runtime/render/submit
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/native/engine/runtime/orchestration/tick/loop
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/native/engine/runtime/engine/application/service
)

function(banana_native_render_runtime_add_source_if_exists relpath)
  set(_abs "${CMAKE_CURRENT_SOURCE_DIR}/${relpath}")
  if(EXISTS "${_abs}")
    target_sources(banana_native_render_runtime PRIVATE "${_abs}")
  endif()
endfunction()

banana_native_render_runtime_add_source_if_exists("src/runtime/render/material/render_material.c")
banana_native_render_runtime_add_source_if_exists("src/runtime/render/submit/render_submit.c")
banana_native_render_runtime_add_source_if_exists("src/runtime/orchestration/tick/loop/render_tick_orchestration.c")
banana_native_render_runtime_add_source_if_exists("src/runtime/engine/application/service/application_service_render_actor_resolver.c")
banana_native_render_runtime_add_source_if_exists("src/runtime/engine/application/service/application_service_render_model_cache.c")
banana_native_render_runtime_add_source_if_exists("src/runtime/engine/application/service/application_service_render_port.c")
