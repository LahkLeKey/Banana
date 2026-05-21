# Feature Specification: Steamworks Release Readiness for Banana Engineer

**Feature Branch**: `feature/steamworks`

**Created**: 2026-05-21

**Status**: Draft

**Input**: User description: "Steamworks App Admin release progress, store presence and game build checklist gaps, package pricing state, and target release date constraints"

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Complete Release Blocking Items (Priority: P1)

As a release owner, I need a single actionable checklist of all Steamworks blockers so the game can pass Steam review and become eligible for launch.

**Why this priority**: Without resolving blockers, the app cannot be reviewed or released regardless of all other preparation work.

**Independent Test**: Can be tested by reviewing the readiness checklist and confirming each blocker has an explicit owner, status, and completion evidence.

**Acceptance Scenarios**:

1. **Given** the Steamworks app page shows incomplete Store Presence and Game Build sections, **When** the release owner opens the readiness tracker, **Then** each missing item is listed with a required action and expected completion output.
2. **Given** a checklist item is completed, **When** it is updated in the tracker, **Then** the item status changes to complete and links to verification evidence.

---

### User Story 2 - Submit Correct Review Sequence (Priority: P2)

As a release owner, I need to submit Store Presence review and Build review in the correct order and state so Valve review cycles are not delayed by preventable omissions.

**Why this priority**: Incorrect sequencing causes avoidable re-review delays and directly risks missing the target launch window.

**Independent Test**: Can be tested by following a documented sequence and confirming all submission prerequisites are satisfied before each submission.

**Acceptance Scenarios**:

1. **Given** all required Store Presence checklist items are complete, **When** the owner performs pre-submit validation, **Then** Store Presence is marked ready for review submission.
2. **Given** a playable build and depot requirements are complete, **When** build pre-submit validation runs, **Then** the build is marked ready for review submission.

---

### User Story 3 - Validate Launch Date Eligibility (Priority: P3)

As a release owner, I need to validate launch constraints (Coming Soon duration and app-credit timing) against the planned release timestamp so we do not schedule an impossible launch date.

**Why this priority**: Date constraints can invalidate a planned launch even when checklists are complete.

**Independent Test**: Can be tested by comparing configured release date to policy constraints and obtaining an explicit eligible/not-eligible result.

**Acceptance Scenarios**:

1. **Given** a planned release datetime, **When** launch eligibility is evaluated, **Then** the system reports whether all Steam launch timing constraints are met.
2. **Given** one or more timing constraints are unmet, **When** eligibility is evaluated, **Then** the system reports the earliest valid release date.

---

## Edge Cases

- A checklist item appears complete in one Steamworks panel but remains incomplete in another linked panel.
- Package pricing exists in draft but is not approved/published for any purchase package.
- Platform support in Steamworks config diverges from store page declarations.
- Build review passes but Store Presence review fails and requires another submission cycle.
- Release date is configured, but Coming Soon visibility has not reached the minimum duration.

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: The release readiness process MUST maintain a consolidated list of all required Steam Store Presence and Game Build checklist items.
- **FR-002**: Each checklist item MUST include a status (`not-started`, `in-progress`, `blocked`, `complete`) and owner.
- **FR-003**: The process MUST track review prerequisites separately for Store Presence and Game Build and prevent marking a section ready when required items are incomplete.
- **FR-004**: The process MUST track package pricing readiness for at least one purchasable package and indicate whether pricing is approved/published.
- **FR-005**: The process MUST confirm the uploaded build is playable and aligned with the features and platform support declared on the store page.
- **FR-006**: The process MUST verify launch eligibility against Steam timing constraints (minimum Coming Soon duration and minimum days since first app-credit purchase).
- **FR-007**: The process MUST report the earliest valid release date when the configured release date is ineligible.
- **FR-008**: The process MUST retain evidence references for completed items (for example screenshots, links, or admin panel confirmations).

### Key Entities *(include if feature involves data)*

- **Readiness Item**: A required release task with category, owner, status, due date, and evidence links.
- **Review Submission Gate**: A logical gate for Store Presence or Build submission that is only open when all required items are complete.
- **Release Constraint**: A date-based launch rule with input dates, validation result, and earliest eligible datetime.
- **Package Pricing Record**: Package state including visibility, pricing approval status, and discount configuration.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: 100% of required Store Presence checklist items are tracked with owner and status before Store review submission.
- **SC-002**: 100% of required Game Build checklist items are tracked with owner and status before Build review submission.
- **SC-003**: Review submissions are attempted only when all mandatory prerequisites are complete, with zero preventable rejections due to missing checklist items.
- **SC-004**: Launch eligibility checks correctly identify whether the configured release datetime is valid and provide an earliest valid date when invalid.
- **SC-005**: At least one package has approved and published pricing before release readiness is marked complete.

## Assumptions

- The current focus is release operations and readiness governance rather than runtime Steamworks SDK integration in game code.
- Steamworks checklists and policy constraints shown in App Admin are the source of truth for readiness decisions.
- A release owner role exists and can coordinate required teams for assets, build, pricing, and compliance tasks.
- Evidence can be captured as links or references to Steamworks admin pages and internal release artifacts.
