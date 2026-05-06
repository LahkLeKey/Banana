# Native lane evidence — 010 Left Brain Regression Runnable

Date: 2026-04
Preset: `default` (Ninja + clang, output `build/cmake-tools`)

## Build

```
cmake --preset default
cmake --build --preset default
```

Result: clean build (only the new test target rebuilt; banana_native DLL
relinked once to pick up the documented header includes).

## Test (ctest)

```
ctest --preset default --output-on-failure
```

Result:

```
1/5 Test #1: banana_test_calc                  Passed
2/5 Test #2: banana_test_ml_models             Passed
3/5 Test #3: banana_test_operational_domains   Passed
4/5 Test #4: banana_test_dal_contracts         Passed
5/5 Test #5: banana_test_ml_regression         Passed

100% tests passed, 0 tests failed out of 5
```

## Net delta

- New ctest target: `banana_test_ml_regression` (10 assertions across US1/US2/US3).
- Existing 4 native tests remain green; no regressions introduced.
- New target follows the existing Windows POST_BUILD pattern that copies
  `$<TARGET_FILE:banana_native>` next to the test executable.

## Files of record

- `tests/native/test_ml_regression.c`
- `tests/native/CMakeLists.txt` (added the 5th target + DLL copy step)
- `src/native/core/domain/ml/regression/banana_ml_regression.{h,c}` (contract
  docs + NaN/Inf guard)
- `src/native/wrapper/domain/ml/regression/banana_wrapper_ml_regression.h`
  (consumer threshold guidance)
