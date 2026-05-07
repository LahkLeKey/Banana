# Plan: React `BananaBadge` Ensemble Variant

**Branch**: `015-react-ensemble-badge` | **Date**: 2026-04-26
**Depends on**: 014 (merged) -> consumes `EnsembleVerdictResult` shape.

## Phases

- **Phase 1 (T001-T003)** Setup -- repoint `.specify/feature.json`,
  `README.md` tracker, in-scope files.
- **Phase 2 (T004-T005)** Foundational types -- add `EnsembleVerdict` to
  `shared/ui/src/types.ts`; assert shape against the 014 snapshot.
- **Phase 3 US1 (T006-T009)** Ensemble badge variant + tests across the
  six SPIKE anchor walks.
- **Phase 4 US2 (T010-T011)** Escalation pill + tests.
- **Phase 5 US3 (T012-T014)** Opt-in attention chip behind
  `VITE_BANANA_SHOW_ATTENTION=1`; default-off UI byte-equivalence test.
- **Phase 6 (T015-T017)** API helper + snapshot test of the ensemble shape.
- **Phase 7 (T018-T020)** Validation (`bun test` in both packages),
  evidence files, close `tasks.md`.

## Out of scope

- Native or managed changes.
- Electron preload bridge.
- New Bun packages.
