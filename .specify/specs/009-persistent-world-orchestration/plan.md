# Implementation Plan: Deterministic Persistent World Orchestration

**Branch**: `009-persistent-world-orchestration` | **Date**: 2026-05-24 | **Spec**: `.specify/specs/009-persistent-world-orchestration/spec.md`

**Input**: Feature specification from `.specify/specs/009-persistent-world-orchestration/spec.md`

## Summary

Define a deterministic, server-authoritative orchestration contract for persistent world areas so that terrain/mesh baselines are reproducible from one world seed, area identity and chunk partitions remain stable across sessions, interactive deltas are replayed and merged deterministically, and generation or persistence faults retry without identity duplication or state divergence.

## Technical Context

**Language/Version**: Native C runtime surfaces (`src/native`) with TypeScript 5.x API orchestration on Bun (`src/typescript/api`)

**Primary Dependencies**: Native world/terrain/mesh generation pipeline, API controller -> service -> pipeline orchestration, persistence services for authoritative area state and delta logs

**Storage**: Server-side authoritative baseline + delta persistence (PostgreSQL-backed runtime contracts) with deterministic replay metadata

**Testing**: Bun unit/contract tests for API orchestration, native deterministic parity/integration tests, replay and conflict integration matrix, injected failure-mode tests

**Target Platform**: Windows 10+ runtime channel and Linux/containerized API runtime

**Project Type**: Cross-domain persistence and deterministic orchestration contract feature in monorepo

**Performance Goals**: Meet SC-001 through SC-005 parity and recovery thresholds; maintain generation/replay decisioning within interactive area-load budgets

**Constraints**: One root seed derivation path, stable area identity/chunk partition mapping, deterministic merge/replay regardless of player order, deterministic retries on recoverable faults, monotonic area state versioning

**Scale/Scope**: Area orchestration contracts spanning unexplored generation, revisit replay, cross-player post-modification consistency, and fault recovery evidence flows

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

- [x] Player trust and disclosure alignment verified. The feature governs runtime consistency behavior and does not introduce new storefront claims beyond deterministic persistent continuity already validated through quality gates.
- [x] Storefront governance artifacts identified when public Steam copy is affected. No direct storefront copy deltas are required at plan time; if claims change, storefront contract/checklist artifacts are added in follow-on scope.
- [x] Cross-domain contracts mapped for touched layers (native/API/client/runtime) and required docs are queued in-scope.
- [x] Quality gates defined with measurable checks for deterministic generation parity, replay/merge consistency, cross-session/cross-player correctness, and failure-mode retry safety.
- [x] Reproducible delivery path identified for target runtime channels with evidence capture paths defined in quickstart.

**Post-Design Re-check**: Pass. Research decisions and Phase 1 artifacts resolve deterministic identity, baseline+delta persistence boundaries, replay/merge policy, and fault retry semantics without unresolved constitution violations.

## Project Structure

### Documentation (this feature)

```text
.specify/specs/009-persistent-world-orchestration/
├── plan.md
├── research.md
├── data-model.md
├── quickstart.md
├── contracts/
│   ├── deterministic-generation-contract.md
│   ├── area-identity-partition-contract.md
│   ├── authoritative-persistence-replay-contract.md
│   └── failure-retry-reconciliation-contract.md
└── tasks.md
```

### Source Code (repository root)

```text
src/native/
├── engine/world/
├── engine/runtime/
└── engine/render/

src/typescript/api/
└── src/

tests/
├── native/
└── integration/
```

**Structure Decision**: Keep implementation inside existing native world/runtime and TypeScript API orchestration boundaries to preserve the controller -> service -> pipeline -> native interop contract and avoid introducing additional structural layers.

## Storefront Deliverables

- Not required for initial implementation planning scope because this feature changes deterministic runtime orchestration and persistence behavior, not public Steam copy directly.
- If release messaging expands persistent-world guarantees, add storefront contract/checklist artifacts in the same change set.

## Phase Plan

### Phase 0: Research (complete)

- Resolved root-seed derivation and normalization policy, stable area identity/chunk partition mapping, deterministic baseline signatures, authoritative merge/replay ordering, and retry semantics for recoverable failures.
- Output: `research.md`

### Phase 1: Design and Contracts (complete)

- Captured entities, validation rules, relationships, and state transitions in `data-model.md`.
- Authored deterministic generation, area identity/partition, authoritative persistence/replay, and failure/retry reconciliation contracts under `contracts/`.
- Defined validation and evidence workflow in `quickstart.md`.
- Updated Speckit plan pointer in `.github/copilot-instructions.md` to this feature plan.

### Phase 2: Task Planning Inputs (prepared for `/speckit.tasks`)

| Slice | Goal | Primary Areas | Validation Focus |
|-------|------|---------------|------------------|
| S1 | Enforce deterministic seed derivation and area identity mapping | native world/runtime + API orchestration | Repeated-input parity and partition stability checks |
| S2 | Implement authoritative baseline + delta persistence boundaries | API services + persistence contracts | Revisit replay parity and stale update rejection |
| S3 | Add deterministic merge/replay policy for overlapping deltas | API reconciliation pipeline + native apply hooks | Cross-player conflict parity across repeated runs |
| S4 | Harden fault handling and deterministic retries | generation/persistence orchestration paths | Injected recoverable/non-recoverable failure matrix |
| S5 | Capture evidence and parity reports for release quality gates | tests + artifacts capture scripts | SC-001 to SC-005 evidence completeness |

## Validation Strategy

1. Deterministic baseline parity: regenerate sampled unexplored areas with equivalent seed+identity inputs and compare canonical signatures.
2. Revisit parity: replay baseline+deltas across independent sessions and verify authoritative state equivalence.
3. Cross-player consistency: apply overlapping modifications in different player-arrival orders and confirm canonical merge parity.
4. Fault recovery: inject generation/persistence faults, perform deterministic retries, and verify no duplicate area identities or divergent canonical states.
5. Evidence capture: publish parity, replay, and failure-recovery outputs to the paths defined in quickstart.

## Traceability and Evidence

### Requirement coverage intent

- FR-001, FR-002, FR-003, FR-004: deterministic world seed derivation, stable area identity/chunk partitioning, and seam-safe baseline generation.
- FR-005, FR-006, FR-009, FR-013, FR-014: server-authoritative baseline+delta persistence and revisit reconstruction path.
- FR-007, FR-008, FR-010: deterministic merge/replay policy with versioned authoritative state transitions and stale update rejection.
- FR-011, FR-012: deterministic failure handling, retry rules, and operator-facing fault context.

### Success criteria evidence intent

- SC-001: baseline geometry/mesh signature parity report for repeated generation runs.
- SC-002: revisit authoritative-state parity report across session boundaries.
- SC-003: cross-player modified-area consistency report.
- SC-004: repeated conflict-reconciliation output parity report.
- SC-005: injected fault recovery report proving deterministic retries and no area identity duplication.

## Complexity Tracking

No constitution violations require exception tracking at planning time.
