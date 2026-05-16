# Contract: ARPG Interaction and Framing

## Purpose

Define runtime-facing behavior contracts for centered ARPG framing, context-menu action dispatch, and non-player actor targeting.

## Contract A: Centered Follow Framing

- The runtime MUST maintain player position inside a centered follow region during keyboard movement.
- Camera updates MUST be continuous and stable when movement direction changes.
- Viewport resizing MUST preserve centered framing behavior.

### Acceptance

- Player does not remain pinned to corner-biased framing under normal movement.
- Camera repositioning avoids abrupt frame-to-frame jumps.

## Contract B: Context Menu Target Resolution

- Right-click MUST resolve a target actor context when an eligible actor is under selection criteria.
- Menu action list MUST be derived from target eligibility.
- When no valid target exists, menu MUST represent unavailable actions without breaking controls.

### Acceptance

- Menu options differ by target context where eligibility differs.
- No-target interactions do not dispatch actor mutation outcomes.

## Contract C: Actor-Scoped Action Dispatch

- Selected action MUST apply to exactly one target actor at dispatch time.
- Dispatch MUST be rejected when target actor is no longer valid (inactive/out of range/ineligible).
- Outcome MUST be observable in scene feedback state.

### Acceptance

- Multi-actor scenes do not apply action outcomes to unintended actors.
- Rejected actions return user-visible non-breaking feedback.

## Contract D: Movement and Interaction Arbitration

- Keyboard movement MUST remain responsive while context menu is used.
- Opening or closing context menu MUST NOT inject movement intent.
- Menu interactions MUST NOT deadlock control input.

### Acceptance

- Movement and menu operations can be alternated rapidly without state corruption.
- Input arbitration remains deterministic in repeated interaction cycles.

## Contract E: WASM Bridge Symbol Stability

- Runtime input and interaction entrypoints MUST remain exported in wasm build artifacts.
- The React runtime bridge MUST call only explicit engine contracts for movement tick/input and interaction dispatch.
- Build updates that change exported symbol lists MUST update bridge references in the same slice.

### Acceptance

- Wasm export list includes movement and interaction bridge symbols used by runtime calls.
- Runtime bridge tests fail when expected symbols are missing and pass when contracts are present.