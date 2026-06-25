vcpkg_minimum_required(VERSION 2024-01-01)

# banana-k3h4-model: vcpkg port for standalone K3H4 model artifact
# 
# This port downloads the published GitHub release tarball from the Banana K3H4
# release workflow and installs model data only (no native library).
#
# The port is a thin data-only adapter over the existing standalone release pipeline.
# All native ABI files (libbanana_native.so, headers) are excluded.
# Consumers who need native interop should depend on a separate banana-native-abi port.

vcpkg_minimum_required(VERSION 2024-01-01)

# Version strategy: exact pinning.
# Consumers must specify the exact model version they need.
set(PORT_VERSION "2026.06.14")

# GitHub release download details
set(GITHUB_OWNER "LahkLeKey")
set(GITHUB_REPO "Banana")
set(RELEASE_TAG "k3h4-model-v${PORT_VERSION}")

# Tarball filename matches the release workflow
set(TARBALL_NAME "k3h4-model-${PORT_VERSION}.tar.gz")

# GitHub releases URL
set(GITHUB_RELEASE_URL "https://github.com/${GITHUB_OWNER}/${GITHUB_REPO}/releases/download/${RELEASE_TAG}")
set(TARBALL_URL "${GITHUB_RELEASE_URL}/${TARBALL_NAME}")

# Download the release tarball
vcpkg_download_distfile(
    TARBALL
    URLS "${TARBALL_URL}"
    FILENAME "${TARBALL_NAME}"
    SHA512 "PLACEHOLDER_SHA512_CHECKSUM"
)

# Extract the tarball to the staging directory
file(
    ARCHIVE_EXTRACT INPUT "${TARBALL}"
    DESTINATION "${CURRENT_BUILDTREES_DIR}/extracted"
)

# Verify the extracted structure contains required model files
set(EXTRACTED_ROOT "${CURRENT_BUILDTREES_DIR}/extracted/k3h4-model-${PORT_VERSION}")

if(NOT EXISTS "${EXTRACTED_ROOT}/MODEL_METADATA.json")
    message(FATAL_ERROR "K3H4 release tarball missing MODEL_METADATA.json")
endif()

# Require that model data directory exists (data/ or similar; varies by release)
file(GLOB MODEL_DATA_FILES "${EXTRACTED_ROOT}/data/*" "${EXTRACTED_ROOT}/*.bin")
if(NOT MODEL_DATA_FILES)
    message(FATAL_ERROR "K3H4 release tarball missing model data files (expected data/ directory or .bin files)")
endif()

# Create the versioned payload directory in the package root
set(PAYLOAD_DIR "${CURRENT_PACKAGES_DIR}/share/banana-k3h4-model/${PORT_VERSION}")
file(MAKE_DIRECTORY "${PAYLOAD_DIR}")

# Copy ONLY model data and metadata; explicitly exclude native files
# This ensures the package remains architecture-independent and decoupled from ABI updates
file(
    COPY "${EXTRACTED_ROOT}/MODEL_METADATA.json"
    DESTINATION "${PAYLOAD_DIR}"
)

# Copy model data directory (if exists)
if(EXISTS "${EXTRACTED_ROOT}/data")
    file(
        COPY "${EXTRACTED_ROOT}/data/"
        DESTINATION "${PAYLOAD_DIR}/data"
    )
endif()

# Copy optional contracts directory (if exists) — useful for format documentation
if(EXISTS "${EXTRACTED_ROOT}/contracts")
    file(
        COPY "${EXTRACTED_ROOT}/contracts/"
        DESTINATION "${PAYLOAD_DIR}/contracts"
    )
endif()

# Verify no native binaries were copied (safety check)
if(EXISTS "${PAYLOAD_DIR}/bin/libbanana_native.so" OR
   EXISTS "${PAYLOAD_DIR}/include/banana_native_v3.h" OR
   EXISTS "${PAYLOAD_DIR}/lib/libbanana_native.so" OR
   EXISTS "${PAYLOAD_DIR}/lib/libbanana_native.dll")
    message(FATAL_ERROR "Native library files detected in K3H4 model payload. Port must be model-only.")
endif()

# Create the loader manifest at the package root
# The manifest is advisory: it declares the model's expected native ABI version,
# but consumers are responsible for explicitly depending on banana-native-abi if needed.
file(
    WRITE "${CURRENT_PACKAGES_DIR}/share/banana-k3h4-model/loader-manifest.json"
    "{\n"
    "  \"bundle_version\": \"${PORT_VERSION}\",\n"
    "  \"payload_path\": \"share/banana-k3h4-model/${PORT_VERSION}\",\n"
    "  \"payload_type\": \"model-only\",\n"
    "  \"native_abi_version\": \"v3\",\n"
    "  \"native_abi_required\": false,\n"
    "  \"native_abi_guidance\": \"Optional: pair with banana-native-abi package for clustering queries\",\n"
    "  \"released_utc\": \"$(date -u +\"%Y-%m-%dT%H:%M:%SZ\")\"\n"
    "}\n"
)

# Create a copyright file (vcpkg requirement)
file(
    WRITE "${CURRENT_PACKAGES_DIR}/share/banana-k3h4-model/copyright"
    "K3H4 Model is part of the Banana game engine.\n"
    "This package contains model data only (no native code).\n"
    "See the Banana repository for full license information.\n"
)

# Install CMake config file for consumer discovery
file(
    WRITE "${CURRENT_PACKAGES_DIR}/share/banana-k3h4-model/banana-k3h4-model-config.cmake"
    "# banana-k3h4-model CMake config\n"
    "# Exposes the K3H4 model payload location for consumers\n\n"
    "get_filename_component(PACKAGE_PREFIX_DIR \"\${CMAKE_CURRENT_LIST_DIR}/../../\" ABSOLUTE)\n"
    "set(K3H4_MODEL_PAYLOAD_PATH \"\${PACKAGE_PREFIX_DIR}/share/banana-k3h4-model/${PORT_VERSION}\")\n"
    "set(K3H4_MODEL_MANIFEST \"\${PACKAGE_PREFIX_DIR}/share/banana-k3h4-model/loader-manifest.json\")\n\n"
    "# Create imported target for consumers\n"
    "if(NOT TARGET banana::k3h4::model)\n"
    "    add_library(banana::k3h4::model INTERFACE IMPORTED)\n"
    "    set_target_properties(banana::k3h4::model PROPERTIES\n"
    "        INTERFACE_INCLUDE_DIRECTORIES \"\${K3H4_MODEL_PAYLOAD_PATH}\"\n"
    "    )\n"
    "endif()\n"
)

# Disable package configuration for data-only packages
set(VCPKG_POLICY_EMPTY_PACKAGE enabled)

# Mark as successful
vcpkg_install_copyright(FILE_LIST "${CURRENT_PACKAGES_DIR}/share/banana-k3h4-model/copyright")

