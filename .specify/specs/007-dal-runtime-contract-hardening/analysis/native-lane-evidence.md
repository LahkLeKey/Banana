# Native Lane Evidence

- Date: 2026-04-26
- Feature: 007-dal-runtime-contract-hardening
- Lane: tests/native (CMake Tools)
- Commands attempted:
  - `Build_CMakeTools`
  - `RunCtest_CMakeTools`
- Native lane fallback command executed:
  - `cmake --preset default && cmake --build --preset default && ctest --preset default --output-on-failure`
- Scenarios covered:
  - Native DAL contract lane invocation attempt for `tests/native/test_dal_contracts.c`
- Expected signal: native test lane runs and reports pass/fail outcomes
- Observed result: PASS (`100% tests passed, 0 tests failed out of 4`)
- Status: PASS
- Notes:
  - CMake Tools still reports no active preset in this session, but preset-driven CLI lane is green.
  - `banana_test_dal_contracts` executed and passed with expected DAL contract assertions.
