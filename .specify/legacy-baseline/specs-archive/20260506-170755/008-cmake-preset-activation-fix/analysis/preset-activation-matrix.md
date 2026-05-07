# Preset Activation Matrix: CMake Preset Activation Fix

**Date**: 2026-04-26
**Purpose**: Define the expected activation state for each workspace folder context.

## Activation Matrix

| Workspace Folder | Path | CMake Project? | Expected CMake Tools Behavior | Preset Source |
|---|---|---|---|---|
| `(Monorepo) Banana` | `.` | ✅ Yes | Configure using `default` preset from `CMakePresets.json` | Root `CMakePresets.json` |
| `(Monorepo) Tests` | `tests` | ✅ Yes (via sourceDirectory bind) | Configure using `default` preset, source directory re-bound to repo root | Root `CMakePresets.json` via `cmake.sourceDirectory: "${workspaceFolder}/.."` |
| `(C) Native` | `src/native` | ❌ No | CMake Tools should not find a project; no activation attempt | N/A |
| `(C#) Api` | `src/c-sharp/asp.net` | ❌ No | CMake Tools should not find a project; no activation attempt | N/A |
| All TS / Docker / Scripts / Specs folders | Various | ❌ No | CMake Tools should not find a project; no activation attempt | N/A |

## Preset Catalog Contract

| Preset Type | Name | Configure Preset | Expected Behavior |
|---|---|---|---|
| Configure | `default` | N/A | Ninja generator, clang compiler, Debug build, `build/cmake-tools` output dir |
| Build | `default` | `default` | Builds all targets using `default` configure |
| Test | `default` | `default` | Runs all ctest tests registered under `default` configure |

## Active Selection Settings Contract

For CMake Tools to activate a preset without user intervention, all three of the following settings must be present and consistent per folder:

| Setting Key | Required | Value |
|---|---|---|
| `cmake.useCMakePresets` | Yes | `"always"` |
| `cmake.configurePreset` | Yes (active selection) | `"default"` |
| `cmake.buildPreset` | Yes (active selection) | `"default"` |
| `cmake.testPreset` | Yes (active selection) | `"default"` |
| `cmake.defaultConfigurePreset` | Recommended (initial value) | `"default"` |
| `cmake.defaultBuildPreset` | Recommended (initial value) | `"default"` |
| `cmake.defaultTestPreset` | Recommended (initial value) | `"default"` |

**Note**: `cmake.configurePreset` (active) and `cmake.defaultConfigurePreset` (initial/fallback) are different keys. Both must be set for deterministic activation.

## Known Failure Modes

| Failure | Symptom | Root Cause |
|---|---|---|
| Missing configure preset | "No configure preset is active" | `cmake.configurePreset` key absent or preset name not found in `CMakePresets.json` |
| Workspace-level only (no folder-level) | Auto-configure fails for folders not covered by workspace default | Workspace settings don't override per-folder state; folder-level keys needed |
| Stale extension state | Preset appears active but configure fails | Extension caches prior state; requires VS Code reload |
| Wrong source directory | Preset file not found for tests folder | `cmake.sourceDirectory` absent or pointing to wrong path |
