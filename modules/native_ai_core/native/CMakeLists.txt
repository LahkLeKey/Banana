cmake_minimum_required(VERSION 3.20)

if(TARGET banana_native_ai_core)
  return()
endif()

add_library(banana_native_ai_core STATIC)
set_target_properties(banana_native_ai_core PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(banana_native_ai_core
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/native/engine
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/native/engine/ai
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../modules/k3h4/native/include
)

find_package(OpenMP QUIET)
if(OpenMP_C_FOUND)
  target_link_libraries(banana_native_ai_core PUBLIC OpenMP::OpenMP_C)
  target_compile_definitions(banana_native_ai_core PUBLIC BANANA_ENGINE_HAS_OPENMP=1)
else()
  message(FATAL_ERROR "[native_ai_core] OpenMP is required: strict parallel runtime has no serial fallback")
endif()

function(banana_native_ai_core_add_source_if_exists relpath)
  set(_abs "${CMAKE_CURRENT_SOURCE_DIR}/${relpath}")
  if(EXISTS "${_abs}")
    target_sources(banana_native_ai_core PRIVATE "${_abs}")
  endif()
endfunction()

banana_native_ai_core_add_source_if_exists("src/ai/controller/controller.c")
banana_native_ai_core_add_source_if_exists("src/ai/controller/controller_system_dispatch.c")
banana_native_ai_core_add_source_if_exists("src/ai/controller/controller_system_lifecycle.c")
banana_native_ai_core_add_source_if_exists("src/ai/combat/combat_controller.c")
banana_native_ai_core_add_source_if_exists("src/ai/navigation/navigation.c")
banana_native_ai_core_add_source_if_exists("src/ai/npc/npc_merchant.c")
banana_native_ai_core_add_source_if_exists("src/ai/perception/perception.c")
banana_native_ai_core_add_source_if_exists("src/ai/state/state_machine.c")
banana_native_ai_core_add_source_if_exists("src/ai/wildlife/wildlife_controller.c")
