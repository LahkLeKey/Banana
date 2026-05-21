# Research: Player Controller Foundation

## Decision 1: Keyboard-Only Movement Contract

- Decision: Keep movement input contract keyboard-only (WASD + Arrow keys) for this slice.
- Rationale: Removes unresolved mouse-pathing instability and unblocks forward gameplay feature development.
- Alternatives considered:
  - Keep hybrid mouse + keyboard control: rejected due to repeated directional regressions and coupling complexity.
  - Temporarily disable all movement updates: rejected because it would block all gameplay iteration.

## Decision 2: Right-Click Custom Menu as UI Surface Only

- Decision: Right-click opens a custom context menu and does not invoke movement.
- Rationale: Preserves player expectation for action selection while decoupling from movement logic.
- Alternatives considered:
  - Keep browser-native context menu: rejected because gameplay actions need custom entries and controlled behavior.
  - Bind right-click to immediate action without menu: rejected because upcoming features need a multi-action surface.

## Decision 3: Input Arbitration Policy

- Decision: Keyboard movement remains authoritative; menu interactions never mutate movement vectors.
- Rationale: Prevents hidden coupling and keeps player controller deterministic.
- Alternatives considered:
  - Implicitly close menu and reinterpret right-click as movement in some states: rejected due to ambiguity and regression risk.
  - Pause movement whenever menu is open: rejected because it harms responsiveness during rapid action selection.

## Decision 4: Validation Scope

- Decision: Validate with targeted runtime checks in VS Code browser and maintain clear regression checks for movement/menu separation.
- Rationale: Fast feedback and alignment with one-window validation principle.
- Alternatives considered:
  - Broader end-to-end environment-only validation before each change: rejected for iteration speed in this stabilization slice.

## Decision 5: Arbitration Edge-Case Outcomes

- Decision: Define concrete outcomes for rapid mixed-input arbitration and focus transitions.
- Verified outcomes:
  - Right-click during active keyboard movement opens menu only; movement axes remain unchanged until keyup/blur.
  - Keyboard keydown while menu is open preserves movement responsiveness and closes menu without mutating non-keyboard state.
  - Blur and hidden-document transitions clear active key state and emit neutral movement input `(0, 0)`.
  - Mouse bridge calls (`engine_handle_right_click*`) remain inert even when keyboard movement is active.
- Rationale: Explicit outcomes remove ambiguity during future controller/menu changes and reduce regression risk.
- Alternatives considered:
  - Implicitly pausing movement whenever menu is visible: rejected because it breaks responsiveness under rapid alternation.
  - Allowing mouse bridge to set movement targets when keyboard input exists: rejected due to authority conflicts.
