# Discovery Expected Surfaces: CMake Preset Activation Fix

**Date**: 2026-04-26
**Purpose**: Define the expected target and test surfaces that must be discoverable after configure.

## Expected Build Targets

| Target | Type | Description |
|---|---|---|
| `banana_native` | Library | Native banana shared library |
| `banana_test_calc` | Executable | Calculation unit tests |
| `banana_test_ml_models` | Executable | ML model tests |
| `banana_test_operational_domains` | Executable | Operational domain tests |
| `banana_test_dal_contracts` | Executable | DAL contract tests (added in 007) |

## Expected CTest Tests

| Test # | Name | Description |
|---|---|---|
| 1 | `banana_test_calc` | Calculation tests |
| 2 | `banana_test_ml_models` | ML model tests |
| 3 | `banana_test_operational_domains` | Operational domain tests |
| 4 | `banana_test_dal_contracts` | DAL contract tests |

## Acceptance Criteria

- All expected build targets appear in `cmake --build --preset default --target help`
- All expected test names appear in `ctest -N` from build directory
- All tests pass when `ctest --preset default --output-on-failure` is executed
