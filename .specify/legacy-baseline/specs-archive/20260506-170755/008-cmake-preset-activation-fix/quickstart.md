# Quickstart: CMake Preset Activation Fix

## Purpose

Validate and close the CMake preset activation bug for the native lane in multi-root workspace usage.

## Prerequisites

- Repository root: `C:/Github/Banana`
- Active feature pointer may be set to 008 during execution
- CMake + Ninja toolchain available

## 1. Validate preset catalog

```bash
cd /c/Github/Banana
cmake --list-presets
```

Expected: `default` configure preset is visible.

## 2. Validate configure/build/test via preset CLI (supporting evidence)

```bash
cd /c/Github/Banana
cmake --preset default
cmake --build --preset default
ctest --preset default --output-on-failure
```

Expected: configure succeeds and native tests pass.

## 3. Validate CMake Tools lane

In VS Code with `(Monorepo) Banana` or `(Monorepo) Tests` as the active folder:

1. `Ctrl+Shift+P` → `CMake: Configure` — expected: no "No configure preset is active" error
2. `Ctrl+Shift+P` → `CMake: Build` — expected: build completes for all native targets
3. `Ctrl+Shift+P` → `CMake: Run Tests` — expected: 4/4 native tests pass

If CMake Tools state appears stale, run `CMake: Reset CMake Tools Extension State` then reload window, then retry.

## 4. Failure triage classifications

See `analysis/diagnostic-classification.md` for full classification table. Quick reference:

| Error | Classification | Fix |
|---|---|---|
| "No configure preset is active" | `ACTIVE_SELECTION_MISSING` | Verify `cmake.configurePreset: "default"` in folder and workspace settings |
| "No such preset" | `MISSING_PRESET` | Use preset name `default`; verify `CMakePresets.json` |
| "Could not read presets: File not found" | `SOURCE_DIR_MISMATCH` | Run from repo root; for tests folder set `cmake.sourceDirectory: "${workspaceFolder}/.."` |
| Nothing shows in CMake Tools status bar | `NON_CMAKE_FOLDER_FOCUS` | Switch active folder to `(Monorepo) Banana` or `(Monorepo) Tests` |

## 5. Fixed Config Files (applied 2026-04-26)

| File | Change |
|---|---|
| `.vscode/settings.json` | Added `cmake.configureOnOpen: false` |
| `tests/.vscode/settings.json` | Added `cmake.configureOnOpen: false` |
| `Banana.code-workspace` | Added `cmake.configurePreset`, `cmake.buildPreset`, `cmake.testPreset`, `cmake.configureOnOpen: false` |

## 6. Closure Evidence

All acceptance evidence is captured in `analysis/`:

- `us1-configure-evidence.md` — configure + build pass
- `us2-target-discovery-evidence.md` — all native targets discoverable
- `us2-test-discovery-evidence.md` — 4 tests discovered
- `us2-native-lane-evidence.md` — 4/4 pass
- `us3-missing-preset-evidence.md` — actionable error for missing preset
- `us3-context-mismatch-evidence.md` — actionable error for wrong directory
