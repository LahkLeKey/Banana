# Feature Specification: Gameplay Asset Model Expansion

**Feature Branch**: `014-add-gameplay-assets`

**Created**: 2026-05-26

**Status**: Draft

**Input**: User description: "more gameplay asset models we only have a basic banana and bumpy green terrain"

**Canonical Baseline**:
- `.specify/specs/011-client-world-assets-demo-scenes/spec.md` defines the prior continent/demo-scene asset slice that this work extends.
- `docs/banana-archipelago-worldbuilding.md` defines canonical regional identity and biome cues for asset theming.
- Current client visual baseline is still dominated by the basic banana model and a bumpy green terrain surface.

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Richer Traversal Landmarks (Priority: P1)

As a player, I can move through scenes that include recognizable gameplay landmarks and props beyond the single banana mesh so traversal reads as a playable world rather than a placeholder prototype.

**Why this priority**: The current asset set undercuts every other worldbuilding slice; adding a small but coherent landmark set immediately improves readability and playtest value.

**Independent Test**: Launch a focused demo scene and verify that at least one landmark cluster, one traversal prop set, and the existing banana reference asset all render together without falling back to placeholder-only composition.

**Acceptance Scenarios**:

1. **Given** a demo scene configured for gameplay asset expansion, **When** the scene loads, **Then** the runtime instantiates multiple non-terrain gameplay models in deterministic locations.
2. **Given** a scene that includes both old and new assets, **When** a playtester traverses the route, **Then** landmarks are visually distinct enough to identify route anchors and points of interest.

---

### User Story 2 - Region-Themed Asset Variants (Priority: P1)

As a playtester, I can see different model sets for tropical, urban, and rugged areas so the Banana Archipelago regions no longer feel like the same terrain with one shared prop.

**Why this priority**: Region identity is already defined in worldbuilding canon; gameplay models need to reinforce those distinctions for continent and Banana Line slices to be credible.

**Independent Test**: Launch three representative scenes or scene variants and verify each one resolves a different asset subset aligned to its region theme.

**Acceptance Scenarios**:

1. **Given** a Stem Territories or Sun Coast slice, **When** scene assets resolve, **Then** the runtime uses tropical/coastal gameplay props rather than only generic placeholder assets.
2. **Given** a Metro Banana or Iron Spine slice, **When** scene assets resolve, **Then** the runtime uses an alternate model subset that differentiates urban or rugged play spaces from tropical zones.

---

### User Story 3 - Asset Catalog Growth Without Scene Rewrites (Priority: P2)

As a developer, I can add new gameplay models through a cataloged asset registration path so future world-content slices do not require hand-editing every scene definition.

**Why this priority**: The immediate goal is more models, but the durable value is avoiding another hardcoded placeholder bottleneck when more props arrive.

**Independent Test**: Register one new gameplay model entry, reference it from a demo scene configuration, and launch that scene without changing unrelated scene bootstrap logic.

**Acceptance Scenarios**:

1. **Given** a registered gameplay asset model, **When** a scene references its catalog key, **Then** the model loads through the existing asset resolution path.
2. **Given** a catalog entry that is disabled or missing source data, **When** the scene loads, **Then** the runtime emits explicit diagnostics and preserves controlled fallback behavior.

### Edge Cases

- A scene references a gameplay model key whose mesh or material payload is missing from the asset bundle.
- A region theme requests more models than the current scene budget can display without overwhelming readability.
- A newly added gameplay asset uses a transform or scale that collides with traversal lanes or spawn anchors.
- Asset registration succeeds for one scene pack but silently falls back to the single banana placeholder in another pack.

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: System MUST support a catalog of gameplay asset models beyond the existing banana mesh and terrain surface.
- **FR-002**: System MUST allow demo scenes to reference gameplay models by stable catalog key instead of embedding one-off mesh wiring per scene.
- **FR-003**: System MUST provide at least three model-group themes that can distinguish representative tropical/coastal, urban/industrial, and rugged/wild play spaces.
- **FR-004**: System MUST allow a scene to place multiple gameplay asset models with deterministic transforms so repeated launches produce the same composition.
- **FR-005**: System MUST preserve the existing banana model as a valid reference asset for comparison, mixed scenes, or fallback behavior.
- **FR-006**: System MUST emit explicit diagnostics when a scene references a missing, disabled, or invalid gameplay asset model entry.
- **FR-007**: System MUST define a focused validation path that proves scenes render more than the current banana-plus-terrain baseline.
- **FR-008**: System MUST keep gameplay asset additions compatible with the worldbuilding canon already defined for the Banana Archipelago regions.

### Key Entities *(include if feature involves data)*

- **Gameplay Asset Model**: A renderable gameplay prop or landmark entry with a stable key, mesh/material source, theme tags, and fallback behavior.
- **Asset Theme Set**: A grouped subset of gameplay asset models used to express a region or biome identity such as tropical, urban, or rugged.
- **Scene Asset Placement**: A deterministic scene record that binds a gameplay asset model key to a transform, optional variant, and diagnostics tag.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: At least three focused gameplay scenes or scene variants launch with two or more non-terrain gameplay models visible in each scene.
- **SC-002**: 100% of targeted validation scenes resolve the intended theme set without reverting to banana-only composition except in explicit fallback tests.
- **SC-003**: Repeated launches of the same validation scene produce matching gameplay asset placements for all cataloged models used by that scene.
- **SC-004**: Adding one new gameplay asset model to the catalog requires no changes to unrelated scene bootstrap code outside the targeted registration path and scene reference.

## Assumptions

- This slice focuses on client-side/native runtime asset presentation and does not introduce new API or persistence requirements.
- Initial gameplay models may be simple low-poly or blockout-quality assets as long as they are distinct and support region identity.
- Existing demo-scene infrastructure from Spec 011 remains the preferred validation surface for this feature.
- Public Steam storefront copy is not changed by this internal gameplay-asset expansion slice.

## Constitution Alignment *(mandatory)*

- **Disclosure Integrity**: This feature improves in-game visual variety but does not change public storefront promises; any screenshots captured as evidence must reflect the actual in-runtime asset set.
- **Cross-Domain Contracts**: Primary contracts are native/runtime scene bootstrap, client asset catalog registration, and worldbuilding/theme metadata. API and orchestration contracts remain unchanged unless validation automation is added later.
- **Quality Gates**: Validation must cover successful scene launches with richer asset sets, deterministic repeated launches, and explicit missing-asset diagnostics.
- **Delivery Evidence**: Evidence should include scene-launch captures or logs for representative themed slices, plus a catalog/registration proof showing the new gameplay asset entries wired into the runtime.

## Storefront & Release Claims *(required when public Steam copy is affected)*

- **Storefront Copy Contract**: No storefront copy changes are in scope for this feature.
- **AI Disclosure**: No AI disclosure changes are required for this feature.
- **Controller Support**: Controller support claims are unchanged by this feature.
- **System Requirements**: System requirement claims are unchanged by this feature.
- **Store Assets**: Store page asset inventory updates are out of scope unless release marketing later adopts screenshots from this slice.
