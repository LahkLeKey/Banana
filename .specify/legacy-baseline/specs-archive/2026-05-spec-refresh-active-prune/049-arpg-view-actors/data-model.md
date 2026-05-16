# Data Model: ARPG View and Actor Scaffolding

## Entity: ARPGCameraState

- Purpose: Represents runtime follow-camera framing behavior for player-centric scene rendering.
- Core Attributes:
  - followAnchorX
  - followAnchorY
  - followAnchorZ
  - centerRegionWidth
  - centerRegionHeight
  - viewportWidth
  - viewportHeight
  - isStable
- Relationships:
  - Reads player world position from PlayerActorState.
  - Uses viewport dimensions from RuntimeViewportState.
- Validation Rules:
  - viewportWidth > 0 and viewportHeight > 0
  - center region dimensions must be positive and bounded by viewport dimensions
- State Transitions:
  - initializing -> tracking
  - tracking -> recentering (when player exits center region)
  - recentering -> tracking

## Entity: PlayerActorState

- Purpose: Represents player banana runtime movement and interaction origin.
- Core Attributes:
  - actorId
  - worldX
  - worldY
  - worldZ
  - moveInputX
  - moveInputZ
  - interactionRange
  - isActive
- Relationships:
  - Influences ARPGCameraState follow anchor.
  - Initiates interactions with WorldActorState.
- Validation Rules:
  - actorId is unique within active scene
  - move input values remain within normalized range

## Entity: WorldActorState

- Purpose: Represents non-player actors available for selection and interaction.
- Core Attributes:
  - actorId
  - actorType
  - worldX
  - worldY
  - worldZ
  - interactionEligibility
  - interactionState
  - isVisible
  - isActive
- Relationships:
  - Selected by ContextInteractionMenu target resolution.
  - Receives InteractionOutcome updates.
- Validation Rules:
  - actorId is unique
  - actorType must map to a known actor behavior contract
  - ineligible actors cannot receive actionable outcomes
- State Transitions:
  - idle -> targeted
  - targeted -> interacted
  - interacted -> idle or cooldown

## Entity: ContextInteractionMenu

- Purpose: Represents right-click context interactions tied to scene target context.
- Core Attributes:
  - isVisible
  - anchorX
  - anchorY
  - targetActorId
  - availableActions
  - selectedAction
- Relationships:
  - Reads target from WorldActorState.
  - Emits action intents that produce InteractionOutcome.
- Validation Rules:
  - menu anchor must remain within viewport bounds
  - actionable options require valid target actor
- State Transitions:
  - hidden -> visible
  - visible -> actionSelected
  - actionSelected -> hidden
  - visible -> hidden (outside click or Escape)

## Entity: InteractionOutcome

- Purpose: Represents observable result of a completed actor interaction.
- Core Attributes:
  - outcomeId
  - actorId
  - actionType
  - appliedAt
  - outcomeState
  - feedbackMessage
- Relationships:
  - Generated from ContextInteractionMenu action selection.
  - Applied to one WorldActorState target.
- Validation Rules:
  - actorId must match active target at dispatch time
  - actionType must exist in availableActions for the target actor
- State Transitions:
  - pending -> applied
  - pending -> rejected
