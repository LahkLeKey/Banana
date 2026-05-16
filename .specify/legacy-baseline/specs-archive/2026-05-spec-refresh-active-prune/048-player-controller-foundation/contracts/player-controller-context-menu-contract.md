# Contract: Player Controller and Context Menu

## Scope

Defines behavior boundaries between player movement controls and custom right-click action menu for feature slice `048-player-controller-foundation`.

## Controller Input Contract

- Movement is accepted from keyboard inputs only:
  - WASD
  - Arrow keys
- Right-click input does not set movement targets or movement vectors.
- Releasing all movement keys must return movement intent to idle.

## Context Menu Contract

- Right-click in viewport opens custom menu at bounded viewport coordinates.
- Bounded coordinate policy clamps the anchor to viewport-safe bounds with minimum padding:
  - `x = clamp(clientX, 8, viewportWidth - 220 - 8)`
  - `y = clamp(clientY, 8, viewportHeight - 140 - 8)`
- Menu must include at least:
  - Interact (placeholder)
  - Harvest (placeholder)
  - Close
- Placeholder actions remain disabled in this slice and are labeled as coming soon.
- Menu closes on:
  - Outside click
  - Escape key
  - Explicit Close action

## Arbitration Contract

- Menu open/close events cannot modify movement axes.
- Movement key events can occur while menu exists; movement remains responsive.
- Menu visibility and movement state are separate state machines with explicit synchronization rules only where documented.

## Arbitration Invariants

- Invariant A: Right-click input changes only `ContextMenuState.visible` and anchor coordinates.
- Invariant B: `PlayerControllerState.activeAxesX` and `activeAxesZ` can only be mutated by keyboard keydown/keyup processing.
- Invariant C: Menu close events (outside click, Escape, Close action) do not change movement vectors.
- Invariant D: Focus-loss recovery clears active keyboard intent to idle and does not auto-open or auto-close menu.
- Invariant E: Placeholder menu actions are non-movement actions and cannot call movement APIs in this slice.

## Regression Contract

- Any future feature that introduces mouse-driven movement must be implemented in a separate spec slice.
- This contract remains authoritative until superseded by a planned and approved movement-control spec.
