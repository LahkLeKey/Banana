# banana-native-abi CMake config
#
# Exposes the Banana native ABI shared library and header include path.
# Consumers can link banana::native::library and use BANANA_NATIVE_PATH at runtime.

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)

set(BANANA_NATIVE_ABI_VERSION "3.0.0")
set(BANANA_NATIVE_INCLUDE_DIR "${PACKAGE_PREFIX_DIR}/include")

if(WIN32)
    set(BANANA_NATIVE_LIBRARY_PATH "${PACKAGE_PREFIX_DIR}/bin/banana_native.dll")
    set(BANANA_NATIVE_IMPORT_LIBRARY_PATH "${PACKAGE_PREFIX_DIR}/lib/banana_native.lib")
else()
    set(BANANA_NATIVE_LIBRARY_PATH "${PACKAGE_PREFIX_DIR}/lib/libbanana_native.so")
endif()

if(NOT EXISTS "${BANANA_NATIVE_LIBRARY_PATH}")
    message(FATAL_ERROR "banana-native-abi: native library not found at ${BANANA_NATIVE_LIBRARY_PATH}")
endif()

if(NOT TARGET banana::native::library)
    add_library(banana::native::library SHARED IMPORTED)
    set_target_properties(banana::native::library PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${BANANA_NATIVE_INCLUDE_DIR}"
        IMPORTED_LOCATION "${BANANA_NATIVE_LIBRARY_PATH}"
    )
    if(WIN32)
        set_target_properties(banana::native::library PROPERTIES
            IMPORTED_IMPLIB "${BANANA_NATIVE_IMPORT_LIBRARY_PATH}"
        )
    endif()
endif()

set(banana-native-abi_FOUND TRUE)
set(banana-native-abi_VERSION "${BANANA_NATIVE_ABI_VERSION}")
set(banana-native-abi_INCLUDE_DIR "${BANANA_NATIVE_INCLUDE_DIR}")
set(banana-native-abi_LIBRARY_PATH "${BANANA_NATIVE_LIBRARY_PATH}")
