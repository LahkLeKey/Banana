# ADR 0002: Use a versioned payload directory plus a top-level loader manifest

Status: Accepted

## Context

The package should expose enough metadata for consumers to discover the installed model version, integrity hash, and compatibility information without manually inspecting release files.

The model package must remain noarch, use exact version pins, and expose the Banana native ABI version explicitly so compatibility checks are deterministic.

## Decision

`banana-k3h4-model` will install as:

- a small top-level loader manifest at the package root
- a versioned payload directory under the package root
- architecture-independent data only

The manifest will record:

- the bundle version
- the tarball hash or content hash
- the Banana native ABI version
- the payload path

## Consequences

- Multiple versions can coexist without path ambiguity.
- Consumers can validate compatibility before loading anything.
- Install paths remain stable and predictable.
- The package does not promise a binary runtime; it promises model data and metadata.

## Rationale

This layout keeps the install root clean while preserving provenance. It also makes the compatibility policy explicit instead of hiding it behind the package version alone.
