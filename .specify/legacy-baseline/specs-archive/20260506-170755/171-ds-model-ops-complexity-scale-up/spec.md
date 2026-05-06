# Feature Specification: DS Model Ops Complexity Scale-Up (171)

**Feature Branch**: `171-ds-model-ops-complexity-scale-up`
**Created**: 2026-05-03
**Status**: Ready for implementation
**Input**: Formalise the five data-science spikes (166-170) that were prototyped in the React workspace. Drive down telemetry noise, surface workbench eval harness metrics, wire the ripeness signal pipeline, expose embedding drift deltas, and enforce promotion governance.

## Problem Statement

The DS spike prototypes (166-170) delivered working UI scaffolds but lack backend contracts, shared-UI type exports, and formal test coverage. Without these the workbench remains a visual demo rather than a measurable, trustworthy ML-ops surface. This feature closes that gap.

## In Scope

- **Spike 166 – Telemetry Reduction**: Add `GET /operator/telemetry/config` to the ASP.NET API returning `{ sample_rate, unit }`; surface the current rate in the OperatorPage UI.
- **Spike 167 – Workbench Eval Harness**: Export `TrainingMetricCard` component from `@banana/ui`; ensure `TrainingRunResult.metrics` type carries `centroid_embedding`, `accuracy`, and `f1` fields; wire it in KnowledgePage.
- **Spike 168 – Ripeness Signal Pipeline**: Export `RipenessResult` type from `@banana/ui`; update `predictRipeness` to accept `{ sample: string }` directly; wire typed response in BananaAIPage `RipenessBar`.
- **Spike 169 – Embedding Drift Surface**: Export `computeCosineDrift(a: number[], b: number[]): number` utility from `@banana/ui`; expose typed `EmbeddingDriftSummary` per lane; wire drift sidebar in KnowledgePage from the utility.
- **Spike 170 – Promotion Governance Gate**: Export `PromotionAuditEntry` type from `@banana/ui`; extend workbench history response type to include `promotion_audit`; surface audit entries in FunctionsPage after promotion.

## Out of Scope

- Real native centroid embedding computation (placeholder zeros acceptable for this slice).
- Multi-instance telemetry aggregation or distributed tracing.
- Promotion workflow persistence beyond in-memory audit log.
- Backend changes to the training runner itself.

## User Scenarios & Testing

### User Story 1 – Telemetry Rate Visible in Operator UI (Priority: P1)

As an operator I want to see the current telemetry sampling rate from the live API so I can verify that noise-reduction config is active.

**Why this priority**: Telemetry reduction is the foundational observability gate; without it all downstream DS metrics are polluted.

**Independent Test**: Navigate to `/operator` — a "Telemetry Config" card shows the `sample_rate` value fetched from `GET /operator/telemetry/config`.

**Acceptance Scenarios**:

1. **Given** the API is running, **When** `GET /operator/telemetry/config` is called, **Then** it returns `{ sample_rate: number, unit: "percent" }`.
2. **Given** the OperatorPage loads, **When** the config fetch resolves, **Then** the sample rate is displayed in a labelled card.
3. **Given** the API is unreachable, **When** OperatorPage loads, **Then** an error state is shown without crashing.

---

### User Story 2 – Training Metric Cards with Accuracy and F1 (Priority: P1)

As a data scientist I want to see accuracy and F1 bars for the last three runs per training lane so I can spot regressions without opening logs.

**Why this priority**: The workbench eval harness is the primary DS productivity win — it replaces manual log parsing with at-a-glance signal.

**Independent Test**: KnowledgePage renders `TrainingMetricCard` for each lane showing per-run accuracy and F1 mini-bars sourced from `@banana/ui`.

**Acceptance Scenarios**:

1. **Given** history returns runs with `metrics.accuracy` and `metrics.f1`, **When** KnowledgePage loads, **Then** each run renders an accuracy bar and an F1 bar.
2. **Given** `metrics.accuracy` is `undefined`, **When** the card renders, **Then** the bar shows 0 without throwing.
3. **Given** `TrainingMetricCard` is exported from `@banana/ui`, **When** imported in KnowledgePage, **Then** no TypeScript errors occur.

---

### User Story 3 – Ripeness Signal Typed Response (Priority: P2)

As a developer I want `predictRipeness` to return a typed `RipenessResult` so I can safely access `label` and `confidence` without casting.

**Why this priority**: Removes the stringly-typed `JSON.stringify` workaround; downstream components rely on correct field access.

**Independent Test**: BananaAIPage compiles with `ripenessData.label` and `ripenessData.confidence` via the exported `RipenessResult` type from `@banana/ui`.

**Acceptance Scenarios**:

