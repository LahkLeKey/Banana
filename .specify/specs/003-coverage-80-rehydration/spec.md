# Feature Specification: Coverage 80 Rehydration for Entire Monorepo

**Feature Branch**: `003-coverage-80-rehydration`  
**Created**: 2026-04-25  
**Status**: Draft  
**Type**: Coverage Stabilization Specification  
**Input**: User description: "Scaffold a robust spec that satisfies unit, integration, and e2e coverage to 80 percent for the entire monorepo."  
**Depends On**: `002-compose-ci-stabilization` (lane contract schema baseline, evidence path-safety, runtime preflight contract)

## Summary

Raise and enforce a single, contract-grade coverage baseline of **80 percent minimum** across all merge-gated test layers and all product domains in the Banana monorepo. This feature turns coverage into a deterministic release signal by requiring normalized measurement, lane/domain attribution, aggregate rollups, explicit failure classification, and governed exceptions.

The spec extends the reliability contracts established in `002-compose-ci-stabilization` so coverage behavior is portable, diagnosable in one run, and resistant to format/tool drift across native C, ASP.NET, and TypeScript surfaces.

## In Scope *(mandatory)*

- Enforce **>= 80 percent** on all merge-gated coverage surfaces for:
  - Unit layer
  - Integration layer
  - End-to-end (e2e) layer
- Cover all primary monorepo domains:
  - Native C (`src/native/core`, `src/native/core/dal`, `src/native/wrapper`)
  - ASP.NET API (`src/c-sharp/asp.net`)
  - TypeScript API (`src/typescript/api`)
  - React UI (`src/typescript/react`)
  - Electron runtime (`src/typescript/electron`)
  - React Native runtime (`src/typescript/react-native`)
  - Shared UI package (`src/typescript/shared/ui`)
- Produce normalized lane evidence and one aggregate monorepo summary per run.
- Keep lane outputs deterministic and attributable at the (domain, layer, lane) level.
- Reuse `002` path-safe evidence and preflight contracts for coverage lanes.
- Define exception governance with owner, rationale, expiration, and remediation plan.
- Define coverage denominator policy (exclusions, not-applicable handling, and dedup behavior).

## Out of Scope *(mandatory)*

- Replacing existing test frameworks or coverage engines.
- Redesigning product behavior unrelated to measurable coverage deltas.
- Enforcing coverage on non-merge-gated experimental lanes.
- Introducing mutation, load, or security test gates as part of this feature.
- Reworking architecture boundaries (controller/service/pipeline/native contracts) outside coverage accountability.

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Keep unit coverage merge-safe (Priority: P1)

As a maintainer, I can trust every merge-gated unit lane to enforce 80 percent minimum and fail deterministically when below threshold.

**Why this priority**: Unit coverage is the highest-frequency quality signal and must stay stable to prevent silent regressions.

**Independent Test**: Induce a unit-only coverage drop in one domain and verify deterministic failure attribution in one pass.

**Acceptance Scenarios**:

1. **Given** a unit lane reports coverage `>= 80%`, **When** the lane completes, **Then** it passes with normalized evidence.
2. **Given** a unit lane reports coverage `< 80%`, **When** the lane completes, **Then** it fails with measured value, threshold, and deficit.
3. **Given** tests execute but no unit coverage artifact is produced, **When** publication runs, **Then** the lane fails as a coverage contract failure.

---

### User Story 2 - Protect integration contracts with 80 percent floor (Priority: P1)

As a release owner, I can trust integration coverage to validate cross-layer contracts at or above 80 percent for each applicable domain.

**Why this priority**: Integration coverage catches controller-service-pipeline-native and API-data contract failures that unit tests cannot.

**Independent Test**: Run integration suites with controlled scenario removal and verify deterministic threshold behavior.

**Acceptance Scenarios**:

1. **Given** integration coverage for a domain is `>= 80%`, **When** lane validation runs, **Then** the integration gate passes.
2. **Given** integration coverage is `< 80%`, **When** lane validation runs, **Then** the gate fails with domain + component attribution.
3. **Given** integration lanes require runtime dependencies, **When** preflight dependency checks fail, **Then** the lane fails as preflight contract violation (not as coverage pass).

---

### User Story 3 - Keep e2e runtime channels at 80 percent exercised surface (Priority: P1)

As a release owner, I can trust e2e coverage gates for API+React, Electron, and mobile runtime channels to enforce 80 percent minimum over declared flow inventories.

