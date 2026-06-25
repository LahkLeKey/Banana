# Flexible K3H4 VCpkg Port Design: Multi-Mode Delivery

## Consumer Modes

Consumers request modes via **feature flags** in `vcpkg.json`:

```json
{
  "dependencies": [
    { "name": "banana-k3h4-model", "version": "2026.06.14" },
    { "name": "banana-k3h4-model", "version": "2026.06.14", "features": ["model-native"] },
    { "name": "banana-k3h4-model", "version": "2026.06.14", "features": ["model-abi-external"] }
  ]
}
```

| Mode | Feature | What's Installed | Use Case |
|------|---------|------------------|----------|
| **A** | `(none)` | Model data only | Lightweight embeds; native is external |
| **B** | `model-native` | Model + libbanana_native.so + headers | Self-contained deployment |
| **C** | `model-abi-external` | Model + ABI contracts; depends on `banana-native-abi` | Pluggable architecture |

## vcpkg.json Structure

```json
{
  "name": "banana-k3h4-model",
  "version": "0.1.0",
  "features": {
    "model-native": {
      "description": "Include native library and headers",
      "dependencies": []
    },
    "model-abi-external": {
      "description": "Include ABI metadata; expect external banana-native-abi",
      "dependencies": [ { "name": "banana-native-abi", "version": "3.0" } ]
    }
  },
  "default-features": []
}
```

## portfile.cmake Conditional Logic

```cmake
# Determine delivery mode from feature flags
set(MODE_NATIVE OFF)
set(MODE_ABI_EXTERNAL OFF)
if("model-native" IN_LIST FEATURES)
  set(MODE_NATIVE ON)
endif()
if("model-abi-external" IN_LIST FEATURES)
  set(MODE_ABI_EXTERNAL ON)
endif()

# Always install model data
file(COPY "${EXTRACTED_ROOT}/data" DESTINATION "${PAYLOAD_DIR}")
file(COPY "${EXTRACTED_ROOT}/MODEL_METADATA.json" DESTINATION "${PAYLOAD_DIR}")

# Mode B: Include native library and headers
if(MODE_NATIVE)
  file(COPY "${EXTRACTED_ROOT}/bin/libbanana_native.so" 
       DESTINATION "${CURRENT_PACKAGES_DIR}/lib")
  file(COPY "${EXTRACTED_ROOT}/include/banana_native_v3.h" 
       DESTINATION "${CURRENT_PACKAGES_DIR}/include")
endif()

# Modes B and C: Include ABI contracts
if(MODE_NATIVE OR MODE_ABI_EXTERNAL)
  file(COPY "${EXTRACTED_ROOT}/contracts" 
       DESTINATION "${PAYLOAD_DIR}")
endif()

# Write mode metadata to loader manifest
file(WRITE "${PAYLOAD_DIR}/install-mode.json" 
  "{\"native_bundled\": ${MODE_NATIVE}, \"abi_external\": ${MODE_ABI_EXTERNAL}}")
```

## Package Layout (All Modes)

```
share/banana-k3h4-model/
├── 2026.06.14/
│   ├── data/*.bin
│   ├── MODEL_METADATA.json
│   ├── contracts/ (if Mode B or C)
│   └── install-mode.json
└── loader-manifest.json
lib/ (if Mode B)
├── libbanana_native.so
include/ (if Mode B)
└── banana_native_v3.h
```

## Runtime Discovery

The loader manifest advertises mode:

```json
{
  "bundle_version": "2026.06.14",
  "payload_path": "share/banana-k3h4-model/2026.06.14",
  "native_abi_version": "v3",
  "install_mode": {
    "native_bundled": false,
    "abi_external": true,
    "model_only": false
  }
}
```

CMake config: `find_package(banana-k3h4-model)` exports mode-specific targets:
- Mode A: `k3h4::data`
- Mode B: `k3h4::data`, `k3h4::native`
- Mode C: `k3h4::data`, `k3h4::abi-metadata`

Consumers inspect `banana-k3h4-model_INSTALL_MODE` to adapt link/include flags.

## Trade-offs

**Flexibility gains**: Support 3+ delivery patterns; allow feature expansion (tools, schemas).

**Complexity costs**: portfile logic explodes; consumer must understand modes; CI tests 3× paths; mismatched features (e.g., requiring `model-abi-external` but not `banana-native-abi`) fail at link time, not install time.

**Mitigation**: Feature dependencies catch mode mismatches; documentation and CMake target names make intent explicit.
