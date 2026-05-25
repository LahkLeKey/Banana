# Implementation Plan: Client World Assets, Continents Playtests, and Modular Demo Scenes

**Branch**: 011-client-world-assets-demo-scenes | **Date**: 2026-05-25 | **Spec**: .specify/specs/011-client-world-assets-demo-scenes/spec.md

**Input**: Feature specification from .specify/specs/011-client-world-assets-demo-scenes/spec.md

## Summary

Implement a client-facing world-content layer on top of deterministic terrain/route contracts by introducing continent asset packs, Banana Line station/corridor demo slices, and a modular scene catalog that supports iterative playtesting without monolithic startup coupling.

## Technical Context

**Language/Version**: C11 native runtime (DX12 POC scene shell), TypeScript React/shared UI for scene metadata surfaces when applicable

**Primary Dependencies**:
- Existing deterministic static-mesh and Banana Line route contracts in runtime/terrain/static_mesh
- DX12 POC scene browser and scene-flow pipeline under src/native/engine/win32_dx12_poc
- Canonical worldbuilding baseline in docs/banana-archipelago-worldbuilding.md

**Storage**: Client asset pack metadata and runtime scene catalog contracts (no new authoritative DB mesh persistence)

**Testing**:
- Native C test targets for scene bootstrap determinism and route-anchor contract checks
- Focused runtime playtest launches for continent and Banana Line demo scenes

**Target Platform**: Windows native DX12 playable prototype shell

**Project Type**: Native runtime world-content scaffolding with modular scene orchestration

**Performance Goals**:
- Deterministic scene bootstrap signatures for repeated launches
- Fast scene-slice startup for iterative playtesting

**Constraints**:
- Preserve launch-gate contract behavior by mode
- Keep server authoritative storage boundaries unchanged from Specs 009/010

**Scale/Scope**:
- Initial scaffold targets canonical continent/route slices needed for immediate playtesting and evidence capture

## Constitution Check

- [x] Player trust and disclosure alignment verified for placeholder vs content-ready scenes
- [x] Cross-domain contracts mapped for terrain routes, scene bootstrap, and asset pack metadata
- [x] Quality gates defined for deterministic scene launch and route-anchor integrity
- [x] Reproducible evidence path identified for continent and Banana Line demo-slice launches

## Project Structure

### Documentation (this feature)

.specify/specs/011-client-world-assets-demo-scenes/
- spec.md
- plan.md
- tasks.md
- quickstart.md

### Source Code (expected touchpoints)

src/native/engine/win32_dx12_poc/
- app/main.c
- scene/scene_flow.c
- scene_flow.h
- overlay/scene_overlay.c

src/native/engine/runtime/terrain/static_mesh/
- terrain_static_mesh_domain.c
- terrain_static_mesh_domain.h

tests/native/runtime/
- engine/*scene* and *launch* tests
- terrain/*route* parity/binding tests where route anchors are consumed

artifacts/native/011-client-world-assets-demo-scenes/
- playtest evidence and scene-catalog validation outputs

## Structure Decision

Use a scene-catalog-first architecture in DX12 POC so each worldbuilding slice (continent, station, corridor, lab scene) is an independent launchable module with deterministic bootstrap signatures and explicit diagnostics.
