# Data Model: Player Controller Foundation

## Entity: PlayerControllerState

- Purpose: Represents authoritative movement intent consumed by the runtime movement loop.
- Fields:
  - activeAxesX: number (-1, 0, 1)
  - activeAxesZ: number (-1, 0, 1)
  - movementEnabled: boolean
  - lastInputSource: enum (`keyboard`, `none`)
- Validation Rules:
  - Axis values must remain clamped to [-1, 1].
  - Movement updates are applied only when movementEnabled is true.
  - Mouse/context-menu events must not change activeAxesX/activeAxesZ.
- State Transitions:
  - Idle -> Moving: any movement key down
  - Moving -> Idle: all movement keys up

## Entity: ContextMenuState

- Purpose: Represents right-click action menu UI state independent of movement.
- Fields:
  - visible: boolean
  - anchorX: number (viewport pixel position)
  - anchorY: number (viewport pixel position)
  - openedAt: timestamp (optional diagnostic)
- Validation Rules:
  - anchorX/anchorY must be clamped within viewport-safe bounds.
  - Menu visibility toggles only through right-click/open and explicit close actions.
  - Menu state changes must not mutate PlayerControllerState axes.
- State Transitions:
  - Hidden -> Visible: right-click in viewport
  - Visible -> Hidden: outside click, Escape, explicit close action

## Entity: ControllerActionEntry

- Purpose: Describes an action item rendered in the custom context menu.
- Fields:
  - id: string
  - label: string
  - enabled: boolean
  - intent: enum (`interact`, `harvest`, `close`, `future-action`)
- Validation Rules:
  - id values are unique within a single menu render.
  - Disabled entries are visible but non-executing.
  - Close intent always remains available while menu is visible.

## Relationship Summary

- ContextMenuState renders a list of ControllerActionEntry items.
- PlayerControllerState is independent of ContextMenuState and must not be directly mutated by menu action selection in this slice.

## Edge-Case Handling Notes

- Viewport Bounds: Context menu anchor values are clamped to a safe in-viewport rectangle so menu content remains visible near edges.
- Rapid Alternation: Input events are processed with keyboard intent as movement authority and right-click as menu authority; no event path may write both state machines in one branch.
- Focus Loss: On blur/focus-loss, `PlayerControllerState` transitions to idle by clearing active key state; menu state remains explicit and user-driven.
- Escape Priority: Escape closes menu first (when visible) and does not emit movement updates.
- Outside Click Priority: Outside-click close only affects `ContextMenuState.visible` and leaves movement axes unchanged.
