# In-scope files — 011 Right Brain Binary

## Source under change

- `src/native/core/domain/ml/binary/banana_ml_binary.h`
- `src/native/core/domain/ml/binary/banana_ml_binary.c`
- `src/native/wrapper/domain/ml/binary/banana_wrapper_ml_binary.h`
- `src/native/wrapper/domain/ml/binary/banana_wrapper_ml_binary.c`
- `src/native/wrapper/banana_wrapper.h` (additive ABI export for `_with_threshold`)
- `src/native/wrapper/banana_wrapper.c` (additive export glue)

## Tests under change

- `tests/native/test_ml_binary.c` (NEW — US1/US2/US3/US4 cases)
- `tests/native/CMakeLists.txt` (register new ctest target + Windows DLL copy)

## Spec / analysis surfaces

- `.specify/specs/011-right-brain-binary-runnable/spec.md`
- `.specify/specs/011-right-brain-binary-runnable/plan.md`
- `.specify/specs/011-right-brain-binary-runnable/tasks.md`
- `.specify/specs/011-right-brain-binary-runnable/analysis/*.md`

## Out of scope

- ASP.NET pipeline wiring (downstream of this slice; covered by feature 003).
- TS/React clients (consumes JSON; no contract change here).
- Postgres DAL (binary classifier is text-only at this layer).
