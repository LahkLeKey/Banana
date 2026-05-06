# Slice 017 -- Native lane evidence

**Constraint (P-R02)**: Native lane stays 7/7. ABI version unchanged at 2.2.

## Build + test

```
cmake --preset default
cmake --build --preset default
ctest --preset default --output-on-failure
```

## Result

```
    Start 1: banana_test_calc
1/7 Test #1: banana_test_calc ..................   Passed
    Start 2: banana_test_message
2/7 Test #2: banana_test_message ...............   Passed
    Start 3: banana_test_profile
3/7 Test #3: banana_test_profile ...............   Passed
    Start 4: banana_test_dal
4/7 Test #4: banana_test_dal ...................   Passed
    Start 5: banana_test_ml_regression
5/7 Test #5: banana_test_ml_regression .........   Passed
    Start 6: banana_test_ml_binary
6/7 Test #6: banana_test_ml_binary .............   Passed
    Start 7: banana_test_ml_transformer
7/7 Test #7: banana_test_ml_transformer ........   Passed

100% tests passed, 0 tests failed out of 7
```

## ABI

- No edits under `src/native/**` for slice 017.
- `BANANA_NATIVE_ABI_VERSION` remains `2.2` (set in slice 012).
- `banana_classify_banana_transformer_ex` was the slice-012 export and is
  the sole native entry point bound by 017's managed P/Invoke.

## Conclusion

Native lane 7/7 unchanged; ABI 2.2 unchanged. P-R02 satisfied.
