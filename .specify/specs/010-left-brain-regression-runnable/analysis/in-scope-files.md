# In-Scope Files: 010 Left Brain Regression Runnable C Code

| Path | Layer | Change Type |
|---|---|---|
| `src/native/core/domain/ml/regression/banana_ml_regression.h` | Native core (header) | Document input/output contract |
| `src/native/core/domain/ml/regression/banana_ml_regression.c` | Native core (impl) | Add NaN/Inf rejection guard |
| `src/native/wrapper/domain/ml/regression/banana_wrapper_ml_regression.h` | Native wrapper (header) | Document consumer usage pattern |
| `tests/native/test_ml_regression.c` | Native test (new) | Range, boundary, calibration anchors, null/NaN guards |
| `tests/native/CMakeLists.txt` | CMake (test) | Register `banana_test_ml_regression` target |

## Out of Scope

- Right Brain binary (covered by 011)
- Full Brain transformer (covered by 012)
- ASP.NET / React / mobile surfaces
- Model retraining or coefficient tuning
