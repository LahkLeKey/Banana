# Feature Specification: Player Controller Foundation

**Feature Branch**: `[048-player-controller-foundation]`
**Created**: 2026-05-10
**Status**: Draft
**Input**: User description: "lets add a new spec for player controllers and related features that we are doing for this"

## In Scope *(mandatory)*

- Establish a player controller feature slice where keyboard movement is the only active movement method.
- Provide a deterministic player movement contract for WASD and Arrow keys across supported viewport sizes.
- Introduce a reusable right-click custom context menu surface for future gameplay actions without triggering movement.
- Define interaction-action placeholders that can be bound to gameplay features in later slices.
- Define acceptance and success criteria for controller responsiveness, menu behavior, and user workflow continuity.

## Out of Scope *(mandatory)*

- Mouse-driven movement, click-to-move pathing, or terrain-based click navigation.
- Combat, harvesting, inventory mutation, quest progression, or AI behavior changes beyond controller integration points.
- Multiplayer input synchronization and networking concerns.
- Controller/gamepad support.

## User Scenarios & Testing *(mandatory)*

### Test Organization

- Organize tests by behavior slice: keyboard movement, context menu behavior, and controller arbitration rules.
- Keep movement verification and menu verification in separate suites so each story remains independently testable.
- Include regression tests proving that right-click no longer affects movement while keyboard movement remains functional.
- Require feature-level validation that all acceptance scenarios pass before plan/task progression.

### User Story 1 - Keyboard Movement Reliability (Priority: P1)

As a player, I can move the banana character with WASD or Arrow keys so I can explore the map without being blocked by unresolved mouse behavior.

**Why this priority**: Movement continuity is required for all subsequent gameplay loops and unblocks all forward development.

**Independent Test**: This story is complete when a user can move in four directions with both key sets and stop immediately when keys are released.

**Acceptance Scenarios**:

1. **Given** the game viewport is active, **When** I hold W, **Then** the player moves forward until I release W.
2. **Given** the game viewport is active, **When** I hold ArrowLeft, **Then** the player moves left until I release ArrowLeft.
3. **Given** movement is active, **When** I release all movement keys, **Then** the player stops within one frame budget.

---

### User Story 2 - Custom Right-Click Menu (Priority: P1)

As a player, I can open a custom context menu with right-click so I have a visible action entry point for future gameplay interactions.

**Why this priority**: It preserves right-click intent while avoiding movement coupling and provides a stable UX surface for upcoming features.

**Independent Test**: This story is complete when right-click opens a custom menu, clicking outside closes it, and Escape closes it.

**Acceptance Scenarios**:

1. **Given** the game viewport is visible, **When** I right-click anywhere on the viewport, **Then** a custom action menu appears at a valid on-screen position.
2. **Given** the custom menu is open, **When** I click outside the menu, **Then** the menu closes.
3. **Given** the custom menu is open, **When** I press Escape, **Then** the menu closes.

---

### User Story 3 - Controller Arbitration Rules (Priority: P2)

As a developer and tester, I can rely on explicit input arbitration rules so movement and menu interactions do not conflict.

**Why this priority**: Prevents regression loops and avoids ambiguous control behavior during later feature expansion.

**Independent Test**: This story is complete when keyboard movement remains active while mouse movement remains disabled and right-click only affects menu visibility.

**Acceptance Scenarios**:

1. **Given** keyboard movement is functioning, **When** I right-click, **Then** menu state changes without changing movement direction.
2. **Given** the menu is open, **When** I press movement keys, **Then** movement remains responsive and menu closes according to defined behavior.

---

### Edge Cases

- What happens when right-click is invoked near viewport edges where a menu could overflow the visible area?
- How does the system behave when rapid alternating input occurs between right-click and movement keys?
- What happens when the viewport loses focus while movement keys are held?

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: System MUST support keyboard-only player movement via WASD and Arrow keys.
- **FR-002**: System MUST ensure movement starts on key press and stops promptly on key release.
- **FR-003**: System MUST ignore mouse input for movement decisions.
- **FR-004**: System MUST open a custom action menu on right-click within the game viewport.
- **FR-005**: System MUST close the custom action menu when users click outside it or press Escape.
- **FR-006**: System MUST keep right-click behavior decoupled from movement.
- **FR-007**: System MUST expose at least two placeholder menu actions that are clearly marked as upcoming features.
- **FR-008**: System MUST keep menu placement within visible viewport bounds.
- **FR-009**: System MUST preserve movement responsiveness while menu features are present.
- **FR-010**: Orchestration MUST run extension-health preflight before execution and surface failures explicitly.
- **FR-011**: Orchestration MUST enforce autonomous continuation only when confidence is >= 80%.
- **FR-012**: Orchestration MUST append heartbeat evidence for each major step with confidence and checkpoint decision.

### Orchestration Contract *(required for Spec Kit workflow slices)*

- **Preflight Command**: `.specify/scripts/bash/spec-extension-preflight.sh --update-first --json`
- **Confidence Gate Command**: `.specify/scripts/bash/spec-confidence-gate.sh --confidence <n> --step "go-copilot-start" --threshold 80 --notes "startup gate"`
- **Pause Rule**: If confidence is < 80%, stop and request human input.
- **Evidence**: Heartbeat entries in `heartbeat-log.md` and validator reports in `artifacts/spec-validation/`.

### Key Entities *(include if feature involves data)*

- **PlayerControllerState**: Represents active movement intent, current input axes, and movement eligibility.
- **ContextMenuState**: Represents menu visibility, anchor position, and currently available action entries.
- **ControllerActionEntry**: Represents a menu action label, availability state, and action intent identifier.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: 100% of movement tests for WASD and Arrow keys pass in local validation.
- **SC-002**: 100% of right-click menu open/close behavior tests pass in local validation.
- **SC-003**: 0 movement updates are triggered by right-click interactions in controller regression tests.
- **SC-004**: At least 90% of internal playtest attempts complete movement + menu interaction flow without controls confusion.

## Assumptions

- Keyboard movement remains the temporary primary movement mode until a later click-movement slice is intentionally reintroduced.
- The right-click custom menu is a UI scaffold and does not need to execute gameplay mutations in this slice.
- Existing camera-follow behavior and terrain rendering remain unchanged by this feature slice.
- Future action features will reuse the context menu surface introduced here instead of native browser menus.
