# banana-native-abi vcpkg Port

This port packages the Banana native ABI surface as a standalone vcpkg package.

## What This Port Contains

- `banana_native` shared library
- Public headers from `src/native/include/`
- Small ABI manifest for discovery and auditing
- CMake config for `find_package(banana-native-abi REQUIRED CONFIG)`

## What This Port Does Not Contain

- K3H4 model data
- GitHub release assets for model bundles
- Any optional consumer model payloads

## Why It Exists

The native ABI is a separate seam from the K3H4 model package. Keeping it separate lets consumers:

- Install only the ABI layer when they need native interop
- Iterate on the ABI without repackaging K3H4 model data
- Pair multiple model versions with a single ABI version when compatible

## Usage

```json
{
  "dependencies": [
    { "name": "banana-native-abi", "version": "3.0.0" }
  ]
}
```

```cmake
find_package(banana-native-abi REQUIRED CONFIG)
target_link_libraries(my_app PRIVATE banana::native::library)
```

At runtime, consumers can point `BANANA_NATIVE_PATH` at the installed shared library or read the `INTERFACE_BANANA_NATIVE_LIBRARY_PATH` property from the imported target.

## Package Layout

```
share/banana-native-abi/
├── abi-manifest.json
├── banana-native-abi-config.cmake
└── copyright
include/
├── banana_native_v3.h
└── banana_native_ui_abi.h
lib/
└── banana_native.dll | libbanana_native.so
```

## Relationship to banana-k3h4-model

`banana-k3h4-model` and `banana-native-abi` are intentionally decoupled. A consumer can install either package independently, or both together when the model and native runtime are used in the same application.
