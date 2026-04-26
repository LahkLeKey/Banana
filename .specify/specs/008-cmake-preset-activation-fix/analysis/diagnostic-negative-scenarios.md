# Diagnostic Negative Scenarios: CMake Preset Activation Fix

**Date**: 2026-04-26  
**Purpose**: Define the negative scenarios to validate actionable diagnostics output.

## Scenario 1: Missing Preset (MISSING_PRESET)

**Setup**: Reference a non-existent preset name in CLI or settings.

**CLI simulation**:
```bash
cmake --preset nonexistent-preset
```

**Expected output**: Error identifying the preset name is not in `CMakePresets.json`.

**Evidence file**: `us3-missing-preset-evidence.md`

---

## Scenario 2: Context Mismatch / Non-CMake Folder (NON_CMAKE_FOLDER_FOCUS)

**Setup**: Run cmake CLI from a non-CMake directory (e.g., `src/native`), which has no `CMakeLists.txt` at root or `CMakePresets.json`.

**CLI simulation**:
```bash
cd src/native && cmake --preset default
```

**Expected output**: Error indicating no CMake project found or preset file not found from that path.

**Evidence file**: `us3-context-mismatch-evidence.md`

---

## Acceptance Criteria

- Both negative scenarios produce actionable error messages that match the `diagnostic-classification.md` entries.
- No scenario causes silent failure or confusing output.
