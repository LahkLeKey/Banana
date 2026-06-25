# Loader Manifest Contract

The loader manifest is the small top-level metadata file installed with `banana-k3h4-model`.

## Required Fields

- `bundle_version`
- `payload_hash`
- `payload_path`
- `native_abi_version`
- `released_utc`

## Invariants

- The bundle version is exact and pinned.
- The payload path is versioned and deterministic.
- The ABI version is explicit and not inferred from the bundle version.
- The manifest must be readable without inspecting the release tarball layout.
