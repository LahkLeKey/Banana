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

## 3. Validate CMake Tools lane (primary bug closure lane)

- Run `Build_CMakeTools`
- Run `ListBuildTargets_CMakeTools`
- Run `ListTests_CMakeTools`
- Run `RunCtest_CMakeTools`

Expected:

- Configure/build succeeds with active preset
- Native targets are listed
- Native tests are listed
- Native tests execute successfully

## 4. Failure triage classifications

If configure fails, classify as:

- missing preset activation
- invalid preset mapping
- workspace-context mismatch

Record evidence in feature analysis artifacts and proceed only when CMake Tools lane is green.
