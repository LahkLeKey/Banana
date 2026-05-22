# Feature Specification: PBJ Pickup Celebration and Steam Release Tracking

**Feature Branch**: `003-pbj-pickup-steam-release-flow`

**Created**: 2026-05-22

**Status**: Draft

**Input**: User description: "Add a peanut butter jelly time pickupable object with dancing banana particles, then scaffold bulk specs/todos and release tracking steps for Steam publish flow and PR completion."

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Pick Up PBJ Celebration Object (Priority: P1)

As a player, I want a pickupable PBJ celebration object in the gameplay runtime so I can trigger a fun reward moment during play.

**Why this priority**: This is the direct gameplay request and visible player-facing value.

**Independent Test**: A player can approach the PBJ pickup marker, trigger pickup, and receive immediate visual confirmation.

**Acceptance Scenarios**:

1. **Given** the player is in the overworld gameplay viewport, **When** the player moves within pickup range and activates pickup, **Then** the PBJ object is consumed for that session.
2. **Given** the PBJ object has been consumed, **When** the player remains in-session, **Then** the object is no longer available to re-collect.

---

### User Story 2 - Trigger Dancing Banana Celebration FX (Priority: P1)

As a player, I want a dancing banana visual burst after pickup so the PBJ event feels rewarding and obvious.

**Why this priority**: The celebration effect confirms state change and fulfills the requested thematic experience.

**Independent Test**: After pickup, the client renders a short-lived particle burst that is visible without blocking core controls.

**Acceptance Scenarios**:

1. **Given** the PBJ pickup is successful, **When** the celebration starts, **Then** dancing banana particles render across the screen for a bounded duration.
2. **Given** active gameplay controls, **When** particle effects run, **Then** movement and HUD interactions remain responsive.

---

### User Story 3 - Track Steam Publish and PR Release State (Priority: P2)

As a release owner, I want a repeatable tracked workflow for Steam publish + PR/release progress so local and CI execution are consistent and auditable.

**Why this priority**: The Steam path is currently operational but fragile; explicit tracking lowers release risk.

**Independent Test**: Team members can follow tasks/checklists to run publish lanes, record build evidence, and close PR/release steps with consistent artifacts.

**Acceptance Scenarios**:

1. **Given** a dev-candidate build is available, **When** release tasks are executed locally or through CI dispatch, **Then** publish progress and resulting BuildID/ManifestID are recorded.
2. **Given** a release-tracking checklist, **When** a PR is prepared for merge, **Then** required Steam, test, and documentation evidence is attached before closeout.

---

## Edge Cases

- Player snapshot data is delayed and pickup distance cannot be calculated immediately.
- Player attempts to pick up while outside pickup radius.
- Particle FX are triggered repeatedly due to duplicate interaction events.
- Steam upload succeeds but library launch remains blocked due configuration mismatches.
- CI publish lane is triggered without expected environment secrets or depot package readiness.

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: The gameplay runtime MUST expose exactly one PBJ pickup interaction point per session viewport load.
- **FR-002**: The PBJ pickup MUST only be collectible when the local player is within a defined world-space radius.
- **FR-003**: On successful pickup, the system MUST mark the PBJ pickup unavailable until session reset/reload.
- **FR-004**: On successful pickup, the UI MUST trigger a visible dancing banana celebration effect with finite duration.
- **FR-005**: The celebration effect MUST be non-blocking for core gameplay input and HUD interactions.
- **FR-006**: Release workflow artifacts MUST define local and CI publish steps for Steam candidate delivery and evidence capture.
- **FR-007**: Release tracking tasks MUST include BuildID/ManifestID capture, launch-validation status, and blocking issue documentation.
- **FR-008**: PR closeout guidance MUST require explicit linkage between implementation tasks and release evidence.

### Key Entities *(include if feature involves data)*

- **PBJ Pickup State**: Session-scoped availability state for the collectible PBJ object.
- **Local Player Position**: Snapshot-driven coordinates used to evaluate pickup eligibility.
- **Celebration Particle Event**: UI event containing particle glyphs, timing, and animation metadata.
- **Steam Release Evidence Record**: Structured notes for BuildID, ManifestID, launch validation status, and blockers.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: 100% of pickup attempts inside the configured radius trigger exactly one successful PBJ collection event per session.
- **SC-002**: 100% of successful pickups render celebration particles for a bounded duration under 5 seconds.
- **SC-003**: No critical gameplay input regression is observed during the PBJ celebration in manual smoke verification.
- **SC-004**: Steam publish/release checklist execution yields complete evidence (BuildID, ManifestID, launch result, blockers) for each candidate cycle.
- **SC-005**: PR closeout includes linked release evidence and open-risk notes before merge decision.

## Assumptions

- Existing snapshot payloads continue to provide stable local player coordinates.
- PBJ pickup state can remain client/session scoped for this implementation slice.
- Current Steam upload scripts remain the execution entry points while SteamCMD instability persists on affected hosts.
- Release tracking docs live under `.specify/specs/003-pbj-pickup-steam-release-flow` and are updated as execution proceeds.

## Constitution Alignment *(mandatory)*

- **Disclosure Integrity**: PBJ behavior and release state claims must match visible runtime behavior and recorded evidence.
- **Cross-Domain Contracts**: Gameplay UI additions must preserve API snapshot and native runtime expectations without changing payload shape.
- **Quality Gates**: Gameplay smoke checks plus release workflow validation must pass before merge recommendation.
- **Delivery Evidence**: Candidate publish and launch outcomes must be documented with reproducible commands and artifacts.
