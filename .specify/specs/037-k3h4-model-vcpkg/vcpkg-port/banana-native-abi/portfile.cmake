vcpkg_minimum_required(VERSION 2024-01-01)

# banana-native-abi: vcpkg port for the Banana native ABI library.
#
# This package exposes the shared library and public ABI header used by
# consumers that need BANANA_NATIVE_PATH-compatible native interop.
# It is separate from banana-k3h4-model so model iteration and ABI iteration
# can proceed independently.

set(PORT_VERSION "3.0.0")
set(SOURCE_ROOT "${CURRENT_PORT_DIR}/../../../../../src/native")

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_ROOT}"
    OPTIONS
        -DBANANA_ENABLE_POSTGRES=OFF
        -DBANANA_ENABLE_DETERMINISTIC_PERSISTENCE_TESTS=OFF
        -DBANANA_ENABLE_COVERAGE=OFF
)

vcpkg_cmake_build(
    TARGET banana_native
)

set(PACKAGE_INCLUDE_DIR "${CURRENT_PACKAGES_DIR}/include")
set(PACKAGE_LIB_DIR "${CURRENT_PACKAGES_DIR}/lib")
set(PACKAGE_SHARE_DIR "${CURRENT_PACKAGES_DIR}/share/banana-native-abi")

file(MAKE_DIRECTORY "${PACKAGE_INCLUDE_DIR}" "${PACKAGE_LIB_DIR}" "${PACKAGE_SHARE_DIR}")

# Install the public ABI header.
file(COPY "${SOURCE_ROOT}/include/banana_native_v3.h" DESTINATION "${PACKAGE_INCLUDE_DIR}")
file(COPY "${SOURCE_ROOT}/include/banana_native_ui_abi.h" DESTINATION "${PACKAGE_INCLUDE_DIR}")

# Copy the built shared library from the build tree.
if(WIN32)
    file(GLOB_RECURSE BUILT_SHARED_LIBS
        "${CURRENT_BUILDTREES_DIR}/*/banana_native.dll"
        "${CURRENT_BUILDTREES_DIR}/*/banana_native.pdb"
    )
else()
    file(GLOB_RECURSE BUILT_SHARED_LIBS
        "${CURRENT_BUILDTREES_DIR}/*/libbanana_native.so"
    )
endif()

list(FILTER BUILT_SHARED_LIBS EXCLUDE REGEX "[/\\]CMakeFiles[/\\]")
list(LENGTH BUILT_SHARED_LIBS BUILT_SHARED_LIB_COUNT)
if(BUILT_SHARED_LIB_COUNT EQUAL 0)
    message(FATAL_ERROR "banana-native-abi: built shared library not found in ${CURRENT_BUILDTREES_DIR}")
endif()

list(GET BUILT_SHARED_LIBS 0 BUILT_SHARED_LIB)
file(COPY "${BUILT_SHARED_LIB}" DESTINATION "${PACKAGE_LIB_DIR}")

if(WIN32)
    file(GLOB_RECURSE BUILT_PDBS "${CURRENT_BUILDTREES_DIR}/*/banana_native.pdb")
    list(FILTER BUILT_PDBS EXCLUDE REGEX "[/\\]CMakeFiles[/\\]")
    list(LENGTH BUILT_PDBS BUILT_PDB_COUNT)
    if(BUILT_PDB_COUNT GREATER 0)
        list(GET BUILT_PDBS 0 BUILT_PDB)
        file(COPY "${BUILT_PDB}" DESTINATION "${PACKAGE_LIB_DIR}")
    endif()
endif()

# Write a small manifest that records the ABI package version and install layout.
file(WRITE "${PACKAGE_SHARE_DIR}/abi-manifest.json" "{\n")
file(APPEND "${PACKAGE_SHARE_DIR}/abi-manifest.json" "  \"package\": \"banana-native-abi\",\n")
file(APPEND "${PACKAGE_SHARE_DIR}/abi-manifest.json" "  \"bundle_version\": \"${PORT_VERSION}\",\n")
file(APPEND "${PACKAGE_SHARE_DIR}/abi-manifest.json" "  \"native_abi_version\": \"v3\",\n")
file(APPEND "${PACKAGE_SHARE_DIR}/abi-manifest.json" "  \"library_name\": \"banana_native\",\n")
file(APPEND "${PACKAGE_SHARE_DIR}/abi-manifest.json" "  \"include_dir\": \"include\"\n")
file(APPEND "${PACKAGE_SHARE_DIR}/abi-manifest.json" "}\n")

# Install the vcpkg config shim.
file(COPY "${CURRENT_PORT_DIR}/banana-native-abi-config.cmake" DESTINATION "${PACKAGE_SHARE_DIR}")
file(COPY "${CURRENT_PORT_DIR}/copyright" DESTINATION "${PACKAGE_SHARE_DIR}")

# Keep this package lean: it should export the ABI surface only.
set(VCPKG_POLICY_EMPTY_PACKAGE enabled)
