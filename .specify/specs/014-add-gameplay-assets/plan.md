# Implementation Plan: Gameplay Asset Model Expansion

**Branch**: `014-add-gameplay-assets` | **Date**: 2026-05-26 | **Spec**: `.specify/specs/014-add-gameplay-assets/spec.md`

**Input**: Feature specification from `.specify/specs/014-add-gameplay-assets/spec.md`

## Summary

Expand the native runtime’s placeholder world presentation into a small but extensible gameplay asset layer by adding a catalog of non-terrain models, region-themed asset groupings, deterministic scene placements, and focused validation scenes that prove the runtime has moved beyond the current banana-plus-bumpy-terrain baseline.

## Technical Context

**Language/Version**: C11 native runtime and Win32 DX12 POC shell

**Primary Dependencies**:
- Existing DX12 POC scene bootstrap and overlay flow under `src/native/engine/win32_dx12_poc`
- Runtime world/scene contracts under `src/native/engine/runtime`
- Canonical regional theming baseline in `docs/banana-archipelago-worldbuilding.md`
- Prior world-assets/demo-scenes slice in `.specify/specs/011-client-world-assets-demo-scenes`

**Storage**: File-backed/native in-memory asset catalog metadata and scene placement records; no new authoritative service or database storage

**Testing**:
- Native C tests for deterministic catalog resolution and scene placement behavior
- Focused runtime validation via DX12 POC scene launches
- Failure-path checks for missing or disabled asset entries

**Target Platform**: Windows 10+ native DX12 prototype runtime

**Project Type**: Native runtime gameplay-content scaffolding

**Performance Goals**:
- Deterministic repeated scene launches for targeted validation slices
- No regression to scene startup usability for focused playtest loops

**Constraints**:
- Preserve current native/UI ABI boundaries and avoid shifting gameplay logic into shell UI layers
- Keep API, persistence, and storefront scope unchanged for this slice
- Add models through cataloged registration rather than scene-by-scene hardcoding

**Scale/Scope**:
- Initial slice targets one extensible gameplay asset catalog, at least three themed model groups, and a small validation set of representative scenes or variants

## Constitution Check

- [x] Player trust and disclosure alignment verified: feature changes runtime visuals only; no new storefront, AI disclosure, controller, or system requirement claims are introduced.
- [x] Storefront governance artifacts identified when public Steam copy is affected: not applicable for this slice because public Steam copy remains unchanged.
- [x] Cross-domain contracts mapped for touched layers: native runtime asset cataloging, scene bootstrap, and worldbuilding/theme metadata are in scope; API and orchestration remain unchanged unless validation automation is added later.
- [x] Quality gates defined with measurable checks for deterministic behavior, integration paths, and failure handling.
- [x] Reproducible delivery path identified for target runtime channels and evidence artifacts listed for release validation via focused DX12 POC scene launches and captured runtime evidence.

## Project Structure

### Documentation (this feature)

```text
.specify/specs/014-add-gameplay-assets/
├── spec.md
├── plan.md
├── checklists/
│   └── requirements.md
└── tasks.md
```

### Source Code (expected touchpoints)

```text
src/native/engine/win32_dx12_poc/
├── app/
├── scene/
└── overlay/

src/native/engine/runtime/
├── asset/
├── scene/
└── terrain/

tests/native/
├── engine/
└── runtime/

artifacts/native/014-add-gameplay-assets/
└── runtime validation evidence
```

**Structure Decision**: Keep the feature entirely in the native runtime/DX12 playtest surface. Asset catalog definitions and scene placements should live beside existing runtime scene/bootstrap code, while evidence stays in a feature-scoped artifact folder. No new frontend or API structure is needed.

## Implementation Phases

### Phase 0: Asset Baseline Inventory

- Identify the current banana mesh, terrain placeholder path, and the narrowest existing asset registration/bootstrap surfaces that control scene composition.
- Inventory which demo scenes from the prior slice are the cheapest validation surfaces for richer gameplay models.
- Define the first themed asset groups to reflect worldbuilding canon: tropical/coastal, urban/industrial, and rugged/wild.

### Phase 1: Catalog and Placement Design

- Introduce or extend a gameplay asset catalog contract keyed by stable model identifiers.
- Define scene placement records that bind catalog keys to deterministic transforms, optional variants, and diagnostics tags.
- Specify fallback behavior for missing, disabled, or invalid asset entries without collapsing the entire scene.

### Phase 2: Runtime Integration

- Wire themed gameplay asset entries into the scene bootstrap path.
- Update targeted scenes or scene variants so each validation slice loads multiple non-terrain models.
- Preserve the banana mesh as a reference/fallback asset instead of removing it entirely.

### Phase 3: Validation and Evidence

- Add or update native tests for deterministic placement and asset resolution.
- Run focused scene launches that prove three representative themed slices render richer compositions.
- Capture evidence under `artifacts/native/014-add-gameplay-assets/` showing both success-path scenes and failure-path diagnostics.

## Validation Strategy

- Deterministic test: repeated launches of the same targeted scene resolve the same catalog keys and placements.
- Theme coverage test: targeted scenes resolve different model groups for tropical/coastal, urban/industrial, and rugged/wild variants.
- Failure-path test: a scene referencing a missing or disabled catalog entry emits explicit diagnostics and preserves controlled fallback behavior.
- Runtime evidence: DX12 POC launch captures or logs prove scenes now render more than the single banana and terrain baseline.

## Delivery Evidence

- `artifacts/native/014-add-gameplay-assets/scene-launch-evidence.md`
- `artifacts/native/014-add-gameplay-assets/catalog-diagnostics.md`
- Focused native test output for asset-catalog determinism and fallback diagnostics

## Complexity Tracking

No constitution exceptions are expected for this feature.
