# Feature Specification: Compose CI Stabilization

**Feature Branch**: `002-compose-ci-stabilization`  
**Created**: 2026-04-25  
**Status**: Draft  
**Input**: User description: "Add a new spec to address Compose CI failures observed in the e2e runner contract run, including compose lane failures, coverage lane instability, artifact upload gaps, and Node runtime deprecation warnings."

## In Scope *(mandatory)*

- Stabilize Compose CI lane behavior for `compose-tests`, `compose-runtime`, `compose-electron`, and smoke validation surfaces.
- Ensure failing lanes emit deterministic diagnostics and usable failure evidence.
- Remove recurring artifact-upload gaps where expected outputs are missing at publish time.
- Align Compose CI workflow behavior with explicit skip semantics for optional or absent test projects.
- Extend deterministic failure evidence patterns to merge-gated non-compose coverage/test jobs.
- Eliminate artifact upload warning paths that currently emit missing-path warnings in merge-gated jobs.
- Require actionable permission-failure diagnostics (including exit code `126`) with direct remediation context.
- Track and eliminate Node runtime deprecation exposure in merge-gated workflows.

## Out of Scope *(mandatory)*

- Introducing net-new product capabilities unrelated to CI reliability.
- Replacing Docker Compose with a different orchestration platform.
- Redesigning application business logic for API, native, or model scoring behavior.
- Expanding release governance beyond current merge-gate and evidence requirements.

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Keep Compose lanes merge-safe (Priority: P1)

As a maintainer, I can trust merge-gated Compose lanes to return deterministic pass/fail outcomes.

**Why this priority**: Unstable Compose lane outcomes block delivery and hide true regressions.

**Independent Test**: Run Compose CI against unchanged baseline commits and verify lane outcomes are consistent and explainable.

**Acceptance Scenarios**:

1. **Given** unchanged baseline code, **When** Compose CI executes `compose-tests`, `compose-runtime`, and `compose-electron`, **Then** each lane reaches a deterministic terminal state without ambiguous shell/runtime exit behavior.
2. **Given** a real lane failure, **When** the job completes, **Then** logs and lane diagnostics identify the failing service and stage.

---

### User Story 2 - Preserve actionable evidence on failures (Priority: P1)

As a release owner, I can inspect complete artifacts for every failing lane without rerunning the workflow blindly.

**Why this priority**: Missing diagnostics turns triage into guesswork and delays remediation.

**Independent Test**: Force representative lane failures and confirm diagnostic artifacts are always generated and uploaded.

**Acceptance Scenarios**:

1. **Given** a lane fails before test execution finishes, **When** artifact upload runs, **Then** a deterministic fallback artifact set still exists.
2. **Given** an optional test surface is absent, **When** the pipeline evaluates it, **Then** the workflow publishes explicit skip evidence rather than a missing-path upload error.
3. **Given** a merge-gated non-compose coverage/test job fails, **When** artifact publication executes, **Then** deterministic stage/reason/exit evidence is published.
4. **Given** a configured artifact path is absent, **When** upload preparation runs, **Then** the workflow emits explicit skip/fallback evidence and avoids missing-path upload warnings.

---

### User Story 4 - Make merge-gated failures one-pass diagnosable (Priority: P1)

As a maintainer, I can diagnose permission and coverage failures in one pass across merge-gated jobs without rerunning CI for missing context.

**Why this priority**: Current failures still require additional digging because stage/reason evidence is inconsistent across job types.

**Independent Test**: Induce representative permission and coverage failures, then verify failed runs include stage/reason attribution and remediation context on first pass.

**Acceptance Scenarios**:

1. **Given** a job exits with a permission-related status, **When** diagnostics are written, **Then** the evidence includes failing surface and remediation guidance.
2. **Given** multiple merge-gated jobs fail in the same run, **When** summaries are published, **Then** a machine-readable aggregate identifies each failed job with stage and reason.

---

### User Story 3 - Eliminate workflow runtime drift warnings (Priority: P2)

As a platform maintainer, I can run merge-gated workflows without Node runtime deprecation warnings.

**Why this priority**: Runtime deprecation debt can silently break CI when runner defaults change.

**Independent Test**: Execute merge-triggered workflow set and verify deprecation warnings are absent or tracked as explicit residual risk with owner/date.

**Acceptance Scenarios**:

1. **Given** merge-triggered workflows, **When** jobs execute on current GitHub runners, **Then** no Node runtime deprecation warnings appear in annotations.
2. **Given** a dependency cannot be upgraded immediately, **When** the workflow runs, **Then** the warning is documented as a bounded, time-stamped exception with a planned remediation path.