**Why this priority**: E2e is the final confidence signal that shipped runtime channels are actually exercised.

**Independent Test**: Reduce declared flow execution in one runtime channel and verify e2e gate failure with explicit missing-flow attribution.

**Acceptance Scenarios**:

1. **Given** e2e flow coverage is `>= 80%`, **When** the lane completes, **Then** the lane passes and records fulfilled flow inventory.
2. **Given** e2e flow coverage is `< 80%`, **When** the lane completes, **Then** the lane fails with unexercised flow identifiers.
3. **Given** e2e flake retries exceed declared retry budget, **When** final status resolves, **Then** the lane fails as flake contract failure.

---

### User Story 4 - Keep coverage evidence one-pass diagnosable (Priority: P1)

As a maintainer, I can diagnose any coverage failure without rerunning CI because evidence is normalized and complete for every failing lane.

**Why this priority**: Without one-pass diagnostics, coverage gates become operational noise instead of actionable quality controls.

**Independent Test**: Trigger failures across multiple lanes in one run and verify aggregate output is sufficient for first-pass triage.

**Acceptance Scenarios**:

1. **Given** multiple lanes fail in one run, **When** aggregate summary is published, **Then** each failing (domain, layer, lane) tuple is listed with reason and deficit.
2. **Given** a lane fails pre-publication, **When** artifact publication runs, **Then** fallback evidence is still published for attribution.

---

### User Story 5 - Govern temporary exceptions without weakening baseline (Priority: P2)

As a platform maintainer, I can allow temporary threshold exceptions only when accountability metadata is explicit and machine-valid.

**Why this priority**: Exceptions are necessary for controlled remediation but must not dissolve the baseline.

**Independent Test**: Add valid, malformed, and expired exception records and verify each path behaves predictably.

**Acceptance Scenarios**:

1. **Given** an active exception with complete metadata, **When** coverage evaluates an affected lane, **Then** status reflects exception allowance and is surfaced in reports.
2. **Given** exception metadata is malformed or incomplete, **When** lane evaluation runs, **Then** baseline enforcement applies and lane fails if below threshold.
3. **Given** exception expiration is in the past, **When** lane evaluation runs, **Then** exception is ignored and lane fails on threshold violation.

---

### User Story 6 - Keep path portability and contract parity with 002 (Priority: P2)

As a maintainer, I can run coverage gates on local and CI environments with the same evidence schema and portable paths.

**Why this priority**: Coverage data is only useful if evidence paths and schema are stable across machines.

**Independent Test**: Compare local and CI evidence outputs for schema and path contract parity.

**Acceptance Scenarios**:

1. **Given** coverage evidence is emitted in CI, **When** a local run emits equivalent outputs, **Then** both conform to the same lane schema and relative path contract.
2. **Given** workspace path differs by machine, **When** coverage artifacts are generated, **Then** no machine-specific absolute paths are required for interpretation.

## Coverage Surface Inventory

- **Unit surfaces**: native core/wrapper, ASP.NET service logic, TS API modules, React component logic, Electron main/preload/bridge logic, React Native modules, shared UI package logic.
- **Integration surfaces**: ASP.NET controller-service-pipeline flow, managed-native interop calls, DAL/database flow, TS API route-middleware-data paths, UI-to-API integration contracts.
- **E2E surfaces**: API+React runtime channel, Electron desktop runtime channel, mobile runtime channel (Android emulator and iOS preview fallback under Ubuntu WSL2 contract).

## Edge Cases

- Coverage tool output format changes in one lane while others remain stable.
- Test process exits success but writes empty/partial coverage files.
- Same source file appears in multiple lane reports and would inflate aggregate without dedup logic.
- Domain-layer tuple has no applicable surface (must be explicit `not-applicable`, not omitted).
- Generated or third-party files accidentally enter denominator and distort percentages.
- Native integration requiring `BANANA_PG_CONNECTION` runs without configured database dependency.
- E2e lane completes with retries and conflicting partial artifacts from failed attempts.
- Coverage value near threshold boundary (for example `79.95`) behaves inconsistently without shared rounding contract.
- Exception record exists but points to unknown lane/domain identifier.
- Coverage artifact exists at non-portable absolute path and cannot be replayed on other machines.

## Requirements *(mandatory)*

### Functional Requirements - Baseline and Determinism

