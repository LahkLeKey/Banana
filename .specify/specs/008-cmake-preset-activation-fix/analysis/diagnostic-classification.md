# Diagnostic Classification: CMake Preset Activation Fix

**Date**: 2026-04-26  
**Purpose**: Define expected diagnostic output for known CMake preset activation failure states.

## Classification Table

| Failure State | Expected CMake Tools Message | Expected CLI Behavior | Classification |
|---|---|---|---|
| Missing preset name in `CMakePresets.json` | "CMake preset 'X' is not in the list of configure presets" | `cmake --preset X` → error: "Cannot find configure preset 'X'" | `MISSING_PRESET` |
| `cmake.useCMakePresets: "never"` or absent | "No configure preset is active" (may fall back to kit mode) | CLI unaffected | `PRESETS_DISABLED` |
| Active-selection key absent (`cmake.configurePreset` missing) | "No configure preset is active" | CLI unaffected | `ACTIVE_SELECTION_MISSING` |
| Wrong `cmake.sourceDirectory` for tests folder | Configure fails: no `CMakeLists.txt` found | CLI from correct dir succeeds | `SOURCE_DIR_MISMATCH` |
| Stale extension session state | Prior failure replays; preset may appear set but configure fails | CLI succeeds | `STALE_EXTENSION_STATE` |
| Multi-root focus on non-CMake folder | No CMake project detected for that folder; status bar shows nothing | N/A | `NON_CMAKE_FOLDER_FOCUS` |

## Actionable Diagnostic Outputs

For each classification, the actionable guidance:

### `MISSING_PRESET`
- Verify `CMakePresets.json` has a `configurePreset` with the exact name used in settings.
- Check `cmake.useCMakePresets` is `"always"`.

### `PRESETS_DISABLED`
- Set `cmake.useCMakePresets: "always"` in folder or workspace settings.

### `ACTIVE_SELECTION_MISSING`
- Add `cmake.configurePreset: "default"` (and build/test equivalents) to the affected folder's `.vscode/settings.json`.

### `SOURCE_DIR_MISMATCH`
- For `tests/` folder: set `cmake.sourceDirectory: "${workspaceFolder}/.."`.
- Verify the resolved path contains both `CMakeLists.txt` and `CMakePresets.json`.

### `STALE_EXTENSION_STATE`
- Perform VS Code reload: `Ctrl+Shift+P` → `Developer: Reload Window`.
- If persists: `Ctrl+Shift+P` → `CMake: Reset CMake Tools Extension State`, then reload.

### `NON_CMAKE_FOLDER_FOCUS`
- Switch active folder to `(Monorepo) Banana` or `(Monorepo) Tests` before invoking CMake commands.
- CMake Tools only activates for folders with a discovered CMake project.