### Edge Cases

- Compose service exits before health checks complete, producing partial startup logs.
- Shell or file-permission mismatches cause exit code `126` before test commands run.
- Optional test project paths are absent in some branches, causing artifact path mismatches.
- Non-compose coverage/test jobs fail before producing reports, leaving upload paths unresolved unless fallback/skip evidence is generated.
- Lane succeeds functionally but still fails due missing artifact files at upload step.
- Multiple merge-gated jobs fail in one run and require a single deterministic aggregate summary.
- Workflow gates pass while runtime deprecation warnings accumulate toward forced runtime cutoff.

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: Compose CI MUST provide deterministic terminal outcomes for `compose-tests`, `compose-runtime`, and `compose-electron` lanes on unchanged baseline commits.
- **FR-002**: Compose CI MUST emit lane-scoped diagnostics that identify service, stage, and failure reason when a lane fails.
- **FR-003**: Compose CI MUST preserve artifact publication behavior for both success and failure states.
- **FR-004**: Compose CI MUST avoid missing-path artifact upload failures by generating explicit fallback or skip artifacts for optional surfaces.
- **FR-005**: Compose CI MUST classify optional/absent project surfaces with explicit skip semantics instead of implicit failures.
- **FR-006**: Compose CI MUST surface container permission and script contract violations as explicit diagnostics rather than opaque exit codes.
- **FR-007**: Compose CI MUST keep merge-gated API/native/compose evidence mutually traceable to lane outcomes.
- **FR-008**: Merge-triggered workflow runs MUST remove Node runtime deprecation warnings or record explicit bounded exceptions with owner and target remediation date.
- **FR-009**: CI diagnostics MUST be sufficient for one-pass triage without requiring an immediate rerun to discover the failure surface.
- **FR-010**: The stabilization baseline MUST remain compatible with existing Banana runtime contracts and compose profiles.
- **FR-011**: Merge-gated non-compose coverage/test jobs MUST emit deterministic job result records with stage, reason code, and exit code when they fail.
- **FR-012**: Permission-denied failures (including exit code `126`) MUST include actionable remediation context that identifies the failing surface or contract.
- **FR-013**: Merge-gated artifact publication steps MUST be path-safe: each configured upload path MUST resolve to produced output or explicit skip/fallback evidence without missing-path warnings.
- **FR-014**: Merge-gated workflows MUST publish a machine-readable aggregate summary of failed jobs with stage and reason attribution.

### Key Entities *(include if feature involves data)*

- **ComposeCILaneResult**: Lane-level pass/fail/skip record with terminal status, exit reason, and stage attribution.
- **MergeGateJobResult**: Job-level pass/fail/skip record for non-compose coverage/test jobs with deterministic stage and reason mapping.
- **ComposeDiagnosticsBundle**: Structured evidence set containing logs, service status snapshots, and health payload captures.
- **ArtifactPublicationRecord**: Expected-versus-produced artifact manifest used to detect and prevent missing-path uploads.
- **FailureAggregateSummary**: Machine-readable run summary enumerating failed merge-gated jobs with stage/reason attribution.
- **WorkflowRuntimeCompatibilityRecord**: Tracking record for runtime deprecation warnings, owners, and remediation deadlines.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: Merge-gated Compose lanes complete with deterministic pass/fail/skip outcomes in 100% of baseline reruns over a 10-run verification window.
- **SC-002**: 100% of failing lanes publish actionable diagnostics that identify failing service and stage.
- **SC-003**: Missing-path artifact upload errors are reduced to zero across a 10-run verification window.
- **SC-004**: Node runtime deprecation warnings in merge-triggered workflows are reduced to zero, or each remaining warning has an explicit tracked exception with owner and target date.
- **SC-005**: Median triage time for Compose CI failures is reduced because failure evidence is complete in the first run.
- **SC-006**: Across a 10-run merge-gated verification window, missing-path upload warnings are zero for compose and non-compose jobs.
- **SC-007**: 100% of permission-denied failures include remediation context identifying the failing surface.
- **SC-008**: 100% of failed merge-gated coverage/test jobs publish deterministic stage/reason/exit evidence.

## Assumptions

- Compose CI remains a merge-gated quality signal for this repository.
- Existing compose profiles (`tests`, `runtime`, `electron`, and app-smoke surfaces) continue to represent required validation coverage.
- Diagnostic artifacts are the primary mechanism for deterministic CI triage.
- Runtime deprecation mitigation is expected to happen incrementally while preserving merge safety.