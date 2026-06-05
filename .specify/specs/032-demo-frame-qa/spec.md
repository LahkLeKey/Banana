# Feature Specification: Demo Frame QA Evidence

**Feature Branch**: `[033-unify-coherent-world]`

**Created**: 2026-06-03

**Status**: Draft

**Input**: User description: "we want to be able to export demo frames as well when we are running tests so we can verify visually with some extra developer context for human review, right now our human qa workflow is non existant we need to fix that, use spec kit to scaffold this requirement and checklists"

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Visual QA evidence from demo tests (Priority: P1)

As a QA reviewer, I want demo test runs to export visual frames with contextual metadata so I can verify the experience visually and review it without re-running the tests.

**Why this priority**: This is the primary gap in the current human QA workflow and directly enables visual verification.

**Independent Test**: Run a demo test scenario with visual export enabled and confirm a review-ready frame bundle exists with required context.

**Acceptance Scenarios**:

1. **Given** a demo test run completes, **When** visual export is enabled, **Then** the run produces a frame bundle with frame images and contextual metadata for review.
2. **Given** a QA reviewer opens the frame bundle, **When** they inspect it, **Then** each frame is accompanied by scenario, timestamp, and run identity context.

---

### User Story 2 - Developers control visual capture scope (Priority: P2)

As a developer, I want to control which demo scenarios and capture frequency are used so the evidence is meaningful without excessive noise.

**Why this priority**: Developers need a predictable, configurable export to align capture with the scenarios under review.

**Independent Test**: Configure a test run to capture a limited scenario set and verify only the configured scenarios produce frames.

**Acceptance Scenarios**:

1. **Given** multiple demo scenarios are available, **When** a subset is configured for capture, **Then** only that subset exports frames.
2. **Given** capture frequency is configured, **When** a demo run executes, **Then** exported frames follow the configured cadence.

---

### User Story 3 - Human QA review workflow exists (Priority: P3)

As a QA lead, I want a documented review checklist and evidence packaging expectations so human review is consistent and repeatable.

**Why this priority**: The team needs a defined workflow to perform and record visual QA reviews.

**Independent Test**: Review a demo test bundle using the checklist and confirm all required review fields can be completed.

**Acceptance Scenarios**:

1. **Given** a new test run is available, **When** a reviewer follows the checklist, **Then** the review results capture required observations and decisions.

---

### Edge Cases

- What happens when visual capture fails mid-run or produces zero frames?
- How does the system handle insufficient storage or output path failures?
- What happens when visual export is disabled but tests still need to pass?

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: System MUST export visual frames from configured demo test runs when visual QA export is enabled.
- **FR-002**: System MUST associate each exported frame with contextual metadata including scenario name, timestamp, and run identity.
- **FR-003**: System MUST allow configuration of capture scope (which scenarios) and capture cadence for demo test runs.
- **FR-004**: System MUST store exported frames and metadata in a predictable, reviewable location with defined retention and cleanup expectations.
- **FR-005**: System MUST surface visual export failures in test outcomes so missing evidence is not silent.
- **FR-006**: System MUST provide a documented human QA review workflow and checklist for visual verification.
- **FR-007**: System MUST support running demo tests with visual export disabled without altering non-visual test outcomes.

### Key Entities *(include if feature involves data)*

- **Frame Bundle**: A collection of exported frames plus associated metadata for a single test run.
- **Capture Context**: Metadata describing the scenario, timing, and run identity tied to each exported frame.
- **QA Review Record**: The documented outcome of human review for a given frame bundle.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: 100% of configured demo test runs produce a frame bundle with metadata when visual export is enabled.
- **SC-002**: Reviewers can locate and open the frame bundle for a completed run in under 2 minutes using the documented workflow.
- **SC-003**: Each release candidate has at least one completed QA review record tied to a frame bundle.
- **SC-004**: Visual export failures are visible in test reports for every affected run.

## Assumptions

- Visual frame capture can be performed without altering the core simulation outcome.
- Existing test orchestration can surface additional artifacts for review.
- Reviewers have access to the storage location where evidence is published.
- The initial scope targets demo scenarios used in current automated test runs.

## Constitution Alignment *(mandatory)*

- **Disclosure Integrity**: No player-facing claims are changed; internal QA evidence does not affect public disclosures.
- **Cross-Domain Contracts**: Impacts native runtime test execution, test orchestration, and artifact publishing policies.
- **Quality Gates**: Visual export success is required for designated demo test runs; human QA review completion is required before release sign-off.
- **Delivery Evidence**: Frame bundles, metadata manifests, and QA review records provide reproducible evidence for supported runtime channels.

## Storefront & Release Claims *(required when public Steam copy is affected)*

- **Storefront Copy Contract**: Not applicable.
- **AI Disclosure**: Not applicable.
- **Controller Support**: Not applicable.
- **System Requirements**: Not applicable.
- **Store Assets**: Not applicable.
