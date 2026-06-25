# Data Model: K3H4 Model Vcpkg Packaging

## Entities

### Release Tarball

- `version`: exact bundle version string
- `source`: GitHub release asset URL
- `checksum`: published tarball hash
- `contents`: standalone K3H4 bundle files

### Loader Manifest

- `bundle_version`: exact bundle version string
- `payload_hash`: content hash or checksum of the installed payload
- `payload_path`: versioned directory path within the package
- `native_abi_version`: Banana native ABI version declared by the package
- `released_utc`: publication timestamp from the release workflow

### Versioned Payload Directory

- `version`: exact bundle version string
- `files`: extracted model payload files
- `layout`: deterministic directory structure for installed files

### Compatibility Record

- `required_native_abi_version`: ABI version that the package expects
- `compatibility_state`: compatible or incompatible
- `notes`: optional maintainer notes for consumers
