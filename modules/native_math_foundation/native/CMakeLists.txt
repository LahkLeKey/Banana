cmake_minimum_required(VERSION 3.20)

if(TARGET banana_native_math_foundation)
  return()
endif()

add_library(banana_native_math_foundation STATIC)
set_target_properties(banana_native_math_foundation PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(banana_native_math_foundation
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(_banana_native_math_foundation_fixed_point_src
  "${CMAKE_CURRENT_SOURCE_DIR}/src/runtime/netcode/vector/netcode_fixed_point.c")
if(EXISTS "${_banana_native_math_foundation_fixed_point_src}")
  target_sources(banana_native_math_foundation PRIVATE "${_banana_native_math_foundation_fixed_point_src}")
endif()
