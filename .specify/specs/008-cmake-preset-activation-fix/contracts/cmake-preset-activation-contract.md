# CMake Preset Activation Contract

**Feature**: 008-cmake-preset-activation-fix  
**Date**: 2026-04-26

## Scope

Defines expected behavior for CMake preset activation and native discovery in Banana's multi-root workspace.

## Activation Contract

1. Configure/build/test must reference resolvable preset names from root `CMakePresets.json`.
2. Workspace contexts used by developers (root and tests folder) must resolve to the same project-level CMake source.
3. Configure must complete without manual preset rescue in normal workflow.

## Discovery Contract

1. Successful configure must expose native build targets.
2. Successful configure must expose native tests including DAL contract lane.
3. Native lane execution remains valid through existing test registration.

## Diagnostics Contract

1. Failure must be classified as one of: missing preset, invalid preset mapping, context mismatch.
2. Failure output must provide actionable remediation direction.
3. Silent/empty diagnostics are considered contract failure.

## Validation Contract

Required minimum evidence for closure:

- CMake Tools configure/build invocation pass in workspace context
- CMake target and test discovery pass
- Native lane execution signal pass
