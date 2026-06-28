cmake_minimum_required(VERSION 3.20)

if(TARGET banana_native_physics_core)
  return()
endif()

add_library(banana_native_physics_core STATIC)
set_target_properties(banana_native_physics_core PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(banana_native_physics_core
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/native/engine
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/native/engine/physics
)

find_package(OpenMP QUIET)
if(OpenMP_C_FOUND)
  target_link_libraries(banana_native_physics_core PUBLIC OpenMP::OpenMP_C)
  target_compile_definitions(banana_native_physics_core PUBLIC BANANA_ENGINE_HAS_OPENMP=1)
else()
  message(FATAL_ERROR "[native_physics_core] OpenMP is required: strict parallel runtime has no serial fallback")
endif()

function(banana_native_physics_core_add_source_if_exists relpath)
  set(_abs "${CMAKE_CURRENT_SOURCE_DIR}/${relpath}")
  if(EXISTS "${_abs}")
    target_sources(banana_native_physics_core PRIVATE "${_abs}")
  endif()
endfunction()

banana_native_physics_core_add_source_if_exists("src/physics/body/body.c")
banana_native_physics_core_add_source_if_exists("src/physics/collider/collider_broad_phase.c")
banana_native_physics_core_add_source_if_exists("src/physics/collider/collider_narrow_phase.c")
banana_native_physics_core_add_source_if_exists("src/physics/collider/collider_resolve.c")
banana_native_physics_core_add_source_if_exists("src/physics/dynamics/dynamics.c")
banana_native_physics_core_add_source_if_exists("src/physics/raycast/raycast.c")
banana_native_physics_core_add_source_if_exists("src/physics/world/world_collision.c")
banana_native_physics_core_add_source_if_exists("src/physics/world/world.c")
