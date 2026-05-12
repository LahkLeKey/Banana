# Feature Specification: ARPG View and Actor Scaffolding

**Feature Branch**: `[049-arpg-view-actors]`
**Created**: 2026-05-11
**Status**: Draft
**Input**: User description: "we are going to want to anchor the banana / terrian to the center of the screen, it should feel like ARPG movement but with wasd; make a follow up spec for the basic ARPG view and finish the context menu / new actors outside of the player banana"

## Brand Assets

- **splash.png** — canonical Banana Engine splash image. Used as the full-screen loading overlay while the WASM engine initializes. Copy lives at `src/typescript/react/public/splash.png`.

## In Scope *(mandatory)*

- Re-scaffold the base playable scene so the player and terrain are visually centered in a stable ARPG-style follow-camera view.
- Define deterministic camera behavior for keyboard movement so player traversal feels anchored and legible across viewport sizes.
- Complete the custom context menu as an interaction surface for nearby world actors.
- Add non-player actor participation in the base scene so interaction targets exist beyond the player banana.
- Define actor interaction flow from right-click context menu intent through visible world-state outcomes.
- Define acceptance and success criteria for scene framing, movement readability, menu usability, and non-player actor coverage.

## Out of Scope *(mandatory)*

- Combat systems, damage calculation, loot drops, inventory persistence, or quest progression.
- Multiplayer synchronization, authoritative server movement, or networked actor simulation.
- Full tactical pathfinding, click-to-move locomotion, or navigation mesh systems.
- Deep narrative systems, dialogue trees, or economy simulation.

## User Scenarios & Testing *(mandatory)*

### Test Organization

- Organize tests into four behavior suites: ARPG camera framing, keyboard movement framing continuity, context menu interactions, and non-player actor interaction outcomes.
- Keep camera-framing tests independent from actor-interaction tests so each user story remains independently testable.
- Include viewport-size coverage that validates behavior at small, medium, and wide layout ratios.
- Include regression tests ensuring movement remains responsive while menu and actor interactions are present.
- Require complete pass of acceptance scenarios before moving to planning/task execution for this feature.

### User Story 1 - Centered ARPG Framing (Priority: P1)

As a player, I can move the banana while the scene remains centered and readable so movement feels like an ARPG rather than a corner-anchored prototype.

**Why this priority**: Camera framing and spatial readability are foundational to all movement and interaction loops.

**Independent Test**: This story is complete when keyboard movement keeps the player in a centered follow region and terrain appears correctly framed at runtime.

**Acceptance Scenarios**:

1. **Given** the scene is loaded, **When** the player starts moving with WASD, **Then** the player remains in the defined centered follow region instead of drifting to a screen corner.
2. **Given** the player changes movement direction repeatedly, **When** the camera updates, **Then** framing remains stable without disorienting jumps.
3. **Given** different viewport sizes are used, **When** movement is tested, **Then** the centered follow behavior remains consistent.

---

### User Story 2 - Completed Context Menu Interactions (Priority: P1)

As a player, I can use right-click context actions on relevant world targets so the interaction loop is complete and not placeholder-only.

**Why this priority**: Interaction affordance is required to validate the game loop beyond movement.

**Independent Test**: This story is complete when the context menu presents actionable options and at least one action results in visible world feedback.

**Acceptance Scenarios**:

1. **Given** a targetable actor is in range, **When** the player right-clicks, **Then** the context menu shows target-relevant actions.
2. **Given** the context menu is open, **When** the player selects a valid action, **Then** the menu closes and the action outcome is reflected in the scene.
3. **Given** no valid target is available, **When** the player right-clicks, **Then** menu behavior communicates unavailable actions without breaking controls.

---

### User Story 3 - Non-Player Actor Presence and Response (Priority: P2)

As a player, I can see and interact with actors other than my banana so the scene feels like a playable world rather than a single-entity demo.

**Why this priority**: Additional actors are required to validate interaction design and prepare future gameplay loops.

