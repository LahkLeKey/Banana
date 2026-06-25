# Implementation Plan: K3H4 Model Vcpkg Packaging

**Branch**: `feat/037-k3h4-model-vcpkg` | **Spec**: `.specify/specs/037-k3h4-model-vcpkg/spec.md`

## Summary

Package the standalone K3H4 model as `banana-k3h4-model`, distributed as architecture-independent data through vcpkg. The port downloads the published GitHub release tarball, installs a versioned payload directory, and writes a small loader manifest that exposes the exact bundle version, payload hash, payload path, and Banana native ABI version.

## Technical Context

**Language/Version**: vcpkg portfile CMake + packaging metadata, Markdown docs for feature artifacts

**Primary Dependencies**:
- Existing standalone K3H4 release workflow in `scripts/build-k3h4-standalone-release.sh`
- Existing GitHub release workflow in `.github/workflows/k3h4-model-release.yml`
- vcpkg port layout and `portfile.cmake` conventions

**Storage**: vcpkg installed package tree, release tarball cache, and package metadata files

**Testing**:
- Port install smoke test for exact-version installation
- Tarball checksum verification
- Manifest presence and field validation

**Target Platform**: Windows and Linux consumers using vcpkg

**Project Type**: Packaging feature spanning release automation, package layout, and consumer metadata

## Constitution Check

- Package distribution remains data-only and does not introduce a new runtime binary surface.
- Exact version pinning keeps compatibility explicit.
- The release workflow remains authoritative; vcpkg acts as an adapter.
- The loader manifest provides the compatibility seam for consumers.

## Project Structure

### Documentation

```text
.specify/specs/037-k3h4-model-vcpkg/
├── spec.md
├── plan.md
├── research.md
├── data-model.md
├── quickstart.md
├── tasks.md
└── contracts/
    ├── loader-manifest.md
    └── port-layout.md
```

### Packaging Surface

```text
vcpkg port for banana-k3h4-model
├── downloads published GitHub release tarball
├── installs versioned payload files
├── writes loader manifest
└── exposes noarch model data only
```

## Delivery Strategy

1. Define the package contract and visible install layout.
2. Define the loader manifest fields and compatibility rules.
3. Write the vcpkg port to consume the published tarball.
4. Add verification steps for checksum, version pinning, and manifest contents.
