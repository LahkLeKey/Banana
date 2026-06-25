# Tasks: K3H4 Model Vcpkg Packaging

## 1. Define the package contract

- [ ] T001 Write the vcpkg package layout contract for `banana-k3h4-model`.
- [ ] T002 Define the loader manifest schema and required fields.
- [ ] T003 Define the exact-version and noarch package rules.

## 2. Wire the release adapter

- [ ] T004 Define how the port downloads the published GitHub release tarball.
- [ ] T005 Define checksum validation and install failure behavior.
- [ ] T006 Define the versioned payload directory layout.

## 3. Write consumer-facing documentation

- [ ] T007 Update the feature quickstart with install and verification steps.
- [ ] T008 Record the packaging decisions in ADRs and the glossary.

## 4. Validate the package surface

- [ ] T009 Verify the loader manifest exposes bundle version, payload hash, payload path, and ABI version.
- [ ] T010 Verify repeated installs of the same version produce the same layout.
- [ ] T011 Verify the port rejects mismatched tarball checksums.
