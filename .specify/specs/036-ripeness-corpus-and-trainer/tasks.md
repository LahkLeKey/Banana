# Tasks: Ripeness Corpus + Trainer

**Branch**: `036-ripeness-corpus-and-trainer` | **Created**: 2026-04-26
**Status**: Draft. Hard prerequisites: SPIKE 034 + spec 035.

## Phase 1 -- Setup

- [ ] T001 Repoint `.specify/feature.json` -> `036-ripeness-corpus-and-trainer`.
- [ ] T002 Create branch `feature/036-ripeness-corpus-and-trainer`
  from spec 035 head.

## Phase 2 -- Corpus

- [ ] T003 Author `data/ripeness/corpus.json` with >=8 samples
  per label (`unripe` / `ripe` / `overripe` / `spoiled`) and a
  `splits` block ensuring all four labels appear in eval.

## Phase 3 -- Trainer

- [ ] T004 Add `scripts/train-ripeness-model.py` mirroring the
  banana trainer CLI surface; multi-class macro-F1 gating.
- [ ] T005 Reuse the determinism pattern from spec 035 (seed RNG,
  sorted emission, normalized metadata).
- [ ] T006 Run the trainer twice with the same flags; assert
  byte-identical output (FR-036-04).

## Phase 4 -- Image management

- [ ] T007 Add `scripts/manage-ripeness-image.py` mirroring the
  banana / not-banana manage scripts.
- [ ] T008 Promote the first ripeness model image to
  `data/ripeness/images/<sha>.tar` + channel pointer JSON;
  commit.

## Phase 5 -- Docs + close-out

- [ ] T009 Author `README.md` execution tracker.
- [ ] T010 Add `analysis/multi-class-gating-notes.md` documenting
  the macro-F1 interpretation + chosen threshold rationale.
- [ ] T011 Run `python scripts/validate-ai-contracts.py` and
  `bash scripts/validate-spec-tasks-parity.sh --all`.
- [ ] T012 Mark all tasks `[x]` and update README to COMPLETE.

## Out of scope

- Wiring ripeness into the API ensemble pipeline.
- Image-based ripeness inference.
- Cross-model attention.
