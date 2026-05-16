# Feature Specification: ARPG View and Actor Scaffolding

**Feature Branch**: `[049-arpg-view-actors]`
**Created**: 2026-05-11
**Status**: Draft → **In Development (Component Refactor + Multiplayer Foundation Phase 3)**
**Input**: User description: "we are going to want to anchor the banana / terrian to the center of the screen, it should feel like ARPG movement but with wasd; make a follow up spec for the basic ARPG view and finish the context menu / new actors outside of the player banana"

## Architecture Update - Shared Component Refactor (Phase 1 ✅ COMPLETE)

**Cross-Platform UI Layer** *(completed 2026-05-15)*:
- ✅ Extracted game engine UI components into domain-driven, reusable modules in `@banana/ui`
- ✅ Implemented SOLID principles: Single Responsibility (component per feature), Dependency Inversion (type-driven APIs)
- ✅ Designed for consumption by React (web), React Native (mobile), and Electron (desktop)
- ✅ No platform-specific code in shared components; pure presentational logic and domain types
- ✅ **Components Extracted**:
  - `MobileRadialControl` — Touch-driven radial input widget (Liskov Substitution with desktop menu)
  - `ContextMenu` — Cross-platform interaction menu (Interface Segregation: minimal props contract)
  - `SplashOverlay` — Loading splash screen animation
  - `GameEngineOverlays` — Error, hint, badge components (Composition over inheritance)
- ✅ **All 19 React component tests passing** (context-menu: 5/5, controller-input: 9/9, controller-arbitration: 5/5)

**Domain-Driven Design Layer** *(completed 2026-05-15)*:
- ✅ `InputDomain` — InputAggregator entity for keyboard/touch input aggregation with priority resolution
  - Follows **Single Responsibility Principle**: manages only input state and aggregation
  - Exports pure functions, no framework dependencies
  - Value Object: `MovementInput` (immutable, comparable)
  
- ✅ `InteractionDomain` — InteractionManager entity for context menu state and action execution
  - Follows **Open/Closed Principle**: extensible action handlers without core modification
  - Follows **Interface Segregation**: ActionHandler function signature, separate outcomes
  - Value Object: `InteractionOutcome` (immutable, serializable)
  
- ✅ `ViewportDomain` — ViewportSizer entity for responsive sizing with mobile fallback chain
  - Follows **Dependency Inversion**: accepts element as dependency, returns pure calculations
  - Mobile-robust: visualViewport → window → documentElement fallback chain
  - Value Object: `ViewportSize` (CSS and device pixel space)
  
- ✅ `GameEngineUIService` — Application Service orchestrating all three domains
  - **State Machine**: idle → loading → running → error/unavailable
  - Publishes state changes to listeners (Observer pattern)
  - Broadcasts only on state mutation (shallow equality check)
  - Minimal, well-defined contract for UI consumption

- ✅ **All 11 domain tests passing** (InputAggregator: 4/4, InteractionManager: 4/4, ViewportSizer: 1/1, GameEngineUIService: 2/2)

## Architecture Decisions & SOLID Application

| Principle | Implementation | Benefit |
|-----------|-----------------|---------|
| **Single Responsibility** | Each domain (Input, Interaction, Viewport) manages one concern | Clear separation enables independent testing and reuse |
| **Open/Closed** | ActionHandler function signature allows new actions without modifying InteractionManager | Extensible game logic without component changes |
| **Liskov Substitution** | MobileRadialControl satisfies same interface as ContextMenu (optional menu item) | Consistent consumption by GameEnginePage |
| **Interface Segregation** | Domains expose minimal, focused props (e.g., `InteractionOutcome` only serializable result) | Clients depend only on what they use |
| **Dependency Inversion** | Services inject dependencies (element, actions); domains don't create them | Testable without framework setup; mockable |

## Brand Assets

- **splash.png** — canonical Banana Engine splash image. Used as the full-screen loading overlay while the WASM engine initializes. Copy lives at `src/typescript/react/public/splash.png`.

## In Scope *(mandatory)*

- ✅ **(COMPLETED Phase 1)** Extract bloated GameEnginePage into reusable shared UI components
  - Mobile radial control component (cross-platform touch input)
  - Context menu component (platform-agnostic interaction surface)
  - Overlay components (splash, error, hints, badges)
  - Centralized domain types and utility functions
  - **Result**: All 19 unit tests passing; ready for React Native/Electron consumption

- ✅ **(COMPLETED Phase 1b)** Implement DDD-inspired domain layer for game engine UI state
  - `InputDomain` — Movement key aggregation and touch gesture recognition ✅
  - `InteractionDomain` — Context menu action routing and outcome propagation ✅
  - `ViewportDomain` — Responsive sizing and frame rate adaptation ✅
  - `GameEngineUIService` — Application service orchestrating all domains ✅
  - **Result**: All 11 domain tests passing; state machine ready for GameEnginePage integration
  
- ✅ **(COMPLETED Phase 2)** Integrate GameEngineUIService into GameEnginePage
  - Refactor event handlers to use InputAggregator
  - Refactor context menu logic to use InteractionManager
  - Refactor viewport sizing to use ViewportSizer
  - Subscribe to state changes for React component updates

- **(Phase 3 - In Progress)** Multiplayer domain foundation
  - `NetcodeDomain` scaffolded with typed input commands and tick snapshots
  - `ReplicationDomain` scaffolded with world state and delta application contracts
  - `PredictionDomain` and `ConnectionManager` introduced as framework-agnostic domain services
  - Shared domain tests expanded and passing in `@banana/ui`
  
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

## Multiplayer Integration (Phase 3)

- **NetcodeDomain** — Client-side input buffering, prediction, and reconciliation (see [Spec 050](../050-multiplayer-server-authority/spec.md))
  - InputBuffer: Queues input commands, tracks server tick ACKs
  - PredictionTracker: Caches predicted outcomes, supports rollback
  - Value objects: InputCommand, TickSnapshot
  - 100% unit test coverage required
- **ReplicationDomain** — State synchronization and delta replication
  - ReplicationService: Tracks replicated entities, applies deltas
  - Value objects: DeltaUpdate, WorldState
  - 100% unit test coverage required
- **GameEnginePage** — Integrates NetcodeDomain and ReplicationDomain for multiplayer
  - Subscribes to ReplicationService for state updates
  - Delegates input to NetcodeDomain before local simulation
  - All 19 React and 11 domain tests must continue to pass

## Spec References
- [Spec 050: Multiplayer with Server-Authoritative Gameplay](../050-multiplayer-server-authority/spec.md)