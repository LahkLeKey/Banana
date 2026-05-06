# Plan: Ensemble Verdict Embedding Passthrough

**Branch**: `017-ensemble-verdict-embedding-passthrough` | **Date**: 2026-04-26
**Status**: Ready -- no SPIKE required (additive managed plumbing only).

## Strategy

Pure managed plumbing. The native `_ex` export already returns the 4-dim
embedding (locked by slice 012's `us3-embedding-helper-evidence.md`).
This slice exposes it through ASP.NET as an opt-in route so a
cross-process consumer exists for slice 016's gate to fire.

## Phases

- **Phase 1 (T001-T002)** Setup -- repoint `.specify/feature.json`,
  scaffold tracker.
- **Phase 2 (T003-T004)** Foundational -- add
  `ClassifyBananaTransformerWithEmbedding` to `INativeBananaClient` and
  `NativeBananaClient`; extend `FakeNativeBananaClient`.
- **Phase 3 US1+US2 (T005-T009)** Add
  `EnsembleVerdictWithEmbeddingResult`, new escalation step variant or
  controller helper, and `POST /ml/ensemble/embedding` route.
- **Phase 4 (T010-T013)** Tests -- 6 anchor walks, embedding null on
  cheap path, embedding length=4 on escalation, legacy route snapshot.
- **Phase 5 (T014-T015)** Validation + close-out (ctest 7/7,
  `dotnet test`, coverage gate). Update
  `013-ml-brain-composition-spike/analysis/deferred-registry.md` to
  flip 016's trigger to FIRED.

## Constraints

- No `src/native/**` changes. ABI stays at 2.2.
- Slice 014's `/ml/ensemble` JSON shape is byte-frozen; new route is
  additive.
- Reuse existing `EnsembleGatingStep` / `EnsembleEscalationStep` /
  `EnsembleCalibrationStep` -- do NOT duplicate cascade logic.
