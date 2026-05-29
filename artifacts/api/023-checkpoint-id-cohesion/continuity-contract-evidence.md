# API Continuity Contract Evidence — 023 Checkpoint Id Cohesion

## Baseline Commands

- `cd src/typescript/api && bun run test:continuity`
- `cd src/typescript/api && bun run test:integration:continuity`

## US1 Evidence — Canonical Checkpoint Id Formatting

- `continuity payload service > rejects non-canonical checkpoint id formatting` — passing.
- Route: `world continuity checkpoint contract > rejects continuity payload with non-canonical checkpoint id diagnostics` — passing.
- Route: `world continuity checkpoint contract > rejects repeated non-canonical checkpoint id payloads deterministically` — passing.
- Drift integration: `rejects non-canonical checkpoint id with deterministic diagnostics` — passing.

## US2 Evidence — Replay Determinism Under Checkpoint Id Drift

- Roundtrip integration: `invalid checkpoint-id retries do not break authoritative replay once canonical commit succeeds` — passing; canonical commit lands at `areaStateVersion: 1` after drifted retries.

## US3 Evidence — Additive Observation Lane

- `continuity payload service > additive checkpoint observation lane field mutates signature deterministically` — passing.
- Growth-path integration: `new checkpoint context field persists and rehydrates through owning contract path` — passing with `checkpointObservationLaneTag` round-tripped.

## Final Validation Summary

- `cd src/typescript/api && bun run test:continuity` → 43 pass / 0 fail / 81 expect() calls.
- `cd src/typescript/api && bun run test:integration:continuity` → 19 pass / 0 fail / 191 expect() calls.
