# US2 Test Discovery Evidence: CMake Preset Activation Fix

**Date**: 2026-04-26
**Command**: `ctest -N` (from `build/cmake-tools`)

## Output

```
Test project C:/Github/Banana/build/cmake-tools
  Test #1: banana_test_calc
  Test #2: banana_test_ml_models
  Test #3: banana_test_operational_domains
  Test #4: banana_test_dal_contracts

Total Tests: 4
```

## Verification

| Expected Test | Discovered? |
|---|---|
| `banana_test_calc` | ✅ Yes (#1) |
| `banana_test_ml_models` | ✅ Yes (#2) |
| `banana_test_operational_domains` | ✅ Yes (#3) |
| `banana_test_dal_contracts` | ✅ Yes (#4) |

**Result**: ✅ PASS — all 4 expected native tests are discoverable.
