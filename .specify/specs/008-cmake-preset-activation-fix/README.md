# 008 CMake Preset Activation Fix — Execution Tracker

**Date**: 2026-04-26
**Feature**: 008-cmake-preset-activation-fix
**Status**: In Progress

## Problem Statement

CMake Tools in the Banana multi-root workspace intermittently fails with "No configure preset is active for this CMake project" despite a valid `CMakePresets.json` with a `default` preset. CLI preset path (`cmake --preset default`) works correctly.

## Root Cause Assessment

The multi-root workspace has 12 folders. CMake Tools activates per focused folder. When folders other than `(Monorepo) Banana` or `(Monorepo) Tests` are focused, CMake Tools may enter an indeterminate activation state that contaminates the session for CMake-owning folders.

## Fix Strategy

1. Ensure root and tests folder settings are fully aligned with canonical preset contract.
2. Add `cmake.configureOnOpen: false` defensively to prevent auto-configure confusion on folder focus changes.
3. Add active-selection preset keys (`cmake.configurePreset`, not only `cmake.defaultConfigurePreset`) at workspace level.
4. Capture CLI-based evidence for targets, tests, and native lane as the reliable validation path.
5. Document diagnostic behavior for known failure states.

## Analysis Artifacts

See `analysis/` directory for all evidence and contract artifacts.
