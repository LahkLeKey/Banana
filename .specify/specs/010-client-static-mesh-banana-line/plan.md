# Implementation Plan: Client Static Mesh Generators and Banana Line Travel

**Branch**: 010-client-static-mesh-banana-line | **Date**: 2026-05-25 | **Spec**: .specify/specs/010-client-static-mesh-banana-line/spec.md

**Input**: Feature specification from .specify/specs/010-client-static-mesh-banana-line/spec.md

## Summary

Implement deterministic client-side static mesh profile generators for 8 counties plus islands, add Banana Line route scaffolding between region hubs, and enforce a storage boundary where server persistence keeps only procedural contracts, player deltas, and progression ledgers under a 10 GB total DB ceiling.

## Technical Context

**Language/Version**: C11 native runtime, TypeScript API

**Primary Dependencies**: Existing runtime terrain generation contracts, native CMake targets, Bun test harness for API follow-up

**Storage**: PostgreSQL or equivalent authoritative DB with strict 10 GB budget

**Testing**: Native deterministic executable tests via CMake + targeted API integration checks in follow-up phase

**Target Platform**: Windows native runtime and existing Banana runtime channels

**Project Type**: Native runtime module extension + follow-up API persistence policy

**Performance Goals**: Deterministic generation and routing parity in all sampled scenarios; playtest-friendly route generation under chunk-scale budgets

**Constraints**: Total DB budget fixed at 10 GB including deltas and progression ledgers

**Scale/Scope**: 348,200 km2 world concept represented as 8 counties + islands static profile set

## Constitution Check

- [x] Player trust and disclosure alignment verified
- [x] Cross-domain contracts mapped for native, client, and API persistence boundaries
- [x] Quality gates defined for deterministic generation and route parity
- [x] Reproducible runtime evidence path identified via native tests and artifact captures

## Project Structure

### Documentation (this feature)

.specify/specs/010-client-static-mesh-banana-line/
- spec.md
- plan.md
- tasks.md
- quickstart.md

### Source Code (repository root)

src/native/engine/runtime/terrain/
- terrain_static_mesh.h
- terrain_static_mesh.c

tests/native/runtime/terrain/
- runtime_terrain_static_mesh_generation_test.c

## Structure Decision

Use a native runtime terrain extension first, because deterministic generation and route parity are foundational to client shipping strategy and storage-boundary enforcement in API layers.
