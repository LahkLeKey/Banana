# Feature Specification: Gameplay Viewport HUD Compression

**Feature Branch**: `034-collapse-hud-menus`

**Created**: 2026-06-09

**Status**: Draft

**Input**: User description: "Collapse thematic HUD clutter into bottom-center grow-up menus and preserve center space for real gameplay visualizations in notebooks."

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Gameplay-first center viewport (Priority: P1)

As a player, I can see a clear center gameplay stage with minimal obstruction so world visualizations remain the primary focus.

**Why this priority**: The center viewport is the core gameplay surface and directly drives user-perceived wow factor.

**Independent Test**: Open notebooks route with and without a selected sector and verify the center area remains visually open while operational controls stay docked.

**Acceptance Scenarios**:

1. **Given** a user opens notebooks with no sector selected, **When** the page renders, **Then** the center area presents mission staging content without dense card stacks.
2. **Given** a user selects a sector, **When** the gameplay surface updates, **Then** center space remains available for world visualization while detail panels stay docked.

---

### User Story 2 - Bottom-center expandable command dock (Priority: P2)

As a player, I can expand only the operational panel I need from a bottom-center command dock so information remains accessible without persistent clutter.

**Why this priority**: This preserves utility while reducing visual noise and keeps controls predictable.

**Independent Test**: Toggle each dock section and verify open/close behavior is consistent, mutually understandable, and does not cover the full gameplay stage.

**Acceptance Scenarios**:

1. **Given** the command dock is visible, **When** the user activates a dock control, **Then** its panel expands upward from the bottom center.
2. **Given** a dock panel is expanded, **When** the user toggles it off or opens another panel, **Then** layout remains stable and readable on desktop and mobile widths.

---

### User Story 3 - Tactical detail access without default raw dump (Priority: P3)

As a power user, I can still access tactical and raw intelligence data on demand, while casual users are not forced into raw code-first screens.

**Why this priority**: Retains advanced utility without regressing core user experience.

**Independent Test**: Validate tactical metrics appear in docked intel panel by default and raw intel requires explicit unlock/selection.

**Acceptance Scenarios**:

1. **Given** a sector is selected, **When** intel panel opens, **Then** tactical summary is the default view.
2. **Given** developer unlock is disabled, **When** user attempts raw intel mode, **Then** raw mode remains gated until explicitly enabled.

---

### Edge Cases

- No sector selected while dock panel state persists from a previous session.
- Very long quest/objective titles that could overflow compact dock panels.
- Small viewport widths where dock controls and expanded content compete for vertical space.
- Sector changes while a dock is open and mode is set to raw intelligence.

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: System MUST reserve a visually dominant center viewport area for gameplay/world visualization content on notebooks route.
- **FR-002**: System MUST provide a bottom-center command dock with collapsible grow-up panels for operational information.
- **FR-003**: System MUST group operational information into distinct dock sections for intelligence, objectives, and player progression.
- **FR-004**: System MUST ensure expanded dock panels do not fully obscure the center gameplay stage.
- **FR-005**: System MUST default sector intelligence to tactical summary mode when a new sector is selected.
- **FR-006**: System MUST gate raw intelligence behind explicit developer unlock and user action.
- **FR-007**: System MUST preserve readability and control access across desktop and mobile viewport ranges.
- **FR-008**: System MUST preserve mission briefing guidance when no sector is selected while maintaining gameplay-stage layout intent.

### Key Entities *(include if feature involves data)*

- **GameplayViewportState**: Current route visual state including selected sector, active dock section, and center-stage context.
- **DockSectionState**: Open/closed state and selected section for intel, objectives, and player panels.
- **IntelDisplayMode**: Tactical or raw mode state with unlock gating metadata.
- **MissionProgressSnapshot**: Progress counters used by objectives and player dock sections.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: In default notebooks view, users can identify center gameplay stage purpose within 3 seconds in usability checks.
- **SC-002**: At least 80% of persistent information cards from prior layout are moved into bottom dock panels.
- **SC-003**: With any single dock panel expanded, at least half of the gameplay viewport area remains visible on common desktop resolutions.
- **SC-004**: Users can access intelligence, objectives, and player details with no more than two interactions per section.
- **SC-005**: Raw intelligence is never displayed by default for first-time or locked users.

## Assumptions

- Notebooks route remains the primary gameplay shell for this interaction model.
- Existing mission/sector metadata continues to supply dock panel content.
- Center gameplay stage may initially host placeholder visualizations before full 3D scene integration.
- This feature does not alter storefront claims or external release messaging.

## Constitution Alignment *(mandatory)*

- **Disclosure Integrity**: No public storefront or external disclosure copy changes in this scope.
- **Cross-Domain Contracts**: Impacts client-side notebooks presentation contract and interaction expectations for route-level HUD behavior.
- **Quality Gates**: Requires route render verification, dock interaction checks, and tactical/raw gating validation for both selected and unselected sector states.
- **Delivery Evidence**: Provide notebooks route snapshots showing collapsed dock behavior and center-stage preservation across at least desktop and mobile viewport sizes.

## Storefront & Release Claims *(required when public Steam copy is affected)*

- Not applicable in this feature phase.
