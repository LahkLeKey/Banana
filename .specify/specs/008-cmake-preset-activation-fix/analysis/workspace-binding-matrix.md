# Workspace Binding Matrix: CMake Preset Activation Fix

**Date**: 2026-04-26  
**Purpose**: Define correct source directory bindings for each CMake-owning workspace folder.

## Binding Contract

| Workspace Folder | `workspaceFolder` value | `cmake.sourceDirectory` | CMake project root resolves to |
|---|---|---|---|
| `(Monorepo) Banana` | `/c/Github/Banana` | Not set (implicit: `workspaceFolder`) | `/c/Github/Banana` ✅ |
| `(Monorepo) Tests` | `/c/Github/Banana/tests` | `${workspaceFolder}/..` | `/c/Github/Banana` ✅ |

## Preset Resolution Path

Both CMake-owning folders resolve to the same project root (`/c/Github/Banana`), which contains:
- `CMakeLists.txt` (root project)
- `CMakePresets.json` (preset definitions)

This means both folders share the same configure/build/test preset catalog and the same build output directory (`build/cmake-tools`).

## Settings Alignment State (Pre-Fix Assessment)

### Root `.vscode/settings.json` (Monorepo Banana folder)

| Key | Current Value | Required? | Status |
|---|---|---|---|
| `cmake.useCMakePresets` | `"always"` | Yes | ✅ Correct |
| `cmake.configurePreset` | `"default"` | Yes | ✅ Correct |
| `cmake.buildPreset` | `"default"` | Yes | ✅ Correct |
| `cmake.testPreset` | `"default"` | Yes | ✅ Correct |
| `cmake.defaultConfigurePreset` | `"default"` | Recommended | ✅ Correct |
| `cmake.defaultBuildPreset` | `"default"` | Recommended | ✅ Correct |
| `cmake.defaultTestPreset` | `"default"` | Recommended | ✅ Correct |
| `cmake.configureSettings.CMAKE_C_COMPILER` | Explicit clang path | Defensive | ✅ Present |
| `cmake.configureOnOpen` | Not set | Defensive | ⚠️ Missing — add `false` to prevent auto-configure on folder focus |

### `tests/.vscode/settings.json` (Monorepo Tests folder)

| Key | Current Value | Required? | Status |
|---|---|---|---|
| `cmake.sourceDirectory` | `${workspaceFolder}/..` | Yes | ✅ Correct |
| `cmake.useCMakePresets` | `"always"` | Yes | ✅ Correct |
| `cmake.configurePreset` | `"default"` | Yes | ✅ Correct |
| `cmake.buildPreset` | `"default"` | Yes | ✅ Correct |
| `cmake.testPreset` | `"default"` | Yes | ✅ Correct |
| `cmake.defaultConfigurePreset` | `"default"` | Recommended | ✅ Correct |
| `cmake.defaultBuildPreset` | `"default"` | Recommended | ✅ Correct |
| `cmake.defaultTestPreset` | `"default"` | Recommended | ✅ Correct |
| `cmake.configureOnOpen` | Not set | Defensive | ⚠️ Missing — add `false` |

### `Banana.code-workspace` (workspace-level defaults)

| Key | Current Value | Required? | Status |
|---|---|---|---|
| `cmake.useCMakePresets` | `"always"` | Yes | ✅ Correct |
| `cmake.defaultConfigurePreset` | `"default"` | Recommended | ✅ Correct |
| `cmake.defaultBuildPreset` | `"default"` | Recommended | ✅ Correct |
| `cmake.defaultTestPreset` | `"default"` | Recommended | ✅ Correct |
| `cmake.configurePreset` | Not set | Active selection | ⚠️ Missing — add `"default"` |
| `cmake.buildPreset` | Not set | Active selection | ⚠️ Missing — add `"default"` |
| `cmake.testPreset` | Not set | Active selection | ⚠️ Missing — add `"default"` |
| `cmake.configureOnOpen` | Not set | Defensive | ⚠️ Missing — add `false` |

## Required Changes Summary

1. Root `.vscode/settings.json`: add `cmake.configureOnOpen: false`
2. `tests/.vscode/settings.json`: add `cmake.configureOnOpen: false`
3. `Banana.code-workspace`: add `cmake.configurePreset`, `cmake.buildPreset`, `cmake.testPreset`, `cmake.configureOnOpen: false`
