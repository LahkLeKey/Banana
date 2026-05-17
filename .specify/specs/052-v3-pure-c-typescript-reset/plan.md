# Implementation Plan: V3 Runtime Baseline Reset

**Branch**: `[864-v3-pure-c-typescript-reset]` | **Date**: 2026-05-16 | **Spec**: [.specify/specs/052-v3-pure-c-typescript-reset/spec.md](.specify/specs/052-v3-pure-c-typescript-reset/spec.md)
**Jurisdiction**: federal | **Agent of Record**: Platform Architecture
**Input**: Feature specification from `.specify/specs/052-v3-pure-c-typescript-reset/spec.md`

## Summary

Reset V3 planning to one repository-grounded authority by treating the current scaffolded source tree as the active runtime baseline, demoting archived/speculative lineage to reference-only status, and generating downstream planning artifacts around four bounded execution lanes: native gameplay, API gameplay, frontend gameplay, and shared gameplay contracts.

## Authority Chain

- **Governing Spec**: `.specify/specs/052-v3-pure-c-typescript-reset/spec.md`
- **Bounded Context**: Cross-repository governance for the V3 gameplay baseline, evidence inventory, stale-path exclusion, and downstream planning handoff
- **Executing Authority**: Platform Architecture coordinating native gameplay, API gameplay, frontend gameplay, and shared gameplay contract lanes
- **Superseded Inputs**: `050-multiplayer-server-authority`, `051-ai-gameplay-loop`, and any archived viewport-first lineage in `.specify/legacy-baseline/`
- **Archive Action**: Keep archived specs and legacy code snapshots for lineage only; do not treat them as active authority when current source paths disagree

## Technical Context

**Language/Version**: C via CMake 3.20 native scaffold; TypeScript/JavaScript ESM package scaffolds under `src/typescript/*`  
**Primary Dependencies**: CMake native build, Bun/Node import-smoke tooling, Spec Kit bash orchestration scripts under `.specify/scripts/bash/`  
**Storage**: N/A for the reset baseline; no active persistence contract is implemented in current scaffold surfaces  
**Testing**: Spec extension preflight, confidence gate heartbeat, native CMake configure/build smoke, and package-script build smokes for the TypeScript scaffold packages  
**Target Platform**: Repository-root planning artifacts for the Banana monorepo; runtime channels remain governed by the Windows + Docker Desktop + Ubuntu WSL2 contract when real runtime surfaces are reintroduced  
**Project Type**: Multi-language monorepo scaffold with native core, managed orchestration shells, and documentation-driven planning artifacts  
**Performance Goals**: Planning authority must resolve baseline-scope decisions from reset artifacts alone; sampled retained capabilities should remain traceable to evidence within 10 minutes per spec success criteria  
**Constraints**: One active baseline only; gameplay/customer-facing scope only; preserve controller -> service -> pipeline -> native interop layering for future managed flows; treat unverified archived or heartbeat claims as lineage rather than runtime truth  
**Scale/Scope**: Current active runtime surface is limited to `src/native`, five TypeScript package scaffolds (`api`, `react`, `electron`, `react-native`, `shared/ui`), and scaffold placeholders under `scripts`, `tests`, `docs`, `docker`, and `data`

## Constitution Check

*GATE: Must pass before research. Re-check after design.*

- **Domain Core First**: Pass. The active runtime baseline centers on `src/native` as the only current gameplay-adjacent implementation surface, with managed layers treated as orchestration shells.
- **Layered Interop Contract**: Pass. The plan preserves the future `controller -> service -> pipeline -> native interop` boundary by assigning lane ownership instead of collapsing responsibilities.
- **Spec First Delivery**: Pass. Planning outputs are generated from the active spec and written into the feature folder before any further implementation slicing.
- **Documentation and Wiki Parity**: Pass for this planning slice. The agent context pointer is updated to the new plan so future work reads the correct authority.
- **Confidence-Gated Clarification / Heartbeat**: Pass. Preflight passed and the startup confidence gate recorded 92% in `heartbeat-log.md`, which is above the 80% threshold.
- **Gameplay-Only Scope Gate**: Pass. This plan excludes workflow-only, training, workbench, and stale viewport-first pathways from V3 runtime authority.
- **Post-Design Re-check**: Pass. `research.md`, `data-model.md`, `quickstart.md`, and `contracts/` all preserve the same active-authority, single-lane, gameplay-only rules with no justified violations.

- If confidence in the next code-improving step is below 80%, stop and resolve the uncertainty with targeted Q/A before continuing. Record the resulting constraint, assumption, or decision in this plan.

## Orchestration Preflight

- Extension health preflight completed successfully:
  - `.specify/scripts/bash/spec-extension-preflight.sh --update-first --json`
- Confidence gate completed successfully:
  - `.specify/scripts/bash/spec-confidence-gate.sh --confidence 92 --step "go-copilot-start" --threshold 80 --notes "startup gate"`
- Heartbeat evidence is recorded in `.specify/specs/052-v3-pure-c-typescript-reset/heartbeat-log.md`.
- If confidence drops below 80, pause and request human input before continuing.

