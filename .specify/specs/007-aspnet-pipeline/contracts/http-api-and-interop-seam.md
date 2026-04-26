# Contract — HTTP API & Interop Seam

## HTTP routes

- The set of routes consumed by `009-react-shared-ui`, `010-electron-desktop`, and `011-react-native-mobile` is stable across v1→v2.
- A breaking route change requires:
  1. An entry under "Breaking changes" in this contract,
  2. A coordinated update to the consuming child spec,
  3. A migration note in the parent `005-v2-regeneration/tasks.md` cutover phase.

## Status code → HTTP mapping

- `NativeStatusCode::Ok` → 200.
- `NativeStatusCode::ConfigMissing` → 503 with structured body `{ "error": "config_missing", "remediation": "…" }`.
- `NativeStatusCode::NotFound` → 404.
- `NativeStatusCode::Invalid*` → 400 with field-level detail.
- All other unmapped codes → 500 with status-code surfaced in the body for diagnosis.

(Exact mapping table maintained in `plan.md` Phase 0 inventory.)

## Interop seam

- One interface (or successor abstract base) defines all native calls.
- Adding a method MUST NOT require updating more than 2 test files.
- Fakes use a default abstract base or generated implementation.

## Pipeline context

- Typed properties only. Adding a property is a non-breaking change; removing one requires a step audit.
- Producers/consumers of each property are documented in `research.md`.

## Environment

- `BANANA_NATIVE_PATH` resolves the native shared library (preserved from v1).
- `BANANA_PG_CONNECTION` is consumed downstream by native DAL (`006`); this layer surfaces typed failures, never silent skips.

## Coverage

- `coverage-denominator.json` path and format preserved for `014-test-coverage`.
