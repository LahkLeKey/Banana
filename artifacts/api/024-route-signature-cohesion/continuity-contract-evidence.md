# API Continuity Contract Evidence — 024 Route Signature Cohesion

## Baseline Commands

- `cd src/typescript/api && bun run test:continuity`
- `cd src/typescript/api && bun run test:integration:continuity`

## US1 Evidence — Canonical Checkpoint Route Signature Tag

- Unit: `rejects non-canonical checkpoint route signature tag formatting` — passing.
- Route: `rejects continuity payload with non-canonical checkpoint route signature diagnostics` — passing.
- Drift integration: `rejects non-canonical checkpoint route signature with deterministic diagnostics` — passing.

## US2 Evidence — Replay Determinism Under Route Signature Drift

- Roundtrip integration: `invalid route-signature retries do not break authoritative replay once canonical commit succeeds` — passing; canonical commit lands at `areaStateVersion: 1`.

## US3 Evidence — Additive Attunement Lane

- Unit: `additive checkpoint attunement lane field mutates signature deterministically` — passing.
- Growth-path integration round-trips `checkpointAttunementLaneTag` — passing.

## Final Validation Summary

- `cd src/typescript/api && bun run test:continuity` → 46 pass / 0 fail / 86 expect() calls.
- `cd src/typescript/api && bun run test:integration:continuity` → 21 pass / 0 fail / 201 expect() calls.