## Project Structure

### Documentation (this feature)

```text
.specify/specs/052-v3-pure-c-typescript-reset/
├── plan.md
├── research.md
├── data-model.md
├── quickstart.md
├── contracts/
│   ├── baseline-authority.md
│   └── execution-lanes.md
├── heartbeat-log.md
├── codebase-scan.md
├── v3-scope-gate-checklist.md
├── v3-bootstrap-docket.md
└── tasks.md
```

### Source Code (repository root)

```text
src/
├── native/
│   ├── CMakeLists.txt
│   ├── include/banana_native_v3.h
│   └── scaffold/native_entry.c
└── typescript/
    ├── api/
    │   ├── package.json
    │   └── src/index.ts
    ├── react/
    │   ├── package.json
    │   └── src/index.ts
    ├── electron/
    │   ├── package.json
    │   └── main.js
    ├── react-native/
    │   ├── package.json
    │   └── index.ts
    └── shared/ui/
        ├── package.json
        └── src/index.ts

scripts/README.md
tests/README.md
docker/README.md
docs/
data/
```

**Structure Decision**: Use the current monorepo scaffold as the only active runtime structure. Planning artifacts must point to existing native and TypeScript scaffold paths, while `scripts`, `tests`, `docker`, `docs`, and `data` remain supporting placeholders until future gameplay slices introduce executable surfaces. `codebase-scan.md`, `v3-scope-gate-checklist.md`, and `v3-bootstrap-docket.md` are the downstream planning drilldowns that turn this scaffold baseline into evidence-backed lane handoffs.

## Phase 0 Research Summary

- The current source tree, `.specify/specs/README.md`, and `.specify/feature.json` collectively establish Spec 052 as the sole active V3 baseline.
- The authoritative runtime baseline is intentionally minimal: `banana_native_v3_abi_version()` / `banana_native_v3_ping()` in native C plus placeholder TypeScript package entry points.
- Archived specs, legacy snapshots, and any artifact claims that reference unavailable functions or modules are lineage-only unless backed by current source paths.
- The correct downstream decomposition is four bounded lanes with two later stitch points:
  - Lane A: native gameplay baseline
  - Lane B: API gameplay/session orchestration
  - Lane C: frontend runtime shells
  - Lane D: shared gameplay contracts

## Phase 1 Design

### Artifact Outputs

- `research.md`: documents baseline authority, stale-path policy, lane selection, and validation decisions
- `data-model.md`: defines the planning entities and validation rules used to classify retained capabilities
- `contracts/baseline-authority.md`: codifies what sources may and may not govern V3 planning decisions
- `contracts/execution-lanes.md`: codifies single-lane ownership and stitch-point rules
- `quickstart.md`: explains how to apply the reset baseline to downstream planning work

### Baseline Design Decisions

1. **Source-first authority**: Current repository paths outrank archived specs and older planning notes.
2. **Scaffold-first truth**: The present baseline is intentionally minimal and should not be inflated with unimplemented capability claims.
3. **Gameplay-only retention**: Only customer-facing gameplay/runtime work is eligible for V3 planning.
4. **Single-lane ownership**: Each retained capability belongs to exactly one of the four execution lanes until a defined stitch slice is reached.
5. **Evidence-backed continuation**: Every downstream slice must cite a live source path or an explicitly archived lineage reference plus migration note.

## DDD/SOLID Decomposition

- **Domain Policy**: One active baseline, one owning lane per retained capability, source-first authority, and confidence/heartbeat gates at or above 80%.
- **Application Flow**: The plan consumes the governing spec, scans live repository surfaces, classifies retained capabilities, and emits lane-specific artifacts without embedding implementation detail into shared governance rules.
- **Infrastructure Adapters**: `.specify/scripts/bash/setup-plan.sh`, `.specify/scripts/bash/spec-extension-preflight.sh`, `.specify/scripts/bash/spec-confidence-gate.sh`, `heartbeat-log.md`, and the generated contracts/data-model artifacts.
- **Single Responsibility**: Each artifact owns one concern: plan (execution strategy), research (resolved decisions), data model (entities and rules), contracts (governance interfaces), quickstart (operator workflow).

## Domain-Contract Test Decomposition

- **Bounded Test Suites**: Current validation is scaffold-oriented and should remain lane-specific as implementation resumes:
  - Native lane: CMake configure/build smoke around `banana_native`
  - API lane: package-level import/build smoke for `src/typescript/api`
  - Frontend lane: shell boot/import smoke for `react`, `electron`, and `react-native`
  - Shared contracts lane: export smoke for `src/typescript/shared/ui`
- **Suite Ownership**: Each lane owns its validation independently; only stitch slices add cross-lane compatibility checks.
- **Testable Boundaries**: Native ABI exports, managed package entry points, and contract module exports are the minimum current boundaries that can be validated without reactivating archived systems.
- **Coverage Contract**: No expanded coverage gate is added in this planning slice; future native gameplay slices must define domain-contract coverage targets before complexity grows beyond the scaffold.

## Complexity Tracking

No constitution violations require justification for this planning slice.
