# Concern-Split Checklist

Use this checklist to prove feature 005 keeps native runtime/render files small, explicit, and reviewable.

## Rule

- Native implementation files should be small and single-responsibility.
- `CMakeLists.txt` is the only standing large-file exception.
- Any other large native implementation file must record an owner, rationale, and follow-up split slice.

## Acceptance Mapping

- FR-001: runtime/render modularization produces smaller, explicit seams.
- FR-009: oversized files are either split, reduced to coordinators, or documented as temporary exceptions.
- SC-003: review artifacts show where responsibilities moved and which files still need follow-up.

## In-Scope Files

| File | Primary Concern | Current State | Evidence | Owner / Follow-up |
|------|-----------------|---------------|----------|-------------------|
| `src/native/engine/runtime/engine_tick.c` | Tick coordination | Reduced to thin coordinator | Input, budget, and post phases extracted | Continue via US1 orchestration adapter tasks |
| `src/native/engine/runtime/engine_composition.c` | Composition root wiring | Reduced to thinner coordinator | Explicit service-port state wiring extracted | Continue slimming via S1 |
| `src/native/engine/render/backend_dx12.c` | DX12 backend transport + diagnostics | Temporary exception | Projection policy extracted; diagnostics still partially local | Split via US2 / T018-T021 |
| `src/native/engine/win32_dx12_poc/main.c` | POC host shell | Temporary exception | Objective and scene policies extracted | Split via S4 |
| `src/native/engine/render/renderer.c` | Backend-agnostic render coordination | Monitor | No active exception yet | Reassess after DX12 diagnostics split |
| `src/native/engine/CMakeLists.txt` | Native target orchestration | Allowed exception | Explicitly permitted by constitution 1.3.0 | No action required unless structure becomes unclear |

## Per-Slice Recording

For each touched file, record one outcome:

1. Split into smaller single-responsibility files.
2. Reduced to a thin coordinator over named helpers.
3. Left as a temporary exception with owner, reason, and next slice.

## Evidence Log

- `artifacts/native/005-us1-runtime-tick.txt`
- `artifacts/native/005-us2-dx12-diagnostics.txt`
- `artifacts/native/005-us3-architecture-guards.txt`
- `artifacts/native/005-final-focused-suite.txt`