- **FR-001**: All merge-gated coverage lanes MUST enforce a minimum threshold of 80 percent on their declared denominator surface.
- **FR-002**: Coverage outcomes MUST be deterministic and attributable to a unique (domain, layer, lane) tuple.
- **FR-003**: Missing, empty, or unparseable coverage artifacts MUST be treated as coverage contract failures.
- **FR-004**: Coverage publication MUST produce machine-readable and human-readable evidence for every lane outcome.
- **FR-005**: Every run MUST publish one aggregate monorepo summary containing all evaluated tuples and statuses.
- **FR-006**: Aggregate summaries MUST deduplicate file-level measurements when a source path appears in multiple lane reports.
- **FR-007**: Threshold comparisons MUST use one documented rounding rule across all lanes.

### Functional Requirements - Coverage Denominator Governance

- **FR-008**: A single exclusion policy MUST define excluded categories (generated, vendored, third-party, build artifacts) and apply uniformly.
- **FR-009**: Domain-layer tuples with no applicable surface MUST be recorded as `not-applicable` with explicit rationale.
- **FR-010**: Not-applicable tuples MUST be excluded from failure counts while remaining visible in summaries.
- **FR-011**: Denominator policy snapshots MUST be included in lane evidence bundles for auditability.

### Functional Requirements - Unit Layer

- **FR-012**: Native C unit coverage MUST enforce >= 80 percent for the applicable unit-testable surface in `src/native/core` and `src/native/wrapper`.
- **FR-013**: ASP.NET API unit coverage MUST enforce >= 80 percent for the applicable unit-testable API service surface.
- **FR-014**: TypeScript API unit coverage MUST enforce >= 80 percent for applicable modules under `src/typescript/api`.
- **FR-015**: React UI unit coverage MUST enforce >= 80 percent for applicable modules under `src/typescript/react`.
- **FR-016**: Electron unit coverage MUST enforce >= 80 percent for applicable modules under `src/typescript/electron`.
- **FR-017**: React Native unit coverage MUST enforce >= 80 percent for applicable modules under `src/typescript/react-native`.
- **FR-018**: Shared UI package unit coverage MUST enforce >= 80 percent for applicable modules under `src/typescript/shared/ui`.

### Functional Requirements - Integration Layer

- **FR-019**: Native integration coverage MUST enforce >= 80 percent for applicable integration paths including DAL flows that rely on `BANANA_PG_CONNECTION`.
- **FR-020**: ASP.NET integration coverage MUST enforce >= 80 percent across applicable controller, service, pipeline, middleware, and managed interop paths.
- **FR-021**: TypeScript API integration coverage MUST enforce >= 80 percent across applicable route, middleware, and data-access integration paths.
- **FR-022**: Cross-layer integration paths (managed-native interop, API-data boundaries) MUST publish attributable coverage evidence for declared entry points.
- **FR-023**: Integration lanes requiring runtime dependencies MUST enforce the `002` preflight contract and fail as preflight violations when unmet.

### Functional Requirements - End-to-End Layer

- **FR-024**: API+React e2e coverage MUST enforce >= 80 percent of declared end-user flow inventory.
- **FR-025**: Electron e2e coverage MUST enforce >= 80 percent of declared desktop runtime flow inventory.
- **FR-026**: Mobile e2e coverage MUST enforce >= 80 percent of declared mobile runtime flow inventory within Ubuntu WSL2 contract constraints.
- **FR-027**: E2e lanes MUST declare their denominator as a machine-readable flow manifest.
- **FR-028**: E2e retry budgets MUST be explicit; retries beyond budget MUST fail as flake contract failures.

### Functional Requirements - Reporting and Failure Taxonomy

- **FR-029**: Every failing lane MUST classify failure reason as one of: threshold violation, coverage contract violation, preflight contract violation, or flake contract violation.
- **FR-030**: Lane summaries MUST include measured value, threshold, deficit, failure class, and evidence references.
- **FR-031**: Aggregate summaries MUST list failed tuples first and include a per-layer failure count.
- **FR-032**: Coverage evidence MUST remain discoverable under lane-scoped artifact roots using workspace-relative references.

### Functional Requirements - Exception Governance

- **FR-033**: Coverage exceptions MUST require owner, rationale, expiration date, remediation plan, and targeted tuple identifier.
- **FR-034**: Exception records with missing or malformed fields MUST be invalid and must not suppress baseline failures.
- **FR-035**: Expired exceptions MUST be ignored automatically and affected lanes MUST enforce baseline threshold.
- **FR-036**: Active exceptions MUST be surfaced in lane summaries and aggregate summaries.
- **FR-037**: Exceptions MUST not alter the baseline threshold value for non-targeted tuples.

