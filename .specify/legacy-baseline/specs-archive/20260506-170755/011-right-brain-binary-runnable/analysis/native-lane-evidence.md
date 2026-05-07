# Native lane evidence -- 011 Right Brain Binary Runnable

Date: 2026-04
Preset: `default` (Ninja + clang, output `build/cmake-tools`)

## Build

```
cmake --preset default
cmake --build --preset default
```

Result: clean build (only pre-existing MSVC `_CRT_DEPRECATE_TEXT` warnings
from system headers; no new warnings introduced).

## Test (ctest)

```
ctest --preset default --output-on-failure
```

Result:

```
1/6 Test #1: banana_test_calc                  Passed
2/6 Test #2: banana_test_ml_models             Passed
3/6 Test #3: banana_test_operational_domains   Passed
4/6 Test #4: banana_test_dal_contracts         Passed
5/6 Test #5: banana_test_ml_regression         Passed
6/6 Test #6: banana_test_ml_binary             Passed

100% tests passed, 0 tests failed out of 6
```

## Net delta

- New ctest target: `banana_test_ml_binary` (10 assertions across US1..US4).
- Public ABI gained one additive symbol:
  `banana_classify_banana_binary_with_threshold` (minor bump 2.0 -> 2.1).
- Existing 5 native tests remain green; no behavior regressions.

## US4 JSON contract field-locks (asserted via substring presence)

- `"model":"binary"`
- `"label":`
- `"confidence":`
- `"banana_score":`
- `"not_banana_score":`
- `"jaccard":`
- `"confusion_matrix":{`, `"tp":`, `"fp":`, `"fn":`, `"tn":`

## Files of record

- `src/native/core/domain/ml/binary/banana_ml_binary.{h,c}`
- `src/native/wrapper/domain/ml/binary/banana_wrapper_ml_binary.{h,c}`
- `src/native/wrapper/banana_wrapper.{h,c}` (additive `_with_threshold`)
- `tests/native/test_ml_binary.c` (NEW)
- `tests/native/CMakeLists.txt` (registered 6th target + DLL copy)
