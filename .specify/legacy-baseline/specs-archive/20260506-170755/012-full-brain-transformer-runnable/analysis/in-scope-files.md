# In-scope files -- 012 Full Brain Transformer

## Source under change

- `src/native/core/domain/ml/transformer/banana_ml_transformer.h`
- `src/native/core/domain/ml/transformer/banana_ml_transformer.c`
- `src/native/wrapper/domain/ml/transformer/banana_wrapper_ml_transformer.h`
  (documentation only; no signature change required)

## Tests under change

- `tests/native/test_ml_transformer.c` (NEW -- US1/US2/US3/US4 cases)
- `tests/native/CMakeLists.txt` (register new ctest target + Windows DLL copy)

## Spec / analysis surfaces

- `.specify/specs/012-full-brain-transformer-runnable/spec.md`
- `.specify/specs/012-full-brain-transformer-runnable/plan.md`
- `.specify/specs/012-full-brain-transformer-runnable/tasks.md`
- `.specify/specs/012-full-brain-transformer-runnable/analysis/*.md`

## Out of scope

- ASP.NET pipeline composition (downstream; covered by 003 + 004).
- Quantized embedding (Future U-001); captured in `residual-followups.md`.
- Public ABI bump (no new wrapper export needed for this slice; the
  attention/embedding diagnostics live behind a core entry point with an
  options struct so the public ABI stays stable at 2.1).
