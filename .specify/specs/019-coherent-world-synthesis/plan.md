# Implementation Plan: Coherent World Synthesis Continuation

**Branch**: `019-coherent-world-synthesis` | **Date**: 2026-05-26 | **Spec**: `.specify/specs/019-coherent-world-synthesis/spec.md`

**Input**: Feature specification from `.specify/specs/019-coherent-world-synthesis/spec.md`

## Summary

Continue unifying remaining continuity slices by tightening canonical identity input enforcement, deterministic cross-entry replay lineage behavior, and additive growth safety in a single owning contract path.

## Technical Context

**Language/Version**: C11 native runtime + TypeScript API (Bun/Fastify)

**Primary Dependencies**:
- 018 coherent-world fusion baseline
- Native demo scene coherence suite
- API continuity payload/replay services and contract tests

**Storage**: Existing baseline/delta replay repositories

**Testing**:
- Focused native coherence/drift suites
- API continuity contract + integration suites
- Deterministic diagnostics and replay lineage checks

**Target Platform**: Windows native + Bun API runtime

**Constraints**:
- Preserve route -> service -> persistence separation
- Keep native changes limited to scene catalog coherence surfaces
- Keep storefront/public claims out of scope

## Constitution Check

- [x] No disclosure/storefront changes
- [x] Cross-domain contracts identified
- [x] Quality gates defined
- [x] Evidence outputs defined

## Project Structure

```text
.specify/specs/019-coherent-world-synthesis/
├── spec.md
├── plan.md
└── tasks.md
```

```text
src/native/engine/win32_dx12_poc/scene/
src/typescript/api/src/
  domains/persistent-world-orchestration/
  routes/
  integration/

tests/native/runtime/engine/
artifacts/native/019-coherent-world-synthesis/
artifacts/api/019-coherent-world-synthesis/
```

## Implementation Phases

### Phase 0: Setup + Inventory
- Seed evidence files and baseline command inventory.
- Inventory remaining synthesis seams from 018 outputs.

### Phase 1: Canonical Identity Inputs (US1)
- Tighten world identity canonicalization/reject diagnostics.
- Add native/API reject-path coverage.

### Phase 2: Replay Synthesis (US2)
- Validate mixed-entry lineage and deterministic replay outcomes.
- Expand drift diagnostics for legacy identity/route patterns.

### Phase 3: Growth Safety (US3)
- Add one new additive checkpoint field via owning contract path.
- Validate deterministic mutation and replay persistence.

### Phase 4: Hardening + Closeout
- Run focused native/API suites and capture outputs.
- Finalize spec/plan/tasks/dashboard and handoff summary.

## Validation Strategy

- Native focused: coherent profile + transition continuity/drift + continuity roundtrip
- API focused: `test:continuity` + `test:integration:continuity`

## Delivery Evidence

- `artifacts/native/019-coherent-world-synthesis/continuity-runtime-evidence.md`
- `artifacts/api/019-coherent-world-synthesis/continuity-contract-evidence.md`

## Final Command Set

- API continuity contracts:
  - `cd src/typescript/api && bun run test:continuity`
- API continuity integrations:
  - `cd src/typescript/api && bun run test:integration:continuity`
- Native focused hardening:
  - `cmake --build out/v3-native -- -m:1`
  - `ctest --test-dir out/v3-native -C Debug -R "coherent|continuity|transition|drift|roundtrip" --output-on-failure`

## Status

- Complete
- Tasks complete: `24/24`
