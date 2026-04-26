---
description: "Tasks for v2 tests + coverage regeneration"
---

# Tasks: Tests & Coverage (v2)

## Phase 0 — Taxonomy

- [ ] T001 Write `contracts/test-taxonomy.md` covering unit/integration/e2e/native.
- [ ] T002 Cross-link from each domain quickstart.

## Phase 1 — Loud DAL gating

- [ ] T010 Coordinate with `006`: assert typed failure on missing `BANANA_PG_CONNECTION`.
- [ ] T011 [P] Replace silent skips with explicit gated markers where intentional.

## Phase 2 — Interop fake centralization

- [ ] T020 Coordinate with `007`: introduce default abstract base or codegen.
- [ ] T021 [P] Migrate existing fakes; assert ≤2 files touch when a method is added.

## Phase 3 — Artifact retention

- [ ] T030 Define retention policy (e.g., last N runs or last 30 days).
- [ ] T031 Add CI cleanup step honoring the policy.

## Phase 4 — Coverage anchor

- [ ] T040 Verify `coverage-denominator.json` path + format unchanged.
- [ ] T041 Add a CI assertion that the file exists and parses.
