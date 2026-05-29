# Implementation Plan: Coherent World Convergence Continuation

**Branch**: `020-coherent-world-convergence` | **Date**: 2026-05-26 | **Spec**: `.specify/specs/020-coherent-world-convergence/spec.md`

**Input**: Feature specification from `.specify/specs/020-coherent-world-convergence/spec.md`

## Summary

Continue coherent-world synthesis by enforcing canonical checkpoint context identity, validating replay determinism under context-drift retries, and adding one safe additive continuity checkpoint attribute through the owning API contract path.

## Technical Context

**Language/Version**: C11 native runtime + TypeScript API (Bun/Fastify)

**Primary Dependencies**:
- 019 coherent-world synthesis baseline
- Native demo scene coherence/transition suite
- API continuity payload and replay services

**Storage**: Existing baseline/delta replay repositories

**Testing**:
- Focused native coherence/transition suites
- API continuity contract and integration suites
- Deterministic diagnostics/replay lineage assertions

**Target Platform**: Windows native + Bun API runtime

**Constraints**:
- Preserve route -> service -> persistence boundaries
- Keep native changes scoped to continuity/coherence seams only when needed
- Keep storefront/public claims out of scope

## Constitution Check

- [x] No disclosure/storefront changes
- [x] Cross-domain contracts identified
- [x] Quality gates defined
- [x] Evidence outputs defined

## Project Structure

```text
.specify/specs/020-coherent-world-convergence/
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
artifacts/native/020-coherent-world-convergence/
artifacts/api/020-coherent-world-convergence/
```

## Implementation Phases

### Phase 0: Setup + Inventory
- Seed evidence docs and baseline validation command inventory.
- Inventory context canonicalization seams from 019 outputs.

### Phase 1: Canonical Context Inputs (US1)
- Tighten checkpoint context canonicalization/reject diagnostics.
- Add API contract/integration reject-path coverage.

### Phase 2: Replay Determinism (US2)
- Validate replay lineage remains authoritative after context-drift retries.
- Expand deterministic diagnostics for context-drift retry family.

### Phase 3: Growth Safety (US3)
- Add one additive continuity checkpoint field through owning contract path.
- Validate deterministic mutation and replay persistence behavior.

### Phase 4: Hardening + Closeout
- Run focused native/API suites and capture outputs.
- Finalize spec/plan/tasks/dashboard and handoff summary.

## Validation Strategy

- Native focused: coherent profile + transition continuity/drift + continuity roundtrip
- API focused: `test:continuity` + `test:integration:continuity`

## Delivery Evidence

- `artifacts/native/020-coherent-world-convergence/continuity-runtime-evidence.md`
- `artifacts/api/020-coherent-world-convergence/continuity-contract-evidence.md`

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