1. **Given** `RipenessResult = { label: Ripeness, confidence: number }` is exported from `@banana/ui`, **When** BananaAIPage imports it, **Then** TypeScript compilation succeeds.
2. **Given** the user clicks "Run Ripeness", **When** the API returns `{ label: "ripe", confidence: 0.87 }`, **Then** the `RipenessBar` renders correctly.
3. **Given** `predictRipeness` accepts `{ sample: string }`, **When** called from BananaAIPage, **Then** no runtime JSON double-encode occurs.

---

### User Story 4 – Embedding Drift Delta Sidebar (Priority: P2)

As a data scientist I want to see the cosine distance between the last two run centroids per lane so I can detect model drift at a glance.

**Why this priority**: Surfaces embedding drift without requiring a separate observability tool.

**Independent Test**: KnowledgePage drift sidebar shows a `Δ` value per lane from `computeCosineDrift` exported by `@banana/ui`; values above 0.1 are highlighted yellow.

**Acceptance Scenarios**:

1. **Given** `computeCosineDrift(a, b)` is exported from `@banana/ui`, **When** called with two equal vectors, **Then** it returns `0`.
2. **Given** orthogonal vectors, **When** `computeCosineDrift` returns `1`, **Then** the drift card is highlighted yellow.
3. **Given** fewer than two runs exist for a lane, **When** KnowledgePage renders, **Then** the drift card shows `—` without errors.

---

### User Story 5 – Promotion Audit Trail in Functions Page (Priority: P3)

As an operator I want to see a log of promotion events after triggering a promotion so I can audit who promoted what and when.

**Why this priority**: Closes the governance loop by making promotions observable.

**Independent Test**: After clicking "Promote Latest Candidate" in FunctionsPage, a promotion audit card appears showing `run_id`, `promoted_at`, and `lane`.

**Acceptance Scenarios**:

1. **Given** `PromotionAuditEntry = { run_id: string, promoted_at: string, lane: TrainingLane }` is exported from `@banana/ui`, **When** FunctionsPage imports it, **Then** TypeScript compiles cleanly.
2. **Given** `promoteTrainingWorkbenchRun` resolves, **When** FunctionsPage updates state, **Then** a promotion audit entry is appended to a visible list.
3. **Given** no promotion has been triggered, **When** FunctionsPage loads, **Then** the audit list is empty with no errors.

### Edge Cases

- What happens when history returns runs with no `metrics` field? Cards render at 0 without crashing.
- What happens when centroid embeddings are all zeros? Drift computes as 0 correctly.
- What if the telemetry config endpoint returns a non-200? OperatorPage shows an inline error card.

## Requirements

### Functional Requirements

- **FR-001**: `GET /operator/telemetry/config` MUST return `{ sample_rate: number, unit: string }` with status 200.
- **FR-002**: `TrainingRunResult.metrics` MUST include optional `accuracy?: number`, `f1?: number`, and `centroid_embedding?: number[]` fields in the `@banana/ui` shared type.
- **FR-003**: `computeCosineDrift` MUST be exported from `@banana/ui` and handle zero-length or equal vectors without throwing.
- **FR-004**: `RipenessResult` MUST be exported from `@banana/ui` with fields `label: Ripeness` and `confidence: number`.
- **FR-005**: `PromotionAuditEntry` MUST be exported from `@banana/ui` with fields `run_id`, `promoted_at`, and `lane`.

### Key Entities

- **TrainingRunResult**: existing type; extended with optional `accuracy`, `f1`, `centroid_embedding` on `metrics`.
- **RipenessResult**: `{ label: Ripeness, confidence: number }` — replaces inline type in BananaAIPage.
- **EmbeddingDriftSummary**: `{ lane: TrainingLane, drift: number | null }` — computed per lane in KnowledgePage.
- **PromotionAuditEntry**: `{ run_id: string, promoted_at: string, lane: TrainingLane }` — appended on successful promotion.

## Success Criteria

### Measurable Outcomes

- **SC-001**: `GET /operator/telemetry/config` returns 200 with correct shape in both local and Fly.io environments.
- **SC-002**: KnowledgePage renders without TypeScript errors after `TrainingMetricCard` is imported from `@banana/ui`.
- **SC-003**: `computeCosineDrift` unit test passes for equal vectors (→ 0) and orthogonal vectors (→ 1).
- **SC-004**: BananaAIPage compiles cleanly with `RipenessResult` from `@banana/ui` (no `as any` casts).
- **SC-005**: FunctionsPage shows a promotion audit entry immediately after a successful promotion call.

## Assumptions

- The workbench training history endpoint continues to return `TrainingRunResult[]` under the `rows` key.
- Centroid embeddings are placeholder zeros until the native layer is updated in a future slice.
- `@banana/ui` exports follow the existing pattern in `src/typescript/shared/ui/src/index.ts`.
- The ASP.NET `AuditStore` pattern can be reused for the telemetry config endpoint without a new DB table.


