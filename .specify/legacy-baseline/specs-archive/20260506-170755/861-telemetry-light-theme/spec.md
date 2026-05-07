# Feature Specification: 861 Telemetry Light Theme

**Feature Branch**: `861-telemetry-light-theme`
**Created**: 2026-05-06
**Status**: Draft
**Wave**: frontend
**Domain**: react

## In Scope *(mandatory)*

- Make telemetry dashboard presentation default to a light-first operator visual direction.
- Ensure dashboard copy avoids restricted vendor-brand wording in all user-visible telemetry surfaces.
- Add automated checks so restricted dashboard wording regressions fail fast in CI.

## Out of Scope *(mandatory)*

- Rewriting backend telemetry ingestion/persistence contracts.
- Replacing current telemetry API payload shape.
- Full design-system/theme overhaul beyond telemetry dashboard routes.

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Light-First Operator View (Priority: P1)

Operators open telemetry dashboards during daytime incident response and need high readability without dark-theme dependency.

**Why this priority**: The dashboard is an active operations surface and readability directly impacts triage speed.

**Independent Test**: Render telemetry dashboard with default app settings and verify light-first palette, contrast, and panel readability without manual theme switching.

**Acceptance Scenarios**:

1. **Given** the operator opens telemetry dashboard with default preferences, **When** the page renders, **Then** the dashboard uses light-first visual presentation with readable panel contrast.
2. **Given** the operator navigates across telemetry panels, **When** cards/chips/tables render, **Then** text and data marks meet defined contrast thresholds.

---

### User Story 2 - Dashboard Copy Policy Guard (Priority: P1)

Operators should not see restricted vendor references in dashboard text so product terminology remains consistent.

**Why this priority**: The user explicitly requested copy-policy enforcement across dashboard surfaces.

**Independent Test**: Run a dashboard-copy policy check against telemetry routes; build/test fails if restricted terms are present in user-visible copy.

**Acceptance Scenarios**:

1. **Given** dashboard copy includes a restricted term, **When** policy checks run, **Then** CI fails with a clear violation message.
2. **Given** dashboard copy is compliant, **When** policy checks run, **Then** CI passes with no violations.

---

### User Story 3 - Regression-Safe Delivery (Priority: P2)

Developers need confidence that copy and theme updates do not break existing telemetry metrics panels.

**Why this priority**: The dashboard now has multiple interactive panels and must stay stable while visual policy updates land.

**Independent Test**: Existing telemetry dashboard unit tests plus targeted new assertions pass under updated copy/theme.

**Acceptance Scenarios**:

1. **Given** the updated dashboard implementation, **When** telemetry page tests run, **Then** core drilldown panels and counters still render correctly.

### Edge Cases

- Dashboard loaded with zero events still presents compliant copy and readable light styling.
- Dashboard loaded from remote API feed with stale timestamps still renders readable table/chip states.
- Policy checker handles mixed-case restricted terms and near-duplicates.

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: Telemetry dashboard MUST ship with a light-first visual presentation for default operator usage.
- **FR-002**: Telemetry dashboard MUST maintain readable contrast for titles, labels, metrics, table rows, and status chips in light-first mode.
- **FR-003**: Telemetry dashboard user-visible copy MUST exclude restricted vendor-brand terminology.
- **FR-004**: Repository MUST include an automated dashboard-copy policy check that runs in CI and fails on restricted terminology.
- **FR-005**: Existing telemetry dashboard rendering tests MUST continue passing after copy/theme updates.
- **FR-006**: Dashboard-copy policy violations MUST report file path and offending token for fast remediation.

### Key Entities *(include if feature involves data)*

- **DashboardCopyPolicy**: Defines restricted terms, target files/routes, and failure-report contract.
- **TelemetryThemeContract**: Defines light-first palette/contrast rules for telemetry dashboard surfaces.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: Telemetry dashboard default view renders in light-first mode with no manual theme toggle required.
- **SC-002**: CI dashboard-copy policy check returns zero violations on compliant branches and deterministic failures on seeded violations.
- **SC-003**: Telemetry dashboard unit tests remain green after copy/theme changes.
- **SC-004**: Dashboard copy contains zero restricted vendor-term occurrences across targeted telemetry dashboard files.

## Assumptions

- The telemetry dashboard remains implemented under `src/typescript/react/src/pages/TelemetryDashboardPage.tsx` and related UI components.
- Existing test infrastructure (Bun + CI workflows) is available for adding copy policy checks.
- Restricted-term policy applies only to user-visible dashboard copy, not to historical logs or third-party documentation snapshots.
