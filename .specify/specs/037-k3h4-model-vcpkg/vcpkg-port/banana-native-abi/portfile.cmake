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

vcpkg_cmake_install()

vcpkg_copy_pdbs()

set(PACKAGE_INCLUDE_DIR "${CURRENT_PACKAGES_DIR}/include")
set(PACKAGE_LIB_DIR "${CURRENT_PACKAGES_DIR}/lib")
set(PACKAGE_SHARE_DIR "${CURRENT_PACKAGES_DIR}/share/banana-native-abi")

file(MAKE_DIRECTORY "${PACKAGE_INCLUDE_DIR}" "${PACKAGE_LIB_DIR}" "${PACKAGE_SHARE_DIR}")

# Write a small manifest that records the ABI package version and install layout.
file(WRITE "${PACKAGE_SHARE_DIR}/abi-manifest.json" "{\n")
file(APPEND "${PACKAGE_SHARE_DIR}/abi-manifest.json" "  \"package\": \"banana-native-abi\",\n")
file(APPEND "${PACKAGE_SHARE_DIR}/abi-manifest.json" "  \"bundle_version\": \"${PORT_VERSION}\",\n")
file(APPEND "${PACKAGE_SHARE_DIR}/abi-manifest.json" "  \"native_abi_version\": \"v3\",\n")
file(APPEND "${PACKAGE_SHARE_DIR}/abi-manifest.json" "  \"library_name\": \"banana_native\",\n")
file(APPEND "${PACKAGE_SHARE_DIR}/abi-manifest.json" "  \"include_dir\": \"include\",\n")
if(WIN32)
    file(APPEND "${PACKAGE_SHARE_DIR}/abi-manifest.json" "  \"library_path\": \"bin/banana_native.dll\",\n")
    file(APPEND "${PACKAGE_SHARE_DIR}/abi-manifest.json" "  \"import_library_path\": \"lib/banana_native.lib\"\n")
else()
    file(APPEND "${PACKAGE_SHARE_DIR}/abi-manifest.json" "  \"library_path\": \"lib/libbanana_native.so\"\n")
endif()
file(APPEND "${PACKAGE_SHARE_DIR}/abi-manifest.json" "}\n")

# Install the vcpkg config shim.
file(COPY "${CURRENT_PORT_DIR}/banana-native-abi-config.cmake" DESTINATION "${PACKAGE_SHARE_DIR}")
file(COPY "${CURRENT_PORT_DIR}/copyright" DESTINATION "${PACKAGE_SHARE_DIR}")

# Keep this package lean: it should export the ABI surface only.
set(VCPKG_POLICY_EMPTY_PACKAGE enabled)
