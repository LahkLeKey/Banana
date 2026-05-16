# Feature Specification: Retire Self-Training Surfaces

**Feature Branch**: `003-retire-self-training-surfaces`
**Created**: 2026-05-09
**Status**: Draft
**Input**: Remove autonomous self-training workflow surfaces from the live codebase and keep only intentional non-self-training automation paths.

## In Scope

- Remove live autonomous self-training references from active workflow, script, and docs surfaces.
- Remove or replace active catalogs that exist only for autonomous self-training orchestration.
- Update validation scripts so removed self-training entry points are no longer required.
- Preserve historical records under `.specify/legacy-baseline/` without treating them as active runtime contracts.

## Out of Scope

- Rewriting archived historical artifacts in `.specify/legacy-baseline/specs-archive/`.
- Replacing removed self-training behavior with a new training framework in this slice.
- Frontend UX redesign work unrelated to self-training retirement.

## User Scenarios & Testing

### User Story 1 - Maintainers Remove Self-Training Entrypoints (Priority: P1)

As a maintainer, I can run repository automation without autonomous self-training workflows being referenced, required, or triggered.

**Why this priority**: This is the direct risk-reduction target and unblocks future scope cleanup.

**Independent Test**: Run CI governance and orchestration checks and verify no active contract depends on autonomous self-training workflow files or catalog paths.

**Acceptance Scenarios**:

1. **Given** the active workflow inventory, **When** CI harness validation runs, **Then** no active lane requires autonomous self-training workflow files.
2. **Given** orchestration scripts, **When** default config is resolved, **Then** no autonomous self-training catalog path is required for successful execution.

---

### User Story 2 - Docs Reflect Retired Surface (Priority: P1)

As a contributor, I can read active docs and see self-training marked as retired instead of active behavior.

**Why this priority**: Prevents drift from stale guidance and avoids accidental reintroduction.

**Independent Test**: Search active docs and verify self-training references are either removed or explicitly marked legacy-only.

**Acceptance Scenarios**:

1. **Given** active docs under `.github`, `docs`, and `.specify/wiki`, **When** documentation validation runs, **Then** self-training references are retirement-oriented and non-operational.

---

### User Story 3 - Validation Contracts Stay Green (Priority: P2)

As QA automation, I can validate active contracts without false failures tied to intentionally removed self-training assets.

**Why this priority**: Keeps CI stable after the retirement change.

**Independent Test**: Run contract validation scripts and confirm they pass with the self-training workflow removed from active scope.

**Acceptance Scenarios**:

1. **Given** active contract validators, **When** self-training artifacts are absent from active roots, **Then** validators pass and report expected retirement behavior.

### Edge Cases

- A script still hard-codes an autonomous self-training catalog path.
- A governance check still expects the retired workflow file to exist.
- A docs sync step republishes removed self-training content from stale source paths.

## Requirements

### Functional Requirements

- **FR-001**: The active repository automation surface MUST NOT require or invoke autonomous self-training workflow files.
- **FR-002**: Active scripts under `scripts/` MUST remove default dependencies on `docs/automation/agent-pulse/autonomous-self-training-default-increments.json`.
- **FR-003**: Active validation scripts under `.specify/scripts/` MUST pass without retired self-training workflow path assertions.
- **FR-004**: Active documentation MUST represent self-training as retired or legacy-only and MUST NOT describe it as an active required path.
- **FR-005**: Legacy historical material MAY remain only under `.specify/legacy-baseline/` and MUST be treated as archival context.
- **FR-006**: Active replacement automation surfaces introduced during retirement MUST preserve DDD/SOLID decomposition (domain logic vs orchestration vs infrastructure adapters) instead of monolithic script growth.

### Key Entities

- **ActiveAutomationSurface**: Repository-managed workflows, scripts, and validators currently used by CI and orchestration.
- **RetiredSelfTrainingArtifact**: Any active-root file or reference whose purpose is autonomous self-training execution.
- **LegacyArchiveBoundary**: The `.specify/legacy-baseline/` tree where historical references can remain non-operational.
- **AutomationArchitectureBoundary**: Separation contract that keeps retirement logic, orchestration flow, and I/O/reporting concerns modular.

## Success Criteria

### Measurable Outcomes

- **SC-001**: Active-root search returns zero operational references that require autonomous self-training workflow execution.
- **SC-002**: Active contract validation completes without failing on missing autonomous self-training workflow files.
- **SC-003**: CI harness run contains no autonomous self-training lane or required check.
- **SC-004**: Documentation search across active roots contains only retirement/legacy phrasing for self-training.
- **SC-005**: Retirement automation remains modular with explicit architecture boundaries and no new monolithic control scripts.

## Assumptions

- Training scripts can still be run manually when needed outside autonomous self-training orchestration.
- Historical files in `.specify/legacy-baseline/` remain read-only context and do not affect active runtime behavior.
- Self-training retirement is a deliberate product-scope decision for the current phase.
