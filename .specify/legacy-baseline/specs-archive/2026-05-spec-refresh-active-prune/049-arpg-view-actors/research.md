# Research: ARPG View and Actor Scaffolding

## Decision 1: Keep camera authority in native engine loop

- Decision: The native engine remains the single authority for follow-camera framing and player-centric scene anchoring.
- Rationale: Movement, terrain/world coordinates, and actor updates already resolve in native tick flow. Keeping camera authority there avoids split-brain state between React and native runtime.
- Alternatives considered:
  - React-managed camera transforms layered over canvas: rejected due to duplicated state and increased drift risk.
  - Hybrid camera state synchronization each frame: rejected due to unnecessary interop complexity for this slice.

## Decision 2: Define centered follow region instead of strict hard-lock center

- Decision: Use a centered follow region contract (player remains inside center band) rather than forced exact pixel-lock center on every frame.
- Rationale: ARPG feel benefits from stable framing while avoiding jitter from micro-adjustments.
- Alternatives considered:
  - Hard center lock every frame: rejected because it can amplify camera jitter during small movement updates.
  - Loose edge-follow only: rejected because user feedback requires clear center anchoring.

## Decision 3: Context menu actions are target-aware and actor-scoped

- Decision: Context menu options are derived from current target actor context, and action outcomes are applied only to the selected actor.
- Rationale: Prevents interaction ambiguity once multiple actors exist in the scene.
- Alternatives considered:
  - Global menu actions with no target binding: rejected because it cannot guarantee actor-specific outcomes.
  - Implicit nearest-actor dispatch with no target lock: rejected due to accidental action routing in dense scenes.

## Decision 4: Introduce baseline non-player actors with interaction-ready state

- Decision: Add at least one non-player actor type with clear interaction eligibility and state transition behavior.
- Rationale: Required to complete the proof-of-concept gameplay loop beyond player-only movement.
- Alternatives considered:
  - Decorative actors only: rejected because this does not satisfy actionable interaction criteria.
  - Full AI behavior expansion in this slice: rejected as out-of-scope complexity.

## Decision 5: Keep build/runtime contract explicit for WASM exports

- Decision: Maintain explicit exported runtime symbols used by React bridge in the wasm build script and validate this in local/runtime checks.
- Rationale: Missing exports can produce silent runtime regressions that mask camera/interaction behavior.
- Alternatives considered:
  - Rely on implicit exports: rejected due to non-deterministic runtime compatibility.
  - Dynamic symbol probing at runtime: rejected for avoidable complexity in this phase.

## Outcome

All planning-phase clarifications were resolved with no remaining NEEDS CLARIFICATION markers.
