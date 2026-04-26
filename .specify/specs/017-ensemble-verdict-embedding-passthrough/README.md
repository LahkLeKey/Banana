# 017 Ensemble Verdict Embedding Passthrough -- Execution Tracker

**Status**: COMPLETE (15/15). Slice 016 trigger FIRED.

## Validation evidence

- Unit tests: `dotnet test tests/unit/Banana.Api.Tests.Unit.csproj` -> 69/69 pass (60 from slice 014 + 9 new in `EnsembleEmbeddingPassthroughTests`).
- Native lane: `ctest --preset default --output-on-failure` -> 7/7 pass.
- ABI: stayed at 2.2 (no native changes).
- Legacy `/ml/ensemble` shape: byte-locked via `LegacyEnsembleRoute_ShapeUnchanged_NoEmbeddingField`.
- Slice 016 trigger: flipped to `trigger-fired` in `.specify/specs/013-ml-brain-composition-spike/analysis/deferred-registry.md`.

## Purpose

Plumb the existing native 4-dim transformer embedding through the
ASP.NET surface as an opt-in route, creating the cross-process consumer
that fires slice 016's deferred-registry trigger.

## Why no SPIKE

The native ABI side was already designed and tested in slice 012
(see `012-full-brain-transformer-runnable/analysis/us3-embedding-helper-evidence.md`).
SPIKE 013's followup-C packet documents the contract. This slice is
purely additive managed plumbing -- no new design, no new contract.

## Phases (mapped to tasks.md)

1. Setup (T001-T002)
2. Foundational interop (T003-T004)
3. Result type + route (T005-T009)
4. Tests (T010-T013)
5. Validation + close-out (T014-T015)

## Constraints

- No native changes; ctest stays 7/7; ABI stays 2.2.
- Slice 014 `/ml/ensemble` shape is byte-frozen.

## Downstream

Merging 017 fires the slice 016 trigger in
[../013-ml-brain-composition-spike/analysis/deferred-registry.md](../013-ml-brain-composition-spike/analysis/deferred-registry.md).
