# Native lane evidence -- 012 Full Brain Transformer

## Command
```
cmake --preset default
cmake --build --preset default
ctest --preset default --output-on-failure
```

## Result (post-slice)

```
Test project C:/Github/Banana/build/cmake-tools
    Start 1: banana_test_calc
1/7 Test #1: banana_test_calc ..................   Passed    0.03 sec
    Start 2: banana_test_ml_models
2/7 Test #2: banana_test_ml_models .............   Passed    0.03 sec
    Start 3: banana_test_operational_domains
3/7 Test #3: banana_test_operational_domains ...   Passed    0.03 sec
    Start 4: banana_test_dal_contracts
4/7 Test #4: banana_test_dal_contracts .........   Passed    0.03 sec
    Start 5: banana_test_ml_regression
5/7 Test #5: banana_test_ml_regression .........   Passed    0.03 sec
    Start 6: banana_test_ml_binary
6/7 Test #6: banana_test_ml_binary .............   Passed    0.03 sec
    Start 7: banana_test_ml_transformer
7/7 Test #7: banana_test_ml_transformer ........   Passed    0.05 sec

100% tests passed, 0 tests failed out of 7
```

## Delta vs prior slice
- Was 6/6 after 011 (added `banana_test_ml_binary`).
- Now 7/7 after 012 (added `banana_test_ml_transformer`).
- No regressions in the previously passing 6 targets.
