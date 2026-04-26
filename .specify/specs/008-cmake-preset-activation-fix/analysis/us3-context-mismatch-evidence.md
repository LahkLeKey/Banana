# US3 Context Mismatch Evidence: CMake Preset Activation Fix

**Date**: 2026-04-26  
**Scenario**: NON_CMAKE_FOLDER_FOCUS / SOURCE_DIR_MISMATCH — invoke cmake from a subdirectory without `CMakePresets.json`.  
**Command**: `cmake --preset default` (from `src/native/`, which has no `CMakePresets.json`)

## Output

```
CMake Error: Could not read presets from C:/Github/Banana/src/native:
File not found: C:/Github/Banana/src/native/CMakePresets.json
exit: 1
```

## Analysis

| Diagnostic Property | Value |
|---|---|
| Error message | ✅ Clear — names the missing preset file |
| Path shown | ✅ Yes — full path `src/native/CMakePresets.json` |
| Exit code | ✅ 1 (failure) |
| Actionable? | ✅ Yes — user knows to run from repo root |

**Classification**: `SOURCE_DIR_MISMATCH` / `NON_CMAKE_FOLDER_FOCUS` per `diagnostic-classification.md`

**Guidance**: Always invoke `cmake --preset default` from the repo root (`/c/Github/Banana`). For the `(Monorepo) Tests` CMake Tools context, `cmake.sourceDirectory` re-binds to the repo root automatically.

**Result**: ✅ PASS — diagnostic output is actionable and matches expected classification.
