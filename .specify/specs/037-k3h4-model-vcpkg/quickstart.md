# Quickstart: K3H4 Model Vcpkg Packaging

## Install

1. Pin the package version in your consumer manifest.
2. Run vcpkg install for `banana-k3h4-model`.
3. Inspect the installed loader manifest.
4. Validate the reported native ABI version before loading the payload.

## What to Verify

- The install did not rebuild the model from source.
- The loader manifest exists.
- The manifest reports the expected bundle version.
- The manifest reports the expected Banana native ABI version.
- The payload lives in the versioned payload directory.

## Evidence

- Installed package tree
- Loader manifest contents
- Checksum verification result
