# Vcpkg Port Layout Contract

The port installs architecture-independent K3H4 model data only.

## Layout Rules

- The port name is `banana-k3h4-model`.
- The install is exact-version pinned.
- The published release tarball is the source of truth.
- The package contains a small loader manifest and a versioned payload directory.
- The port does not rebuild the model from source during install.

## Failure Rules

- Fail if the tarball checksum does not match the release metadata.
- Fail if the manifest is missing required fields.
- Fail if the payload directory is incomplete.
