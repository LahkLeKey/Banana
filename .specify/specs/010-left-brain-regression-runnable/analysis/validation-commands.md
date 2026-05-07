# Validation Commands: 010 Left Brain Regression Runnable C Code

## Build + Configure

```bash
cd /c/Github/Banana
cmake --preset default
cmake --build --preset default
```

## Run Native Lane

```bash
ctest --preset default --output-on-failure
```

## Run Just the New Target

```bash
cd /c/Github/Banana/build/cmake-tools
ctest -R banana_test_ml_regression --output-on-failure
```

## Discovery

```bash
cd /c/Github/Banana/build/cmake-tools
ctest -N
```

Expected after this slice: 5 tests (`banana_test_calc`, `banana_test_ml_models`, `banana_test_operational_domains`, `banana_test_dal_contracts`, `banana_test_ml_regression`).
