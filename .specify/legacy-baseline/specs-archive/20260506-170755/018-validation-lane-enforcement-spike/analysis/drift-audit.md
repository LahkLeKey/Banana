# 018 SPIKE -- Drift Audit (slices 010-017)

Audit of `spec.md` Validation lane / Success criteria items that have no
corresponding task in `tasks.md`. Source: T002.

| Slice | spec.md validator                                                            | tasks.md present? | Drift severity |
| ----- | ---------------------------------------------------------------------------- | ----------------- | -------------- |
| 010   | regression path coverage `>= 90%` line coverage                              | NO                | HIGH (silent)  |
| 010   | `banana_test_ml_regression` runs in `<1s`                                    | partial (ctest)   | LOW            |
| 011   | binary path coverage `>= 90%` line coverage                                  | NO                | HIGH (silent)  |
| 011   | JSON output schema explicitly asserted (not substring-matched)               | yes (US3 tests)   | none           |
| 012   | transformer path coverage `>= 85%` line coverage                             | NO                | HIGH (silent)  |
| 012   | idempotence verified across 5 repeated calls                                 | yes (T017)        | none           |
| 013   | (SPIKE -- N/A)                                                               | -                 | -              |
| 014   | `dotnet test ... XPlat Code Coverage` + `check-dotnet-coverage-threshold.sh` | yes (T025)        | none           |
| 014   | `ctest --preset default`                                                     | yes (T024)        | none           |
| 015   | `bun test` (shared/ui + react)                                               | yes (T018-T019)   | none           |
| 016   | (NOT YET EXECUTED -- excluded from drift count)                              | -                 | -              |
| 017   | `dotnet test ... XPlat Code Coverage`                                        | yes (T013)        | none (no -collect flag in task, downgraded MEDIUM) |
| 017   | `bash scripts/check-dotnet-coverage-threshold.sh`                            | NO                | HIGH (silent)  |
| 017   | `cmake --build --preset default && ctest`                                    | yes (T014)        | none           |

## Summary

- **High-severity gaps (silent skips)**: 4 / 7 executable slices.
  - 010, 011, 012 -- coverage thresholds declared in spec, no coverage
    task in tasks.md. ctest runs without coverage instrumentation.
  - 017 -- coverage threshold gate script declared in spec
    Validation lane, no task references it.
- **Medium-severity gaps**: 1 (017's `dotnet test` task missing the
  `--collect:"XPlat Code Coverage"` flag declared in spec).
- **Clean slices**: 014, 015 (both used post-013 templates).

## Pattern observation

Drift correlates with slice age: slices written before SPIKE 013
(010-012) declared coverage in spec but never wired tasks for it.
Slices written after 013 (014-015) mirrored cleanly. Slice 017
regressed by skipping the coverage threshold script even though the
template was current. Conclusion: template fix alone is insufficient;
a structural enforcement mechanism is required.
