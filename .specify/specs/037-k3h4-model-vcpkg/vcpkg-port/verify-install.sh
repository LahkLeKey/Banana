#!/bin/bash
# Verify banana-k3h4-model installation

set -e

# Color output for clarity
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo "Verifying banana-k3h4-model installation..."

# Detect vcpkg installed directory (platform-specific)
if [ -z "$VCPKG_INSTALLED_DIR" ]; then
    # Try to detect from vcpkg root
    if command -v vcpkg &> /dev/null; then
        VCPKG_ROOT=$(vcpkg ls 2>/dev/null | head -1 | sed 's|/.*||')
        VCPKG_INSTALLED_DIR="${VCPKG_ROOT}/installed"
    else
        echo -e "${RED}Error: VCPKG_INSTALLED_DIR not set and vcpkg not found in PATH${NC}"
        exit 1
    fi
fi

# Determine target triplet (default: x64-linux for Linux, x64-windows for Windows)
if [ -z "$VCPKG_TARGET_TRIPLET" ]; then
    if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" ]]; then
        VCPKG_TARGET_TRIPLET="x64-windows"
    else
        VCPKG_TARGET_TRIPLET="x64-linux"
    fi
fi

PKG_DIR="${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/share/banana-k3h4-model"

echo "Target triplet: $VCPKG_TARGET_TRIPLET"
echo "Package directory: $PKG_DIR"
echo ""

# Check 1: Loader manifest exists
if [ -f "$PKG_DIR/loader-manifest.json" ]; then
    echo -e "${GREEN}✓${NC} Loader manifest found"
else
    echo -e "${RED}✗${NC} Loader manifest missing"
    exit 1
fi

# Check 2: Loader manifest is valid JSON
if jq empty "$PKG_DIR/loader-manifest.json" 2>/dev/null; then
    echo -e "${GREEN}✓${NC} Loader manifest is valid JSON"
else
    echo -e "${RED}✗${NC} Loader manifest is not valid JSON"
    exit 1
fi

# Check 3: Required fields in manifest
REQUIRED_FIELDS=("bundle_version" "payload_path" "native_abi_version" "released_utc")
for field in "${REQUIRED_FIELDS[@]}"; do
    if jq -e ".$field" "$PKG_DIR/loader-manifest.json" > /dev/null 2>&1; then
        echo -e "${GREEN}✓${NC} Manifest field '$field' present"
    else
        echo -e "${RED}✗${NC} Manifest field '$field' missing"
        exit 1
    fi
done

# Check 4: Payload directory exists
PAYLOAD_PATH=$(jq -r '.payload_path' "$PKG_DIR/loader-manifest.json")
FULL_PAYLOAD_PATH="${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/${PAYLOAD_PATH}"

if [ -d "$FULL_PAYLOAD_PATH" ]; then
    echo -e "${GREEN}✓${NC} Payload directory exists: $PAYLOAD_PATH"
else
    echo -e "${RED}✗${NC} Payload directory missing: $FULL_PAYLOAD_PATH"
    exit 1
fi

# Check 5: Native library file exists
if [ -f "$FULL_PAYLOAD_PATH/bin/libbanana_native.so" ] || \
   [ -f "$FULL_PAYLOAD_PATH/bin/libbanana_native.dll" ]; then
    echo -e "${GREEN}✓${NC} Native library artifact present"
else
    echo -e "${YELLOW}⚠${NC} Native library artifact not found (may not be applicable)"
fi

# Check 6: ABI header exists
if [ -f "$FULL_PAYLOAD_PATH/include/banana_native_v3.h" ]; then
    echo -e "${GREEN}✓${NC} ABI header present"
else
    echo -e "${YELLOW}⚠${NC} ABI header not found"
fi

# Summary
echo ""
BUNDLE_VERSION=$(jq -r '.bundle_version' "$PKG_DIR/loader-manifest.json")
ABI_VERSION=$(jq -r '.native_abi_version' "$PKG_DIR/loader-manifest.json")
echo -e "${GREEN}All checks passed!${NC}"
echo "Installed bundle: $BUNDLE_VERSION"
echo "Native ABI version: $ABI_VERSION"
