# Feature Specification: Client World Assets, Continents Playtests, and Modular Demo Scenes

**Feature Branch**: 011-client-world-assets-demo-scenes

**Created**: 2026-05-25

**Completed**: 2026-05-25

**Status**: Complete

**Input**: User description: Client-side gameplay assets are still on the old basic world presentation. Add a follow-up spec to playtest new continents and Banana Line content, and add modular demo scenes so slices can be scaffolded and validated independently.

**Canonical Baseline**:
- docs/banana-archipelago-worldbuilding.md defines canonical region/county names and biome adjacency.
- .specify/specs/010-client-static-mesh-banana-line/spec.md defines deterministic terrain and Banana Line route contracts that this feature consumes.

**Completion Evidence**:
- artifacts/native/011-client-world-assets-demo-scenes/setup-evidence.md
- artifacts/native/011-client-world-assets-demo-scenes/end-to-end-evidence.md

## User Scenarios & Testing (mandatory)

### User Story 1 - Client Asset Packs for New Continents (Priority: P1)

As a player, I can load region-specific client gameplay assets (terrain materials, props, and landmark bundles) for the new continents so the runtime no longer displays the old basic placeholder world.

**Why this priority**: Deterministic mesh contracts are already in place; content playtesting requires visible client asset differentiation per continent.

**Independent Test**: Launch a continent scene pack and confirm assigned asset pack IDs resolve to expected props/material variants with no fallback to legacy basic set.

**Acceptance Scenarios**:

1. Given a canonical region ID, when scene bootstrap resolves client assets, then continent-specific pack IDs are loaded instead of legacy defaults.
2. Given a missing or invalid continent asset pack, when scene bootstrap runs, then runtime reports explicit missing-pack diagnostics and uses controlled fallback tags.

---

### User Story 2 - Banana Line Station and Corridor Scene Slices (Priority: P1)

As a playtester, I can launch Banana Line-focused demo slices for station hubs and corridor traversal so route content can be validated independently from full-world sessions.

**Why this priority**: Corridor and station validation needs fast, repeatable scene entry points to iterate route-adjacent content quickly.

**Independent Test**: Launch each station/corridor demo scene and verify route start/end anchors, station props, and traversal transitions render consistently.

**Acceptance Scenarios**:

1. Given a named Banana Line route and stop pair, when the corresponding demo slice launches, then the scene initializes at the expected anchor and station bundle.
2. Given repeated launches of the same slice, when runtime initializes scene content, then resulting scene IDs and asset bindings remain deterministic.

---

### User Story 3 - Modular Demo Scene Catalog for Slice Playtesting (Priority: P2)

As a developer, I can register modular demo scenes by slice (continent, station, corridor, feature lab) so playtesting can scale without entangling all content in one monolithic startup path.

**Why this priority**: Modular scene cataloging prevents spaghetti growth while worldbuilding slices are scaffolded incrementally.

**Independent Test**: Add one new demo scene entry through the catalog contract and launch it without changing unrelated scene code paths.

**Acceptance Scenarios**:

1. Given a scene catalog entry, when the scene browser starts, then metadata (scene key, asset pack, route binding, expected playtest checks) appears in index order.
2. Given a scene entry marked disabled, when scene browser renders, then entry is visible for diagnostics but not launchable.

### Edge Cases

- Asset pack metadata exists but referenced client files are missing from bundle path.
- Banana Line station scene route points to a corridor stop no longer present in canonical route table.
- Demo scene catalog exceeds default index bounds in DX12 POC scene browser.
- Playtester launches in offline mode with missing optional art assets while deterministic gameplay contracts remain available.

## Requirements (mandatory)

### Functional Requirements

- FR-001: System MUST define a client-side continent asset pack registry mapped to canonical region IDs.
- FR-002: System MUST replace the old basic-world fallback as the default render path for continents covered by new asset packs.
- FR-003: System MUST provide modular Banana Line station/corridor demo scenes with deterministic anchor bindings.
- FR-004: System MUST provide a demo-scene catalog contract that supports add/remove/disable without scene-browser rewrites.
- FR-005: System MUST expose scene-level diagnostics for missing asset packs and route/anchor binding mismatches.
- FR-006: System MUST preserve deterministic scene bootstrap signatures for repeated launches of the same catalog entry.
- FR-007: System MUST provide focused playtest commands for continent scenes, Banana Line scenes, and scene-catalog integrity checks.
- FR-008: System MUST capture merge evidence showing at least one continent scene and one Banana Line scene launched with non-legacy assets.

### Key Entities

- Continent Asset Pack: Client bundle contract for materials, props, and landmarks keyed by canonical region ID.
- Demo Scene Catalog Entry: Metadata record describing scene key, launch mode, asset pack references, route bindings, and diagnostics tags.
- Banana Line Station Slice: Launchable scene variant anchored at a named route stop with station-specific content bundle.
- Scene Bootstrap Signature: Deterministic fingerprint for scene initialization inputs (scene key, region/route IDs, asset pack IDs, runtime mode).

## Success Criteria (mandatory)

### Measurable Outcomes

- SC-001: 100% of targeted continent playtest scenes load non-legacy client asset packs in focused validation runs. Targeted scene set: continent-stem-territories (variant 0) and continent-north-crown (variant 1).
- SC-002: 100% deterministic parity for scene bootstrap signatures across repeated launches of the same demo scene entry.
- SC-003: 100% of Banana Line demo slices validate expected route-anchor bindings with no unresolved stops.
- SC-004: Scene-catalog additions for new slices require only catalog metadata changes plus scene implementation file, with no regressions in existing scene launches.

## Assumptions

- Deterministic terrain/route contracts from Spec 010 are the source of truth for continent and Banana Line scene bindings.
- Art production may ship incrementally; controlled diagnostic fallbacks are allowed during scaffold phases.
- DX12 POC scene browser remains the primary local playtest shell for early modular scene validation.
- Server persistence boundaries from Specs 009 and 010 remain unchanged for this feature.

## Constitution Alignment (mandatory)

- Disclosure Integrity: Playtest and release messaging distinguishes placeholder fallback behavior from continent-ready asset-pack behavior.
- Cross-Domain Contracts: Native scene bootstrap, client asset binding contracts, and Banana Line route metadata all participate in this feature.
- Quality Gates: Deterministic scene bootstrap tests, route-anchor binding checks, and launch diagnostics are required gates.
- Delivery Evidence: Merge evidence includes successful launch captures for new continent content and Banana Line station/corridor slices.