**Independent Test**: This story is complete when non-player actors spawn reliably, are visually distinguishable, and respond to defined interaction intents.

**Acceptance Scenarios**:

1. **Given** the scene initializes, **When** actor spawning runs, **Then** at least one non-player actor type appears in the playable area.
2. **Given** a non-player actor is present, **When** the player attempts an applicable context action, **Then** actor state or behavior changes according to the action contract.
3. **Given** multiple actors are visible, **When** the player interacts with one actor, **Then** only the intended actor receives the interaction outcome.

---

### Edge Cases

- What happens when the player reaches terrain bounds while camera follow behavior remains centered?
- How does the context menu behave when the selected actor exits interaction range before action confirmation?
- What happens when overlapping actors compete for right-click selection in dense scenes?
- How does the system handle rapid alternating input between movement keys and repeated context-menu open/close actions?

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: System MUST maintain an ARPG-style follow-camera framing contract that keeps the player within a centered on-screen region during keyboard movement.
- **FR-002**: System MUST keep terrain/world framing aligned with player-centric camera behavior so the environment reads as centered and navigable.
- **FR-003**: System MUST preserve responsive WASD and Arrow-key movement while camera follow behavior is active.
- **FR-004**: System MUST provide a context menu that exposes actionable options for valid interaction targets.
- **FR-005**: System MUST execute at least one context-menu action that produces visible world-state feedback.
- **FR-006**: System MUST represent unavailable actions in a non-breaking manner when no valid target exists.
- **FR-007**: System MUST include non-player actors in the base scene and keep their presence stable across scene initialization.
- **FR-008**: System MUST ensure actor-targeted actions are applied only to the intended actor.
- **FR-009**: System MUST keep movement, camera follow, and context interactions operational during the same gameplay session without control deadlocks.
- **FR-010**: Orchestration MUST run extension-health preflight before execution and surface failures explicitly.
- **FR-011**: Orchestration MUST enforce autonomous continuation only when confidence is >= 80%.
- **FR-012**: Orchestration MUST append heartbeat evidence for each major step with confidence and checkpoint decision.

### Orchestration Contract *(required for Spec Kit workflow slices)*

- **Preflight Command**: `.specify/scripts/bash/spec-extension-preflight.sh --update-first --json`
- **Confidence Gate Command**: `.specify/scripts/bash/spec-confidence-gate.sh --confidence <n> --step "go-copilot-start" --threshold 80 --notes "startup gate"`
- **Pause Rule**: If confidence is < 80%, stop and request human input.
- **Evidence**: Heartbeat entries in `heartbeat-log.md` and validator reports in `artifacts/spec-validation/`.

### Key Entities *(include if feature involves data)*

- **ARPGCameraState**: Represents camera follow anchor, centered framing window, and runtime viewport-relative framing state.
- **ContextInteractionMenu**: Represents visibility, target context, action availability, and selected action intent.
- **WorldActor**: Represents non-player scene participants with actor type, interaction eligibility, and current interaction state.
- **InteractionOutcome**: Represents the observable result of an action applied to a target actor.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: 100% of camera-framing acceptance tests pass for small, medium, and wide viewport ratios.
- **SC-002**: 100% of movement responsiveness tests pass while camera follow and context menu logic are both active.
- **SC-003**: 100% of context-menu action tests pass for both valid-target and no-target interaction paths.
- **SC-004**: At least one non-player actor type is present in all scene initialization test runs, with interaction targeting accuracy of 100% in actor-selection tests.
- **SC-005**: At least 90% of internal playtest runs complete a movement-plus-interaction loop without reported framing confusion.

## Assumptions

- Keyboard movement remains the primary locomotion mode for this feature slice.
- Interaction outcomes in this slice can be lightweight world-state responses and do not require full progression systems.
- Existing rendering and controller infrastructure will be extended rather than replaced.
- Non-player actors introduced in this slice establish baseline interaction contracts for future gameplay features.