## In Scope *(mandatory)*

<!--
  ACTION REQUIRED: List the specific outcomes included in this feature slice.
  Keep these bounded and directly tied to user value.
-->

- [In-scope outcome 1]
- [In-scope outcome 2]

## Out of Scope *(mandatory)*

<!--
  ACTION REQUIRED: List explicit exclusions for this slice.
  Use this section to prevent implementation drift.
-->

- [Out-of-scope item 1]
- [Out-of-scope item 2]

## User Scenarios & Testing *(mandatory)*

<!--
  IMPORTANT: User stories should be PRIORITIZED as user journeys ordered by importance.
  Each user story/journey must be INDEPENDENTLY TESTABLE - meaning if you implement just ONE of them,
  you should still have a viable MVP (Minimum Viable Product) that delivers value.

  Assign priorities (P1, P2, P3, etc.) to each story, where P1 is the most critical.
  Think of each story as a standalone slice of functionality that can be:
  - Developed independently
  - Tested independently
  - Deployed independently
  - Demonstrated to users independently
-->

### User Story 1 - [Brief Title] (Priority: P1)

[Describe this user journey in plain language]

**Why this priority**: [Explain the value and why it has this priority level]

**Independent Test**: [Describe how this can be tested independently - e.g., "Can be fully tested by [specific action] and delivers [specific value]"]

**Acceptance Scenarios**:

1. **Given** [initial state], **When** [action], **Then** [expected outcome]
2. **Given** [initial state], **When** [action], **Then** [expected outcome]

---

### User Story 2 - [Brief Title] (Priority: P2)

[Describe this user journey in plain language]

**Why this priority**: [Explain the value and why it has this priority level]

**Independent Test**: [Describe how this can be tested independently]

**Acceptance Scenarios**:

1. **Given** [initial state], **When** [action], **Then** [expected outcome]

---

### User Story 3 - [Brief Title] (Priority: P3)

[Describe this user journey in plain language]

**Why this priority**: [Explain the value and why it has this priority level]

**Independent Test**: [Describe how this can be tested independently]

**Acceptance Scenarios**:

1. **Given** [initial state], **When** [action], **Then** [expected outcome]

---

[Add more user stories as needed, each with an assigned priority]

### Edge Cases

<!--
  ACTION REQUIRED: The content in this section represents placeholders.
  Fill them out with the right edge cases.
-->

- What happens when [boundary condition]?
- How does system handle [error scenario]?

## Requirements *(mandatory)*

<!--
  ACTION REQUIRED: The content in this section represents placeholders.
  Fill them out with the right functional requirements.
-->

### Functional Requirements

- **FR-001**: System MUST [specific capability, e.g., "allow users to create accounts"]
- **FR-002**: System MUST [specific capability, e.g., "validate email addresses"]
- **FR-003**: Users MUST be able to [key interaction, e.g., "reset their password"]
- **FR-004**: System MUST [data requirement, e.g., "persist user preferences"]
- **FR-005**: System MUST [behavior, e.g., "log all security events"]

*Example of marking unclear requirements:*

- **FR-006**: System MUST authenticate users via [NEEDS CLARIFICATION: auth method not specified - email/password, SSO, OAuth?]
- **FR-007**: System MUST retain user data for [NEEDS CLARIFICATION: retention period not specified]

### Key Entities *(include if feature involves data)*

- **[Entity 1]**: [What it represents, key attributes without implementation]
- **[Entity 2]**: [What it represents, relationships to other entities]

## Success Criteria *(mandatory)*

<!--
  ACTION REQUIRED: Define measurable success criteria.
  These must be technology-agnostic and measurable.
-->

### Measurable Outcomes

- **SC-001**: [Measurable metric, e.g., "Users can complete account creation in under 2 minutes"]
- **SC-002**: [Measurable metric, e.g., "System handles 1000 concurrent users without degradation"]
- **SC-003**: [User satisfaction metric, e.g., "90% of users successfully complete primary task on first attempt"]
- **SC-004**: [Business metric, e.g., "Reduce support tickets related to [X] by 50%"]

## Assumptions

<!--
  ACTION REQUIRED: The content in this section represents placeholders.
  Fill them out with the right assumptions based on reasonable defaults
  chosen when the feature description did not specify certain details.
-->

- [Assumption about target users, e.g., "Users have stable internet connectivity"]
- [Assumption about scope boundaries, e.g., "Mobile support is out of scope for v1"]
- [Assumption about data/environment, e.g., "Existing authentication system will be reused"]
- [Dependency on existing system/service, e.g., "Requires access to the existing user profile API"]
