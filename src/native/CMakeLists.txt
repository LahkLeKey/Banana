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
	${CMAKE_CURRENT_SOURCE_DIR}/../../modules/k3h4
	${CMAKE_CURRENT_BINARY_DIR}/modules/k3h4
)

add_subdirectory(engine)

add_library(banana_native SHARED
	scaffold/native_entry.c
)
set_target_properties(banana_native PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)

target_include_directories(banana_native PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/engine
	${CMAKE_CURRENT_SOURCE_DIR}/engine/runtime
	${CMAKE_CURRENT_SOURCE_DIR}/../../modules/k3h4/native/include
	${CMAKE_CURRENT_SOURCE_DIR}/../../modules/k3h4/native/src
)
target_link_libraries(banana_native PRIVATE banana_engine_core banana_k3h4_core)

if(BANANA_ENABLE_POSTGRES)
	target_compile_definitions(banana_native PRIVATE BANANA_ENABLE_POSTGRES=1)
endif()

include(GNUInstallDirs)

install(TARGETS banana_native
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(FILES
	${CMAKE_CURRENT_SOURCE_DIR}/include/banana_native_v3.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/banana_native_ui_abi.h
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
