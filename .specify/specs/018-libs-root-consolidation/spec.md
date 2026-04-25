# Feature Specification: Root Library Consolidation Under libs

**Feature Branch**: `018-libs-root-consolidation`  
**Created**: 2026-04-25  
**Status**: Draft  
**Parent**: [`004-trim-vibe-drift`](../004-trim-vibe-drift/spec.md)

## Summary

Define and govern a monorepo layout update that consolidates library-like code domains under `libs/` to keep the repository root focused, navigable, and CI-stable. The migration must be phased, reversible, and contract-safe across native, API, web, desktop, and mobile channels.

## User Scenarios & Testing

### User Story 1 - Keep root layout clear and predictable (Priority: P1)

As a contributor, I can identify root-level concerns quickly because reusable libraries are grouped under `libs/` and root clutter is reduced.

**Why this priority**: Root discoverability affects onboarding speed, change confidence, and review quality across all domains.

**Independent Test**: Validate the root directory against an approved top-level layout contract and confirm library-like domains are mapped under `libs/`.

**Acceptance Scenarios**:

1. **Given** the approved repository layout contract, **When** the root is evaluated, **Then** top-level folders match the allowed set and library-like domains are consolidated under `libs/`.
2. **Given** a contributor onboarding task, **When** they follow repository layout docs, **Then** they can locate native/API/TypeScript library domains without root-level ambiguity.

---

### User Story 2 - Preserve build and runtime stability during migration (Priority: P1)

As a platform maintainer, I can continue running established build/test/runtime entry points while layout consolidation progresses.

**Why this priority**: Layout cleanup cannot come at the cost of broken CI, broken runtime channels, or unstable developer workflows.

**Independent Test**: Execute canonical build/test/runtime entry points before and after each migration phase and verify equivalent pass/fail behavior.

**Acceptance Scenarios**:

1. **Given** existing build and test entry points, **When** layout migration phases are applied, **Then** command outcomes remain stable or are replaced by explicitly documented equivalents.
2. **Given** runtime channel launch flows, **When** path consolidation completes, **Then** channel startup contracts continue to pass without manual path patching.

---

### User Story 3 - Migrate in phases with compatibility and rollback (Priority: P2)

As a release owner, I can roll out layout changes incrementally with compatibility checkpoints and a tested rollback path.

**Why this priority**: A high-reference-path migration is risky and must be controlled through reversible checkpoints.

**Independent Test**: Simulate phase progression and rollback from an intermediate checkpoint, confirming repository state and contracts recover deterministically.

**Acceptance Scenarios**:

1. **Given** a phase boundary, **When** compatibility checks run, **Then** path contracts and key workflows remain green before the next phase begins.
2. **Given** a failed phase validation, **When** rollback is executed, **Then** repository layout and contract checks return to the last known-good state.

---

### User Story 4 - Keep documentation and AI guidance aligned (Priority: P2)

As a documentation owner, I can rely on synchronized docs/prompts/skills/workflows that reflect the new layout consistently.

**Why this priority**: Stale path references in docs and automation guidance create repeated implementation and triage drift.

**Independent Test**: Run reference validation across docs, prompts, skills, and workflow definitions to ensure moved paths are consistently updated or intentionally exempted.

**Acceptance Scenarios**:

1. **Given** updated layout contracts, **When** repository guidance assets are scanned, **Then** active references point to valid post-migration paths.
2. **Given** legacy baseline snapshots retained for historical context, **When** they intentionally keep old paths, **Then** they are explicitly marked as baseline-only and excluded from active enforcement.

## Edge Cases

- Windows file-handle locks prevent direct directory rename operations during active development sessions.
- Path separator/casing differences create false negatives in contract validation.
- Hidden hardcoded paths in workflows, scripts, prompts, or test fixtures survive partial migration.
- Legacy baseline artifacts intentionally preserve historical paths and must not be misclassified as active drift.
- Generated caches or build artifacts retain pre-migration paths and contaminate validation checks.
- Relative path assumptions from varying working directories break after consolidation.

## Requirements

### Functional Requirements

- **FR-001**: System MUST define objective criteria for what qualifies as a library-like folder eligible for consolidation under `libs/`.
- **FR-002**: System MUST define an approved root-level layout contract, including allowed top-level folders and ownership intent.
- **FR-003**: System MUST provide an explicit source-to-target migration map for each consolidated library domain.
- **FR-004**: Migration MUST execute in phased checkpoints with entry and exit criteria for each phase.
- **FR-005**: Migration MUST provide a compatibility strategy for existing path consumers during transition windows.
- **FR-006**: Existing canonical build/test/runtime entry points MUST remain operational or be replaced with clearly documented equivalents.
- **FR-007**: Active docs, prompts, skills, workflow files, and runbooks MUST be updated to valid post-migration paths.
- **FR-008**: Validation MUST cover native, ASP.NET, TypeScript API, React, Electron, mobile runtime, and compose channel contracts.
- **FR-009**: CI MUST include path-contract validation gates that fail on unresolved active references.
- **FR-010**: Migration MUST include a documented rollback playbook with deterministic recovery checkpoints.
- **FR-011**: File history traceability for moved libraries MUST remain reviewable.
- **FR-012**: `.specify/wiki/` and `.wiki/` operational references MUST remain synchronized for repository layout guidance.
- **FR-013**: Migration MUST avoid functional behavior changes unrelated to repository layout.

### Hard contracts to preserve

- Controller -> service -> pipeline -> native interop flow continuity.
- Existing automation entry points in workspace tasks, `scripts/*.sh`, workflow orchestration scripts, and CMake/dotnet/Bun paths.
- Runtime environment contracts such as `BANANA_NATIVE_PATH`, `BANANA_PG_CONNECTION` for PostgreSQL-backed flows, and `VITE_BANANA_API_BASE_URL` for React.
- Canonical AI-consumable wiki source in `.specify/wiki/` with `.wiki/` as human-facing publication output.

### Key Entities

- **LayoutContract**: Canonical definition of allowed top-level folders and intent.
- **LibraryDomainMap**: Mapping of current library-like folder paths to `libs/` destinations.
- **MigrationPhaseCheckpoint**: Phase gate artifact with preconditions, validations, and rollback point.
- **CompatibilityExceptionList**: Time-bounded list of approved temporary path compatibility allowances.
- **ReferenceValidationReport**: Audit output of active vs baseline-only path references.
- **RollbackPlaybook**: Ordered recovery instructions for reverting to last known-good layout state.

## Success Criteria

- **SC-001**: Repository root conforms 100% to the approved top-level layout contract after migration completion.
- **SC-002**: 100% of active path references across code, workflows, scripts, and operational docs resolve to valid post-migration locations.
- **SC-003**: Canonical CI validation gates pass across native/API/TypeScript/runtime channels after each migration phase.
- **SC-004**: A rollback drill from at least one intermediate phase restores the previous known-good layout and validation state.
- **SC-005**: Onboarding and repository layout docs reflect the new structure with no contradictory active guidance.

## Assumptions

- Migration may be delivered through multiple incremental pull requests rather than one atomic change.
- Some legacy baseline materials can retain historical paths if explicitly marked as baseline-only.
- Directory move operations on Windows may require lock-aware sequencing due to active editor/build handles.
- Domain ownership boundaries remain unchanged; only folder layout and path contracts are in scope.
