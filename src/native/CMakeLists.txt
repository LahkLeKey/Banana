cmake_minimum_required(VERSION 3.20)
project(banana_native_v3 C)

include(CTest)

option(BANANA_ENABLE_POSTGRES "Enable native PostgreSQL/PgBouncer bridge paths" OFF)
option(BANANA_ENABLE_DETERMINISTIC_PERSISTENCE_TESTS
	"Enable deterministic persistent-world native test targets"
	ON)

add_subdirectory(engine)

add_library(banana_native SHARED scaffold/native_entry.c)
set_target_properties(banana_native PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)

target_include_directories(banana_native PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/engine
	${CMAKE_CURRENT_SOURCE_DIR}/engine/runtime
)
target_link_libraries(banana_native PRIVATE banana_engine_core)

if(BANANA_ENABLE_POSTGRES)
	target_compile_definitions(banana_native PRIVATE BANANA_ENABLE_POSTGRES=1)
endif()
