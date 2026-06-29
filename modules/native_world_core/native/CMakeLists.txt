cmake_minimum_required(VERSION 3.20)

if(TARGET banana_native_world_core)
  return()
endif()

add_library(banana_native_world_core STATIC)
set_target_properties(banana_native_world_core PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(banana_native_world_core
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/native/engine
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/native/engine/world
)

find_package(OpenMP QUIET)
if(OpenMP_C_FOUND)
  target_link_libraries(banana_native_world_core PUBLIC OpenMP::OpenMP_C)
endif()

function(banana_native_world_core_add_source_if_exists relpath)
  set(_abs "${CMAKE_CURRENT_SOURCE_DIR}/${relpath}")
  if(EXISTS "${_abs}")
    target_sources(banana_native_world_core PRIVATE "${_abs}")
  endif()
endfunction()

banana_native_world_core_add_source_if_exists("src/world/core/world.c")
banana_native_world_core_add_source_if_exists("src/world/entity/entity.c")
banana_native_world_core_add_source_if_exists("src/world/query/world_query.c")
banana_native_world_core_add_source_if_exists("src/world/signals/signals.c")
