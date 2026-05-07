# Implementation Plan: 261 Wasm Viewport React Overlays

**Branch**: `261-wasm-viewport-react-overlays` | **Date**: 2026-05-07 | **Spec**: `.specify/specs/261-wasm-viewport-react-overlays/spec.md`
**Input**: Feature specification from `.specify/specs/261-wasm-viewport-react-overlays/spec.md`

## Summary

Migrate Banana into a game engine runtime where the WASM viewport is the primary full-screen background scene and React is used for layered UI overlays, HUD, menus, diagnostics, and modal interactions without owning frame rendering.

## Technical Context

**Language/Version**: TypeScript, React, Bun, Vite, Tailwind, WASM runtime bridge
**Primary Dependencies**: Bun, React, @banana/ui, shared WASM loader and worker contracts
**Storage**: Runtime state in-memory; telemetry/evidence artifacts under `artifacts/`
**Testing**: Biome lint/format, TypeScript checks, runtime smoke, contract tests for lifecycle transitions
**Target Platform**: React web first, then Electron/mobile parity follow-ups
**Project Type**: Frontend + runtime integration
**Performance Goals**: <= 3.0 s first frame in baseline profile; deterministic overlay/input handoff <= 100 ms
**Constraints**: Preserve existing controller/service/pipeline/native contracts and existing API endpoints
**Scale/Scope**: Viewport/overlay migration slice only (no multiplayer/world-streaming expansion)

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

- Existing safety contracts are preserved across runtime, API, and native layers.
- No production deployment bypasses are introduced.
- All behavior changes include auditable evidence in tasks and CI run artifacts.
- React remains Bun-managed and respects `VITE_BANANA_API_BASE_URL` runtime contract.

## Project Structure

- `src/typescript/react/src/` - viewport host shell, overlay layer manager, input-routing hooks
- `src/typescript/react/src/workers/` - WASM worker bootstrap and message protocol adapters
- `src/typescript/shared/ui/src/` - reusable overlay primitives and layout tokens
- `src/typescript/electron/` - channel compatibility follow-up (readiness checks only in this slice)
- `.specify/specs/261-wasm-viewport-react-overlays/` - spec, plan, tasks, evidence

## Phases

### Phase 1: Setup

- Define viewport lifecycle state model and overlay layering contract.
- Define input-routing and focus policy between scene controls and overlay controls.
- Define telemetry schema for healthy path and degraded recovery path.

### Phase 2: Implementation

- Implement viewport-first app shell layout with WASM surface as background.
- Implement React overlay manager with deterministic z-order and lock behavior.
- Implement lifecycle transitions and degraded-mode fallback UX.
- Wire telemetry for startup, handoff, and recovery.

### Phase 3: Validation

- Add runtime contract tests for lifecycle and input-routing transitions.
- Execute healthy-path smoke validation and degraded-path recovery validation.
- Capture CI/UAT evidence and close feature tasks with run IDs and artifact paths.
