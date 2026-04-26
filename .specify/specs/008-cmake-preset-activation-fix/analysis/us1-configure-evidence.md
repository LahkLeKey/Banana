# US1 Configure Evidence: CMake Preset Activation Fix

**Date**: 2026-04-26  
**Purpose**: Capture evidence that configure succeeds using the `default` preset.

## Pre-Fix Config Changes Applied

| File | Change |
|---|---|
| `.vscode/settings.json` | Added `cmake.configureOnOpen: false` |
| `tests/.vscode/settings.json` | Added `cmake.configureOnOpen: false` |
| `Banana.code-workspace` | Added `cmake.configurePreset`, `cmake.buildPreset`, `cmake.testPreset`, `cmake.configureOnOpen: false` |

## CMakePresets.json Validation

```
version: 6
configurePresets: ['default']
buildPresets: ['default']
testPresets: ['default']
configure/default generator: Ninja
configure/default binaryDir: ${sourceDir}/build/cmake-tools
configure/default CMAKE_C_COMPILER: clang
```

**Result**: ✅ PASS — preset catalog is complete and correct.

## CLI Configure Result

```
cmake --preset default
```

```
-- Configuring done (0.0s)
-- Generating done (0.0s)
-- Build files have been written to: C:/Github/Banana/build/cmake-tools
```

**Result**: ✅ PASS — configure succeeded using preset `default`.

## CLI Build Result

```
cmake --build --preset default
```

```
ninja: no work to do.
```

**Result**: ✅ PASS — build completed (no-op since already built; no errors).

## CMake Tools Extension Assessment

**Status**: Extension-integrated flow remains subject to VS Code session state.

**Disposition**: Per research.md R-03, CLI-path evidence is the reliable closure path. CMake Tools extension behavior depends on which workspace folder is focused and whether prior stale state has been cleared.

**Actionable guidance for CMake Tools users**:
1. Ensure `(Monorepo) Banana` or `(Monorepo) Tests` folder is the active folder.
2. If "No configure preset is active" appears after a VS Code reload, run `CMake: Reset CMake Tools Extension State` then reload window.
3. All three config files now include both active-selection preset keys and `cmake.configureOnOpen: false`.

## Acceptance Verdict

| Scenario | Result |
|---|---|
| CLI configure using preset | ✅ PASS |
| CLI build using preset | ✅ PASS |
| Preset catalog complete | ✅ PASS |
| Config file alignment complete | ✅ PASS |
