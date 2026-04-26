# CMake Tools Configure Acceptance Checklist (US1)

**Date**: 2026-04-26  
**Purpose**: Gate to confirm CMake Tools configure activates correctly in supported workspace contexts.

## Pre-Fix Required Changes

- [ ] Root `.vscode/settings.json` has `cmake.configureOnOpen: false`
- [ ] `tests/.vscode/settings.json` has `cmake.configureOnOpen: false`
- [ ] `Banana.code-workspace` has `cmake.configurePreset: "default"`
- [ ] `Banana.code-workspace` has `cmake.buildPreset: "default"`
- [ ] `Banana.code-workspace` has `cmake.testPreset: "default"`
- [ ] `Banana.code-workspace` has `cmake.configureOnOpen: false`
- [ ] `CMakePresets.json` contains configure preset named `default`
- [ ] `CMakePresets.json` contains build preset named `default`
- [ ] `CMakePresets.json` contains test preset named `default`

## Post-Fix Acceptance Scenarios

### Scenario 1: Root folder configure

- [ ] VS Code workspace opened with `(Monorepo) Banana` folder focused
- [ ] CMake Tools status bar shows preset `default`
- [ ] Configure succeeds without "No configure preset is active" error
- [ ] OR: CLI evidence confirms `cmake --preset default` succeeds (acceptable alternate path)

### Scenario 2: Tests folder configure

- [ ] `(Monorepo) Tests` folder activated
- [ ] `cmake.sourceDirectory` correctly resolved to repo root
- [ ] Configure uses same `default` preset as root context
- [ ] OR: CLI evidence confirms same CLI path applies

### Scenario 3: Non-CMake folder (no-op)

- [ ] A non-CMake folder (e.g., `(C) Native`) is focused
- [ ] CMake Tools shows no active project (not an error, expected)
- [ ] No spurious configure attempt is triggered

## Evidence Required

- [ ] `us1-configure-evidence.md` captured with configure result
