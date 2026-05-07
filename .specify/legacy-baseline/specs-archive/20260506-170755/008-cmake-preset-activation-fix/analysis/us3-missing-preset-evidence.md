# US3 Missing Preset Evidence: CMake Preset Activation Fix

**Date**: 2026-04-26
**Scenario**: MISSING_PRESET — reference a preset name that does not exist in `CMakePresets.json`.
**Command**: `cmake --preset nonexistent-preset` (from repo root)

## Output

```
CMake Error: No such preset in C:/Github/Banana: "nonexistent-preset"
Available configure presets:

  "default" - Banana Default
exit: 1
```

## Analysis

| Diagnostic Property | Value |
|---|---|
| Error message | ✅ Clear — names the missing preset |
| Available presets listed | ✅ Yes — `default` is shown |
| Exit code | ✅ 1 (failure) |
| Actionable? | ✅ Yes — user immediately knows to use `default` |

**Classification**: `MISSING_PRESET` per `diagnostic-classification.md`

**Result**: ✅ PASS — diagnostic output is actionable and matches expected classification.
