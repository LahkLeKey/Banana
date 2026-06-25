# banana-k3h4-model CMake Config
# 
# This config file exports the K3H4 model payload location for consumers.
# The model is data-only; no native ABI is bundled.
#
# Usage:
#   find_package(banana-k3h4-model REQUIRED)
#   get_target_property(MODEL_PATH banana::k3h4::model PAYLOAD_LOCATION)
#   # Now use ${MODEL_PATH} to load model data

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)

# Determine payload path from manifest or directly from share/ directory
set(K3H4_MODEL_VERSIONS "2026.06.14")  # Update when adding new versions
set(K3H4_MODEL_MANIFEST "${PACKAGE_PREFIX_DIR}/share/banana-k3h4-model/loader-manifest.json")

# Try to read manifest and extract bundle version; fall back to hardcoded list
if(EXISTS "${K3H4_MODEL_MANIFEST}")
    file(READ "${K3H4_MODEL_MANIFEST}" K3H4_MANIFEST_JSON)
    string(JSON K3H4_BUNDLE_VERSION GET "${K3H4_MANIFEST_JSON}" "bundle_version")
    string(JSON K3H4_PAYLOAD_SUBDIR GET "${K3H4_MANIFEST_JSON}" "payload_path")
    set(K3H4_MODEL_PAYLOAD_PATH "${PACKAGE_PREFIX_DIR}/${K3H4_PAYLOAD_SUBDIR}")
else()
    # Fallback: try to detect payload directory
    file(GLOB K3H4_PAYLOAD_CANDIDATES "${PACKAGE_PREFIX_DIR}/share/banana-k3h4-model/20*")
    if(K3H4_PAYLOAD_CANDIDATES)
        list(GET K3H4_PAYLOAD_CANDIDATES 0 K3H4_MODEL_PAYLOAD_PATH)
        get_filename_component(K3H4_BUNDLE_VERSION "${K3H4_MODEL_PAYLOAD_PATH}" NAME)
    else()
        message(FATAL_ERROR "banana-k3h4-model: Could not find model payload. Manifest or versioned payload directory missing.")
    endif()
endif()

# Ensure payload path exists
if(NOT EXISTS "${K3H4_MODEL_PAYLOAD_PATH}")
    message(FATAL_ERROR "banana-k3h4-model: Payload path does not exist: ${K3H4_MODEL_PAYLOAD_PATH}")
endif()

# Check for required metadata file
if(NOT EXISTS "${K3H4_MODEL_PAYLOAD_PATH}/MODEL_METADATA.json")
    message(WARNING "banana-k3h4-model: MODEL_METADATA.json not found in payload")
endif()

# Create imported target
if(NOT TARGET banana::k3h4::model)
    add_library(banana::k3h4::model INTERFACE IMPORTED)
    set_target_properties(banana::k3h4::model PROPERTIES
        INTERFACE_MODEL_PAYLOAD_LOCATION "${K3H4_MODEL_PAYLOAD_PATH}"
    )
endif()

# Expose variables for direct consumption
set(banana-k3h4-model_FOUND TRUE)
set(banana-k3h4-model_BUNDLE_VERSION "${K3H4_BUNDLE_VERSION}")
set(banana-k3h4-model_PAYLOAD_PATH "${K3H4_MODEL_PAYLOAD_PATH}")
set(banana-k3h4-model_MANIFEST_PATH "${K3H4_MODEL_MANIFEST}")

# Print diagnostic info if requested
if(banana-k3h4-model_FIND_VERBOSE)
    message(STATUS "banana-k3h4-model: Bundle version ${K3H4_BUNDLE_VERSION}")
    message(STATUS "banana-k3h4-model: Payload path ${K3H4_MODEL_PAYLOAD_PATH}")
endif()
