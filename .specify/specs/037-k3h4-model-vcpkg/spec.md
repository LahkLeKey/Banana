# Feature Specification: K3H4 Model Vcpkg Packaging

**Feature Branch**: `feat/037-k3h4-model-vcpkg`

**Status**: Draft

**Input**: User request: publish the K3H4 model as its own vcpkg package so other users can install the model directly.

## User Scenarios & Testing

### User Story 1 - Install the K3H4 model directly

As a Banana consumer, I want to install the K3H4 model as a standalone vcpkg package so I can consume model data without pulling in unrelated workspace packages.

**Independent Test**: Run `vcpkg install banana-k3h4-model:<triplet>` for an exact version pin and verify the package installs without building native code.

### User Story 2 - Verify compatibility before loading

As a Banana runtime maintainer, I want the package manifest to declare the native ABI version explicitly so compatibility checks are deterministic before the model is loaded.

**Independent Test**: Inspect the installed loader manifest and verify it contains the bundle version, content hash, payload path, and ABI version.

### User Story 3 - Keep release and packaging logic separate

As a maintainer, I want the vcpkg port to download the published GitHub release tarball so release assembly stays centralized in the existing standalone release workflow.

**Independent Test**: Re-run the vcpkg port with the same pinned version and confirm it consumes the published release artifact rather than rebuilding the model from source.

## Requirements

- **FR-001**: The package MUST be named `banana-k3h4-model`.
- **FR-002**: The package MUST be noarch data only.
- **FR-003**: The package MUST install from an exact version pin.
- **FR-004**: The vcpkg port MUST download the published GitHub release tarball produced by the K3H4 standalone release workflow.
- **FR-005**: The installed package MUST include a small top-level loader manifest.
- **FR-006**: The installed package MUST place payload files in a versioned payload directory.
- **FR-007**: The loader manifest MUST expose the bundle version, payload hash, payload path, and Banana native ABI version.
- **FR-008**: Consumers MUST be able to validate compatibility from the manifest without inspecting the release tarball layout directly.
- **FR-009**: The port MUST fail installation if the tarball checksum does not match the published release metadata.
- **FR-010**: The port MUST keep package layout deterministic across installs.

## Key Entities

- **Release Tarball**: The published GitHub archive produced by the existing K3H4 release workflow.
- **Loader Manifest**: Small metadata file that tells consumers the bundle version, payload hash, payload path, and ABI version.
- **Versioned Payload Directory**: Directory containing the actual model payload files for one exact release version.
- **Compatibility Record**: The manifest fields that allow a consumer to validate ABI compatibility before loading.

## Success Criteria

- **SC-001**: A consumer can install `banana-k3h4-model` with an exact version pin and find the payload without manual release reconstruction.
- **SC-002**: The installed loader manifest always exposes the exact bundle version and native ABI version.
- **SC-003**: The vcpkg port downloads the published tarball and does not rebuild the model artifact from source.
- **SC-004**: Two installs of the same version produce the same visible layout and manifest fields.

## Assumptions

- The existing standalone K3H4 release workflow remains the authoritative producer of release tarballs.
- The model payload is architecture-independent data.
- Consumers prefer deterministic installs over floating or auto-updating versions.