### Functional Requirements - Portability and Contract Parity

- **FR-038**: Coverage evidence pathing MUST satisfy `002` path-safety and portability conventions with no machine-specific absolute path dependency.
- **FR-039**: Coverage lane schemas and validation rules MUST remain synchronized across spec, plan artifacts, and executable validation logic.
- **FR-040**: Coverage gates MUST preserve existing cross-layer runtime contracts (`BANANA_PG_CONNECTION`, `BANANA_NATIVE_PATH`, `VITE_BANANA_API_BASE_URL`) for applicable lane surfaces.

### Key Entities *(include if feature involves data)*

- **CoverageLaneResult**: Lane-level record containing `domain`, `layer`, `lane_id`, `measured_percent`, `threshold_percent`, `status`, `failure_class`, `started_at`, `finished_at`, and `evidence_paths`.
- **CoverageTupleStatus**: Normalized status for one (domain, layer) tuple with fields for applicability, measured value, threshold, pass/fail, and deficit.
- **CoverageAggregateSummary**: Run-level collection of all tuple statuses, per-layer failure counts, total failing tuples, and run outcome.
- **CoverageDenominatorPolicy**: Declared policy defining denominator inclusion/exclusion categories, not-applicable rules, and policy version.
- **CoverageExceptionRecord**: Time-bounded exception metadata linked to one targeted tuple with owner and remediation accountability.
- **CoverageContractEvidenceBundle**: Lane-scoped artifact package containing raw outputs, normalized outputs, denominator policy snapshot, and exception snapshot.
- **CoverageVerificationWindow**: Verification set definition (for example 10-run baseline window) used to validate determinism and warning-free behavior over repeated runs.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: 100 percent of merge-gated unit lanes enforce the 80 percent baseline across a 10-run baseline verification window.
- **SC-002**: 100 percent of merge-gated integration lanes enforce the 80 percent baseline across a 10-run baseline verification window.
- **SC-003**: 100 percent of merge-gated e2e lanes enforce the 80 percent baseline across a 10-run baseline verification window.
- **SC-004**: 100 percent of failing lanes publish attributable evidence in the same run with no required rerun for failure-surface identification.
- **SC-005**: Missing-path or missing-artifact coverage publication warnings are reduced to zero across the 10-run verification window.
- **SC-006**: Aggregate run summary coverage includes every applicable (domain, layer) tuple in 100 percent of runs.
- **SC-007**: 100 percent of active exceptions contain complete metadata; malformed or expired exceptions never suppress baseline failures.
- **SC-008**: Coverage outcome classification is deterministic across reruns on identical commits (except declared flake-contract failures).
- **SC-009**: 100 percent of published evidence paths are portable and workspace-relative under `002` lane artifact roots.
- **SC-010**: Median coverage-failure triage time is reduced because first-run artifacts include threshold, deficit, and failure class for all failed tuples.
- **SC-011**: 100 percent of applicable domain-layer tuples achieve and sustain >= 80 percent in merge-gated windows with no ungoverned exceptions.

## Assumptions

- Existing lane foundations remain in place and this feature hardens policy, normalization, gating, and reporting contracts.
- 80 percent remains the agreed baseline floor for all merge-gated applicable tuple surfaces.
- Exceptions are temporary and used only for accountable remediation windows.
- The contracts delivered in `002-compose-ci-stabilization` are authoritative inputs to this feature.

## Dependencies

- `002-compose-ci-stabilization` outputs: lane schema parity, path-safe evidence publication, runtime preflight dependency checks.
- Existing merge-gated lanes for native, .NET, TypeScript, and compose runtime channels.
- Existing environment/runtime contracts for database, native library, and API base URL surfaces.

## Risks and Mitigations

- **Risk**: Coverage format drift across ecosystems breaks normalization.
  - **Mitigation**: Require failure-class taxonomy and explicit contract-failure handling for parse/shape mismatches.
- **Risk**: Denominator inflation from generated or vendored sources masks real regressions.
  - **Mitigation**: Enforce single denominator policy and snapshot policy in evidence bundle.
- **Risk**: E2e flakes create false negatives and unstable enforcement.
  - **Mitigation**: Declare retry budgets and classify over-budget results separately as flake contract failures.
- **Risk**: Exception records become permanent debt.
  - **Mitigation**: Enforce expiration, remediation plan, and aggregate exception visibility.
