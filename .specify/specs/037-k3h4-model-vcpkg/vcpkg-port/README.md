# banana-k3h4-model vcpkg Port

This vcpkg port packages the Banana K3H4 model as **architecture-independent model data only** (no native library).

## What This Port Does

- Downloads the published K3H4 release tarball from GitHub Releases
- Extracts and installs **model data files only** (MODEL_METADATA.json, data/ directory, optional contracts/)
- **Explicitly excludes** native binaries (libbanana_native.so) and headers
- Creates a loader manifest with version, payload path, and native ABI compatibility hints
- Provides CMake discovery via `find_package(banana-k3h4-model)`

## Design Philosophy

This port is deliberately minimal: it contains **only the model**, not the native ABI layer.

**Why?** This enables:
- **Independent iteration**: Update K3H4 without touching native library; update native library without repackaging K3H4
- **Selective installation**: Users install only what they need
- **Clean dependencies**: Consumers who need native interop explicitly depend on a separate `banana-native-abi` port

## Usage

### Install

Declare the exact version in your vcpkg.json:

```json
{
  "dependencies": [
    { "name": "banana-k3h4-model", "version": "2026.06.14" }
  ]
}
```

Or install directly:

```bash
vcpkg install banana-k3h4-model
```

If you also need the native ABI layer, install it separately:

```json
{
  "dependencies": [
    { "name": "banana-k3h4-model", "version": "2026.06.14" },
    { "name": "banana-native-abi", "version": "3.0.0" }
  ]
}
```

### Discover and Load

Use CMake:

```cmake
find_package(banana-k3h4-model REQUIRED)

# Get the payload location
get_target_property(K3H4_PAYLOAD banana::k3h4::model INTERFACE_MODEL_PAYLOAD_LOCATION)

# Now load model data from ${K3H4_PAYLOAD}/data/...
```

Or inspect the manifest directly:

```bash
cat $(dirname $(find /usr/local -name loader-manifest.json | head -1))/loader-manifest.json
```

## Package Contents

After installation, the package contains:

```
${VCPKG_INSTALLED_DIR}/<triplet>/share/banana-k3h4-model/
├── loader-manifest.json                     ← Root discovery file
├── 2026.06.14/                              ← Versioned payload directory
│   ├── MODEL_METADATA.json                  ← Model metadata (version, schema info)
│   ├── data/                                ← Model binary data
│   │   ├── model.bin (or similar)
│   │   └── ...
│   └── contracts/                           ← Optional: format/ABI hints (if present)
├── banana-k3h4-model-config.cmake           ← CMake config for find_package()
└── copyright
```

## Loader Manifest

The `loader-manifest.json` file is advisory and contains:

```json
{
  "bundle_version": "2026.06.14",
  "payload_path": "share/banana-k3h4-model/2026.06.14",
  "payload_type": "model-only",
  "native_abi_version": "v3",
  "native_abi_required": false,
  "native_abi_guidance": "Optional: pair with banana-native-abi package for clustering queries"
}
```

**Key fields**:
- `bundle_version`: Model version (for auditing)
- `payload_path`: Relative path to model data
- `payload_type`: Always `"model-only"` for this port
- `native_abi_version`: The ABI version this model was trained with (advisory)
- `native_abi_required`: `false` — model is self-contained, but optional ABI pairing is suggested
- `native_abi_guidance`: Helpful hint for consumers

## Native ABI Integration (Optional)

If you need to run clustering queries or use native interop, depend on the separate `banana-native-abi` port:

```json
{
  "dependencies": [
    { "name": "banana-k3h4-model", "version": "2026.06.14" },
    { "name": "banana-native-abi", "version": "3.0.0" }
  ]
}
```

Then link both in your CMake:

```cmake
find_package(banana-k3h4-model REQUIRED)
find_package(banana-native-abi REQUIRED)

target_link_libraries(my_app PRIVATE banana::native::library)
```

The loader manifest's `native_abi_version` field tells you which ABI version the model expects. The model port remains data-only; `banana-native-abi` is the separate seam that exposes the shared library and headers.

## Port Strategy

The port implements a **ports-and-adapters** design:
- **Outer port** (vcpkg): Thin consumer-facing packaging layer
- **External provider adapter**: GitHub releases (existing k3h4-model-release.yml workflow)
- **Artifact adapter**: Installed versioned payload + loader manifest

## Maintenance

To update the port to a new model version:

1. Publish the new K3H4 model via the existing release workflow (`.github/workflows/k3h4-model-release.yml`)
2. Update `PORT_VERSION` in `portfile.cmake` to match the new release tag (e.g., `2026.06.15`)
3. Obtain the tarball SHA512 checksum from the release
4. Update the `SHA512` in `vcpkg_download_distfile()` call
5. Update `vcpkg.json` version to match
6. Increment `port-version` if needed
7. Test locally: `vcpkg install banana-k3h4-model`
8. Commit with a message like: `refactor(vcpkg): update banana-k3h4-model to v2026.06.15`

See [MAINTENANCE.md](MAINTENANCE.md) for checksum tracking details.

## Feature Specification

For full design context, see `.specify/specs/037-k3h4-model-vcpkg/`:
- `spec.md`: Requirements and user stories
- `plan.md`: Technical architecture
- `adr/`: Design decisions (ports-and-adapters, version pinning, etc.)

