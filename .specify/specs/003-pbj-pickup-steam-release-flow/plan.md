# Implementation Plan: PBJ Pickup Celebration and Steam Release Tracking

**Branch**: `003-pbj-pickup-steam-release-flow` | **Date**: 2026-05-22 | **Spec**: `.specify/specs/003-pbj-pickup-steam-release-flow/spec.md`

**Input**: Feature specification from `.specify/specs/003-pbj-pickup-steam-release-flow/spec.md`

## Summary

Deliver a player-facing PBJ pickup interaction with dancing banana screen celebration in the React gameplay viewport, then provide explicit execution tasks for Steam publish and PR/release tracking so local and CI lanes remain consistent and auditable.

## Technical Context

**Language/Version**: TypeScript + React for gameplay UI, markdown for release/spec artifacts

**Primary Dependencies**: Existing game-session snapshot stream, GameEnginePage runtime loop, Steam publish helper scripts, workflow-dispatch publish lane

**Storage**: Session-scoped UI state for pickup/particles; markdown evidence records for release tracking

**Testing**: TypeScript compile checks, gameplay viewport smoke checks, release workflow dry-run/evidence validation

**Target Platform**: Windows developer machines and CI workflow-dispatch lanes

**Project Type**: Gameplay UI enhancement + release process scaffolding

**Performance Goals**: No visible gameplay input lag/regression from celebration effect; bounded particle duration

**Constraints**: Preserve snapshot payload contracts and avoid introducing new backend requirements for this slice

**Scale/Scope**: One pickup interaction + one celebration effect + one actionable release tracking spec package

## Constitution Check

*GATE: Must pass before implementation and before merge recommendation.*

- [x] Runtime behavior is aligned with player-facing disclosure.
- [x] Cross-layer contracts remain unchanged for API/native surfaces.
- [x] Validation path is defined for gameplay + release artifacts.
- [x] Evidence expectations for publish/launch/blockers are explicit.

## Project Structure

### Documentation (this feature)

```text
.specify/specs/003-pbj-pickup-steam-release-flow/
├── spec.md
├── plan.md
├── tasks.md
└── checklists/
    └── requirements.md
```

### Source Code (repository root)

```text
src/typescript/react/src/pages/
└── GameEnginePage.tsx

scripts/
└── steam-*.sh / steam-*.ps1 (execution surfaces referenced, no mandatory change in this slice)
```

**Structure Decision**: Keep implementation scoped to React runtime page for gameplay behavior and `.specify` docs for release tracking orchestration.

## Complexity Tracking

No constitution violations identified for this slice.

## Execution Notes

- PBJ interaction remains client-session scoped by design for this increment.
- If server-authoritative pickup state is needed later, add a follow-up feature spec rather than extending this scope ad hoc.
