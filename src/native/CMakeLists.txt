cmake_minimum_required(VERSION 3.20)
project(banana_native_v3 C)

include(CTest)

option(BANANA_ENABLE_POSTGRES "Enable native PostgreSQL/PgBouncer bridge paths" OFF)
option(BANANA_ENABLE_DETERMINISTIC_PERSISTENCE_TESTS
	"Enable deterministic persistent-world native test targets"
	ON)
option(BANANA_ENABLE_COVERAGE "Enable native coverage instrumentation" OFF)

if(BANANA_ENABLE_COVERAGE AND NOT MSVC)
	add_compile_options(--coverage)
	add_link_options(--coverage)
endif()

add_subdirectory(
	${CMAKE_CURRENT_SOURCE_DIR}/../../modules/native_math_foundation
	${CMAKE_CURRENT_BINARY_DIR}/modules/native_math_foundation
)

add_subdirectory(
	${CMAKE_CURRENT_SOURCE_DIR}/../../modules/native_runtime_kernel
	${CMAKE_CURRENT_BINARY_DIR}/modules/native_runtime_kernel
)

add_subdirectory(
	${CMAKE_CURRENT_SOURCE_DIR}/../../modules/native_physics_core
	${CMAKE_CURRENT_BINARY_DIR}/modules/native_physics_core
)

add_subdirectory(
	${CMAKE_CURRENT_SOURCE_DIR}/../../modules/native_world_core
	${CMAKE_CURRENT_BINARY_DIR}/modules/native_world_core
)

add_subdirectory(
	${CMAKE_CURRENT_SOURCE_DIR}/../../modules/native_ai_core
	${CMAKE_CURRENT_BINARY_DIR}/modules/native_ai_core
)

add_subdirectory(
	${CMAKE_CURRENT_SOURCE_DIR}/../../modules/native_netcode_runtime
	${CMAKE_CURRENT_BINARY_DIR}/modules/native_netcode_runtime
)

add_subdirectory(
	${CMAKE_CURRENT_SOURCE_DIR}/../../modules/native_render_runtime
	${CMAKE_CURRENT_BINARY_DIR}/modules/native_render_runtime
)

add_subdirectory(
	${CMAKE_CURRENT_SOURCE_DIR}/../../modules/native_ui_runtime
	${CMAKE_CURRENT_BINARY_DIR}/modules/native_ui_runtime
)

add_subdirectory(
	${CMAKE_CURRENT_SOURCE_DIR}/../../modules/k3h4
	${CMAKE_CURRENT_BINARY_DIR}/modules/k3h4
)

add_subdirectory(engine)

set(BANANA_NATIVE_WRAPPER_SOURCES
	scaffold/native_entry.c
)

# banana_native is the public ABI wrapper/composition boundary.
# It must not aggregate extracted module internals directly.
add_library(banana_native SHARED
	${BANANA_NATIVE_WRAPPER_SOURCES}
)
set_target_properties(banana_native PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)

target_include_directories(banana_native PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/engine
	${CMAKE_CURRENT_SOURCE_DIR}/engine/runtime
	${CMAKE_CURRENT_SOURCE_DIR}/../../modules/k3h4/native/include
)
target_link_libraries(banana_native PRIVATE
	banana_engine_core
	banana_k3h4_core
	banana_native_runtime_kernel
	banana_native_physics_core
	banana_native_world_core
	banana_native_ai_core
	banana_native_netcode_runtime
	banana_native_render_runtime
	banana_native_ui_runtime
)

if(BANANA_ENABLE_POSTGRES)
	target_compile_definitions(banana_native PRIVATE BANANA_ENABLE_POSTGRES=1)
endif()

include(GNUInstallDirs)

# Public ABI contract freeze:
# - Install path remains ${CMAKE_INSTALL_INCLUDEDIR}.
# - Header set changes require an explicit ABI versioning decision.
set(BANANA_NATIVE_PUBLIC_ABI_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR})
set(BANANA_NATIVE_PUBLIC_ABI_HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/include/banana_native_v3.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/banana_native_ui_abi.h
)

install(TARGETS banana_native
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(FILES
  ${BANANA_NATIVE_PUBLIC_ABI_HEADERS}
  DESTINATION ${BANANA_NATIVE_PUBLIC_ABI_INSTALL_DIR}
)
